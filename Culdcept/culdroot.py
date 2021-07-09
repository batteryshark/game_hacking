'''
Culdcept Root Directory Unpacker by atr0x
'''

import os,sys,struct

file_table = {}
infile = open(sys.argv[1],"rb")

endian = "<I"
if(len(sys.argv) > 2):
	if(sys.argv[2] is 'b'):
		endian = ">I"

	

offset = struct.unpack(endian,infile.read(4))[0]
size = struct.unpack(endian,infile.read(4))[0]
offset_0 = offset
file_table[offset] = size

while(infile.tell() < offset_0):
		offset = struct.unpack(endian,infile.read(4))[0]
		size = struct.unpack(endian,infile.read(4))[0]
		file_table[offset] = size
	
	
print("Number of Files: %d" % len(file_table))

file_ctr = 1
for offset in file_table:
	outfile = open(os.path.splitext(sys.argv[1])[0]+".part"+str(file_ctr)+".bin","wb" )
	infile.seek(offset,0)
	outfile.write(infile.read(file_table[offset]))
	outfile.close()
	file_ctr += 1
	