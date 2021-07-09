import os,sys,struct
from flz import FLZ

f = open(sys.argv[1],'rb')
data = f.read()
f.close()
out_name = os.path.splitext(sys.argv[1])[0]
print(sys.argv[1])
file_magic = struct.unpack("<I",data[0:4])[0]
cmp_type = struct.unpack("<I",data[12:16])[0]

COMPRESSED_DB = [0x2D,0x1D,0x8A,0x3E8,0xC7,0x3D,0x4FFC]
STRTAB_DB = [0x03,0x04,0x05,0x06,0x07,0x08,0x0A,0x14,0x1A,0x3D,0x20]
IDK_DB = [0x1008]
if(file_magic in COMPRESSED_DB):
	if(cmp_type == 0x02 or cmp_type == 0x03 or 'shop' in out_name): #Stupid Edge Cases
		if(file_magic == 0x4FFC):
			fldata = FLZ(data[12:])
		else:
			fldata = FLZ(data[4:])
		with open(out_name+'.txt','wb') as f:
			f.write(fldata.uncompressed_data)

	
if(cmp_type != 2 and cmp_type != 3):
	if(file_magic in STRTAB_DB):
		strtab_size = struct.unpack("<I",data[4:8])[0] << 4
		strdata_sz = struct.unpack("<I",data[8:12])[0]
		strdata = data[12:12+strdata_sz]
		strtab_offset = 12+strdata_sz
		#Get output handle ready.
		g = open(out_name+'.txt','wb')
		for i in range(strtab_offset,len(data),4):
			if(file_magic == 0x1A):
				if(struct.unpack("<I",data[i:i+4])[0] == strdata_sz-1):
					g.write("[[[TRAP_BYTE]]]\n")
					continue
			str_offset = struct.unpack("<I",data[i:i+4])[0] + 12

			str_sz = data[str_offset:].find("\x00")
			
			
			tmp_str = data[str_offset:str_offset+str_sz]
	
			g.write("%s\n"%tmp_str)
			
		g.close()
		
if(out_name == 'pc'): #Stupid special cases...
	num_rawstrings = struct.unpack("B",data[0])[0]
	offset = 1
	outdata = ""
	#First - the uncompressed part...
	for i in range(0,num_rawstrings):
		str_len = struct.unpack("B",data[offset])[0]
		offset+=1
		outdata+=data[offset:offset+str_len].split("\x00")[0]+"\n"
		offset+=str_len
	#Now - the compressed part...
	data = data[offset:]
	fldata = FLZ(data)
	with open(out_name+'.txt','wb') as f:
		f.write(outdata+fldata.uncompressed_data)
	
