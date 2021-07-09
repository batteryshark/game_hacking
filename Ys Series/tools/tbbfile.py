import os,sys,struct

f = open(sys.argv[1],'rb')
data = f.read()
f.close()
tbb_version = struct.unpack("<I",data[0:4])[0]

IMPL_VERSIONS = [0x03,0x08]
print("TBB Version: %x."%tbb_version)

if(not tbb_version in IMPL_VERSIONS):
	print("Not Implemented")
	exit(1)

	
#Version 3 Handling
if(tbb_version == 0x03 or tbb_version == 0x08):
	idk_1 = struct.unpack("<I",data[4:8])[0]
	strdata_sz = struct.unpack("<I",data[8:12])[0]
	strdata = data[12:12+strdata_sz]
	strtab_offset = 12+strdata_sz
	#Get output handle ready.
	g = open('out.csv','wb')
	for i in range(strtab_offset,len(data),4):
		str_offset = struct.unpack("<I",data[i:i+4])[0]
		
		tmp_str = data[str_offset+12:].split("\x00")[0]
		g.write("%s\n"%tmp_str)
		
	g.close()