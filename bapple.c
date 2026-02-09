/*
	this code is semi-generalized for frame width and height, which means
	it most likely isn't going to work for other sizes as-is.
*/
#ifndef EMBED_USING_C23_EMBED_KEYWORD
#define EMBED_USING_C23_EMBED_KEYWORD 0
#endif
#define RATE_PER_SECOND 30
#define LOOP_VIDEO true
#define W_SCALE 2 // stretches the output width leaving spaces between columns to look a bit better

#define FRAME_WIDTH 16
#define FRAME_HEIGHT 12
#define PIXEL_CHAR_BLACK "*"
#define PIXEL_CHAR_WHITE " "


#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <mmsystem.h>
#else
	#ifndef _POSIX_C_SOURCE
	#define _POSIX_C_SOURCE 199309L
	#endif
	#include <time.h> // nanosleep
#endif

#include <assert.h>
#include <inttypes.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	PIXEL_UNKNOWN,
	PIXEL_BLACK,
	PIXEL_WHITE,
} PixelColor;

typedef struct {
	int frame_width, frame_height;
	size_t n_frames;
	const uint8_t* data; // Each frame is `frame_width*frame_height` pixels, packed into bytes (so with 16x12 frame size each frame would be 24bytes of B/W pixels)
} SpriteMap;

#if defined(EMBED_USING_C23_EMBED_KEYWORD) && EMBED_USING_C23_EMBED_KEYWORD == 1
	static const uint8_t embedded_map[] = {
		#embed "map.bin"
	};	
#else
	#define EMBEDDED_MAP_VARNAME embedded_map
	#include "embed.c"
#endif

static bool playing = true; // signal handler feedback

const char* pixel_color_to_char(PixelColor col)
{
	switch (col) {
		case PIXEL_BLACK: return PIXEL_CHAR_BLACK;
		case PIXEL_WHITE: return PIXEL_CHAR_WHITE;
		default:          return "?";
	}
}

static inline void move_cursor(int row, int col)
{
	printf("\x1b[%d;%dH", row, col);
}

static inline void render_pixel(int row, int col, PixelColor color)
{
	move_cursor(row, col);
	printf("%s", pixel_color_to_char(color));
}

bool setup_things(void)
{
	printf("\e[?1049h"); // Use alternate screen buffer
#ifdef _WIN32
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
		goto log_and_return_error;

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
		goto log_and_return_error;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
		goto log_and_return_error;

	// Set the greatest resolution we can get for Sleep/SleepEx()
	timeBeginPeriod(1/*ms*/);
#endif

	// Plat-independed options //
	printf("\e[?25l"); // Hide the cursor
	printf("\e[2J"); // Clear screen

	return true;

#ifdef _WIN32
log_and_return_error:
	fprintf(stderr, "[setup_things] Errored with code: %lu\n", GetLastError());
	return false;
#endif
}

void restore_things(void)
{
#ifdef _WIN32
	timeEndPeriod(1/*ms, has to match timeBeginPeriod*/);
#endif
	printf("\e[?25h"); // Show the cursor
	printf("\e[2J"); // Clear screen
	move_cursor(0, 0);
	printf("\e[?1049l"); // Return to the main screen buffer
}

static inline void clear_canvas(int rows, int cols)
{
	assert(rows > 0 && cols > 0);
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
			render_pixel(r, c, PIXEL_WHITE);
}

void time_sleep(unsigned int sleep_for)
{
#ifdef _WIN32
	SleepEx(sleep_for, true);
#else
	nanosleep(&(struct timespec){ .tv_nsec = sleep_for * 1e3*1e3 }, NULL);
#endif
}

void play_video(SpriteMap data, int rate_per_second, bool loop_video)
{
	if (data.frame_width < 1 || data.frame_height < 1 || data.n_frames < 1)
		return;

	size_t frame_bits = data.frame_width * data.frame_height;
	size_t frame_bytes = frame_bits / 8;

	for (; playing; ) {

		for (size_t fr = 0; fr < data.n_frames && playing; fr++) {
			// clear_canvas(data.frame_height, data.frame_width);

			for (size_t offset = 0; offset < frame_bits; offset++) {
				size_t target_byte = fr * frame_bytes + offset / 8;
				int rr = offset / data.frame_width;
				int cc = offset % data.frame_width;
				
				size_t mask = (size_t)1 << (data.frame_width - cc - 1); // [!!!] This too makes it so the max columns is sizeof(size_t)
				if (cc < 8)
					mask >>= 8;

				render_pixel(rr, cc * W_SCALE + 1, (data.data[target_byte] & mask) ? PIXEL_BLACK : PIXEL_WHITE);
			}
			time_sleep(1000 / rate_per_second);
		}

		if (!loop_video) break;
	}
}

void sig_handler(int sig)
{
	playing = false;
}

int main(int argc, char** argv)
{
	if (!setup_things())
		return -1;

	signal(SIGINT,  sig_handler);
	signal(SIGTERM, sig_handler);

	SpriteMap data = {
		.frame_width = FRAME_WIDTH,
		.frame_height = FRAME_HEIGHT,
		.n_frames = sizeof(embedded_map) / (FRAME_WIDTH*FRAME_HEIGHT/8),
		.data = embedded_map,
	};

	play_video(data, RATE_PER_SECOND, LOOP_VIDEO);

	restore_things();
	printf("\nDone.\n");
	return 0;
}
