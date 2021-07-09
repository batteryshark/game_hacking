import os,sys,struct

infile = open("file_102","rb")
afs_head = infile.read(4)

if(afs_head == b'AFS\x00'):
	print("Valid AFS file!")
	
	num_files = struct.unpack("<I",infile.read(4))[0]
	print("Num files = %d" % num_files)
	
	file_offsets = []
	for i in range(0,num_files):
		file_offsets.append((struct.unpack("<I",infile.read(4))[0],struct.unpack("<I",infile.read(4))[0]))
	print(file_offsets)
	
	for i in range(0,num_files):
		infile.seek(file_offsets[i][0],0)
		outfile = open("output2/file_%d" % i,"wb")
		outfile.write(infile.read(file_offsets[i][1]))
		outfile.close()
	
	