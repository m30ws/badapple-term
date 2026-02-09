INPUT_BIN = 'map.bin'
OUTPUT_C = 'embed.c'

# What will the variable be named in the resulting C code
VAR_NAME = 'EMBEDDED_MAP_VARNAME'
VAR_NAME_LEN = None

with open(INPUT_BIN, 'rb') as f:
	data = f.read()

with open(OUTPUT_C, 'w') as fp_out:
	print(f'const uint8_t {VAR_NAME}[] = {{', end='', file=fp_out)
	
	for i, byt in enumerate(data):
		if i % 16 == 0:
			print('\n\t', end='', file=fp_out)
		print(f'0x{byt:02x}, ', end='', file=fp_out)
	print(f'\n}};', file=fp_out)

	# if (VAR_NAME_LEN):
	# 	print(f'const size_t {VAR_NAME_LEN} = {len(data)};', file=fp_out)
