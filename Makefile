USE_EMBED_KEYWORD = 0 # Set this to 1 if you want to embed data using C23's #embed feature
PYTHON = python # 'python' should be enough if you use a venv or are on Windows, but on linux it might run python2 instead of python3

ifeq ($(OS), Windows_NT)
	CFLAGS += -lwinmm
endif

ifeq ($(USE_EMBED_KEYWORD), 1)
	CFLAGS += -std=c23 -DEMBED_USING_C23_EMBED_KEYWORD
endif

.PHONY: all
all:
	@echo "Run \`make compile_assets && make build\` to build everything."

.PHONY: clean
clean:
	-rm -rf ./frames/
	-rm -f ./bapple.exe ./bapple
	-rm -f ./map.bin ./embed.c

.PHONY: compile_assets
compile_assets: generate_frames generate_binary_data convert_binary_data_to_code

.PHONY: build
build:
	gcc bapple.c $(CFLAGS) -o ./bapple

.PHONY: run
run:
	./bapple

.PHONY: generate_frames
generate_frames:
	-mkdir frames
	ffmpeg -i ./badapple.mp4 -r 30 -vf scale=16:12 ./frames/output_%04d.png

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
