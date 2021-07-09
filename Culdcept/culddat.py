'''
Culdcept PS2/DC Dat unpacker
'''
import os,sys,struct

offset_table = []
infile = open(sys.argv[1],"rb")

	

offset = struct.unpack("<I",infile.read(4))[0]
offset_0 = offset
offset_table.append(offset)

while(infile.tell() < offset_0):
		offset = struct.unpack("<I",infile.read(4))[0]
		offset_table.append(offset)



print("Number of Files: %d" % len(offset_table))
offset_table.append(os.path.getsize(sys.argv[1]))

file_ctr = 1
for i in range(0,len(offset_table)-1):
	sc = ".part%0*d" % (5,file_ctr)
	outfile = open(os.path.splitext(sys.argv[1])[0]+sc+".bin","wb" )
	infile.seek(offset_table[i],0)
	outfile.write(infile.read(offset_table[i+1] - offset_table[i]))
	outfile.close()
	file_ctr += 1
	