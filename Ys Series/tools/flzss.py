# Falcom LZSS Library
# Reimplemented native Python version of x2_data_uncomp by EsperKnight
import os,sys,struct
COMPRESSED_BUFFER = bytearray(0x10000)
UNCOMPRESSED_BUFFER = bytearray(0x10000)
C_POS = 0x00
U_POS = 0x00
FLAG = 0x00
FLAG_POS = 0x00

def get_compressed_byte():
	global C_POS
	global COMPRESSED_BUFFER
	cb = COMPRESSED_BUFFER[C_POS]
	C_POS+=1
	return cb

# Read the next two bytes from compressed buffer as the flag.
def set_flags():

	global FLAG
	global FLAG_POS
	global COMPRESSED_BUFFER
	global C_POS
	FLAG = struct.unpack("<H",COMPRESSED_BUFFER[C_POS:C_POS+2])[0]
	C_POS+=2

	FLAG_POS = 0x10


	
# If flag position is zero, we need a new flag.
def get_flag():

	global FLAG_POS
	global FLAG

	
	if(FLAG_POS == 0x00):
		set_flags()

	is_flag = ((FLAG & 0x01) == 1)
	FLAG = FLAG >> 1
	if(FLAG_POS > 0):
		FLAG_POS-=1


	return is_flag

def uncompress(prev_u_pos,run):

	global U_POS
	global UNCOMPRESSED_BUFFER
	if(run != 0):
		for i in range(0,run):
			UNCOMPRESSED_BUFFER[U_POS] = UNCOMPRESSED_BUFFER[U_POS - prev_u_pos]
			U_POS+=1

			
def setup_run(prev_u_pos):

	run = 0x02
	if(not get_flag()):

		run+=1
		if(not get_flag()):

			run+=1
			if(not get_flag()):

				run+=1
				if(not get_flag()):

					if(not get_flag()):

						run = get_compressed_byte() + 0x0E
						
						uncompress(prev_u_pos,run)						
					else:
						run = 0
						run |= (1 if (get_flag() == True) else 0)
						run = (run << 1) 
						run |= (1 if (get_flag() == True) else 0)
						run = (run << 1) 
						run |= (1 if (get_flag() == True) else 0)
						run += 0x06
						
						uncompress(prev_u_pos,run)
				else:
					uncompress(prev_u_pos,run)
			else:
				uncompress(prev_u_pos,run)
		else:
			uncompress(prev_u_pos,run)
	else:
		uncompress(prev_u_pos,run)


def main_loop():
	global U_POS
	global C_POS
	global FLAG
	global FLAG_POS
	global COMPRESSED_BUFFER
	global UNCOMPRESSED_BUFFER
	FLAG = struct.unpack("<H",struct.pack("B",get_compressed_byte())+"\x00")[0]
	FLAG_POS = 0x08

	while C_POS < len(COMPRESSED_BUFFER):
		if(get_flag() == True):

			if(get_flag() == True):

				run = 0x00
				for i in range(0,5):
					run = run << 1
					
					if(get_flag() == True):
						run |= 1
						
				prev_u_pos = get_compressed_byte()
				if(run != 0x00):
					prev_u_pos |= ((run << 0x08))

					setup_run(prev_u_pos)
				elif(prev_u_pos > 0x02):

					setup_run(prev_u_pos)
				elif(prev_u_pos == 0x00):
					return
				else:
					branch = get_flag()
					for i in range(0,4):
						run = run << 1
						
						if(get_flag()==True):
							run |= 1
					if(branch):
						run = run << 0x08
						
						run |= get_compressed_byte()
					run += 0x0E
					
					byte = get_compressed_byte()
					for i in range(0,run):
						UNCOMPRESSED_BUFFER[U_POS] = byte
						U_POS+=1
			else:
				prev_u_pos = get_compressed_byte()

				setup_run(prev_u_pos)
		else:

			UNCOMPRESSED_BUFFER[U_POS] = get_compressed_byte()
			U_POS+=1


			
def decompress_block(data):
	global U_POS
	global C_POS
	global FLAG
	global FLAG_POS
	global COMPRESSED_BUFFER
	global UNCOMPRESSED_BUFFER
	COMPRESSED_BUFFER = bytearray(0x10000)
	COMPRESSED_BUFFER[:len(data)] = bytearray(data)
	UNCOMPRESSED_BUFFER = bytearray(0x10000)
	C_POS = 0x00
	U_POS = 0x00
	FLAG = 0x00
	FLAG_POS = 0x00
	main_loop()
	return UNCOMPRESSED_BUFFER[:U_POS]







if(__name__=="__main__"):

	f = open("test.bin",'rb')
	data = f.read()
	f.close()
	uncompressed_d = decompress_block(data)

	with open("out.bin",'wb') as g:
		g.write(uncompressed_d)
	
