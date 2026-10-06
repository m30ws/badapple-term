# Just `make` to process assets & build the main program
# Needs python, ffmpeg and gcc available on the path (change in the following lines if different)
USE_EMBED_KEYWORD = 0 # Set this to 1 if you want to embed data using C23's #embed feature
PYTHON = python # 'python' should be enough if you use a venv or are on Windows, but on linux it might run python2 instead of python3
FFMPEG = ffmpeg
CC = gcc
CFLAGS = -Wall -Wextra -O2 # -static

SRC = ./bapple.c
OUT = ./bapple

ifeq ($(USE_EMBED_KEYWORD), 1)
	CFLAGS += -std=c23 -DEMBED_USING_C23_EMBED_KEYWORD
endif

ifeq ($(OS), Windows_NT)
	CFLAGS += -lwinmm
endif

.PHONY: all
all: compile_assets build

.PHONY: clean
clean:
	-rm -rf ./frames/
	-rm -f $(OUT) $(OUT).exe
	-rm -f ./map.bin ./out_map.png ./embed.c

.PHONY: compile_assets
compile_assets: generate_frames generate_binary_data convert_binary_data_to_code

.PHONY: build
build: compile_assets build_only


.PHONY: build_only
build_only:
	$(CC) $(SRC) $(CFLAGS) -o $(OUT)

.PHONY: run
run:
	./$(OUT)

.PHONY: generate_frames
generate_frames:
	-mkdir frames
	$(FFMPEG) -i ./badapple.mp4 -r 30 -vf scale=16:12 ./frames/output_%04d.png

.PHONY: generate_binary_data
generate_binary_data: 
	$(VENV_CMD_PREFIX) $(PYTHON) compile_map.py

.PHONY: convert_binary_data_to_code
ifeq ($(USE_EMBED_KEYWORD), 1)
convert_binary_data_to_code: 
else
convert_binary_data_to_code: 
	$(VENV_CMD_PREFIX) $(PYTHON) bin_to_code.py
endif
