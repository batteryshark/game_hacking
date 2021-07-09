import os
import sys
import struct


fp = open(sys.argv[1],"rb")
fileSize = os.path.getsize(sys.argv[1])

numfilestr = struct.unpack_from("I",fp.read(4))
numfiles = numfilestr[0]
print("Numfiles: %d"%numfilestr[0])
for x in range(0,numfiles):
	strlenstr = struct.unpack("I",fp.read(4))
	strlen = strlenstr[0]
	filestr = bytes(fp.read(strlen))
	filestr = filestr.decode()
	fileoffsetd = struct.unpack("I",fp.read(4))
	filesized = struct.unpack("I",fp.read(4))
	fileoffset = fileoffsetd[0]
	filesize = filesized[0]
	print("File ",x," Name:",filestr," Offset: ",fileoffsetd[0]," Size:",filesized[0])
	dircount = filestr.count("\\")
	for y in range(0,dircount):
		dirsplit = filestr.split('\\')
		if(os.path.exists(dirsplit[0]) != True):
			os.mkdir(dirsplit[y])
	outfile = open(filestr.rstrip(' \t\r\n\0'),"wb")
	curpos = fp.tell();
	fp.seek(fileoffset,0)
	buff = fp.read(filesize)
	outfile.write(buff)
	outfile.close()
	fp.seek(curpos,0)
	
print("DONE!")
