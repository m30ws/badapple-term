import glob
import sys

from PIL import Image

OUTPUT_DATA_FILE = f'map.bin'
FRAMES_DIR = './frames/'
FRAME_FILES_FORMAT = f'output_*.png'
FRAME_WIDTH, FRAME_HEIGHT = 16, 12

def export_bin(frames_glob, fp_out):
	total_frames = len(frames_glob)

	for idx, frame_file in enumerate(frames_glob):
		with Image.open(frame_file) as opened:
			img = opened.load()
			byt = 0
			byt_cnt = 0

			for pixel in range(FRAME_WIDTH * FRAME_HEIGHT):
				x, y = pixel % FRAME_WIDTH, pixel // FRAME_WIDTH

				byt = (byt << 1) | (0 if sum(img[x, y]) > 0 else 1)
				byt_cnt += 1

				if byt_cnt == 8:
					fp_out.write(bytes([ byt ]))
					byt, byt_cnt = 0, 0

		if idx % 500 == 0 and idx > 0:
			print(f'[{idx}/{total_frames}] written.')

	print(f'All written. ({total_frames} frames)')


## debug purposes
def output_sprite_map(frames_glob):
	OUTPUT_IMG_ROWS, OUTPUT_IMG_COLS = 83, 80
	OUTPUT_FILENAME = f'out_map.png'

	output: Image = Image.new("RGBA", (OUTPUT_IMG_COLS*FRAME_WIDTH, OUTPUT_IMG_ROWS*FRAME_HEIGHT), color=None)

	i = 0
	place_row, place_col = 0, 0
	for frame_file in frames_glob:
		with Image.open(frame_file) as img:                
			at = (place_col * FRAME_WIDTH, place_row * FRAME_HEIGHT)
			# Visual map
			output.paste(img, at, None)

		i += 1
		place_col += 1
		if (i % OUTPUT_IMG_COLS == 0):
			place_row += 1
			place_col = 0

	output.save(OUTPUT_FILENAME)


if __name__ == '__main__':
	frames_glob = glob.glob(f'{FRAMES_DIR}/{FRAME_FILES_FORMAT}')
	n_frames = len(frames_glob)
	# assert(OUTPUT_IMG_ROWS * OUTPUT_IMG_COLS >= n_frames)

	with open(OUTPUT_DATA_FILE, "wb") as fp_out:
		export_bin(frames_glob, fp_out)
