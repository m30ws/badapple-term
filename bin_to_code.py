INPUT_BIN = 'map.bin'
OUTPUT_C = 'embed.c'

# What will the variable be named in the resulting C code
VAR_NAME = 'EMBEDDED_MAP_VARNAME'
VAR_NAME_LEN = None

with open(INPUT_BIN, 'rb') as f:
	data = f.read()

with open(OUTPUT_C, 'wb') as fp_out:
	fp_out.write(f'const uint8_t {VAR_NAME}[] = {{'.encode())
	
	for i, byt in enumerate(data):
		if i % 16 == 0:
			fp_out.write(f'\n\t'.encode())
		fp_out.write(f'0x{byt:02x}, '.encode())
	fp_out.write(f'\n}};'.encode())

	# if (VAR_NAME_LEN):
	# 	fp_out.write(f'const size_t {VAR_NAME_LEN} = {len(data)};'.encode())
