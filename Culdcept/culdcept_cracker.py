'''
Culdcept DAT Extraction Utility by Mario Mercaldi
(C) 2013 HackItUp
'''

import os,sys,struct


def carveFile(datfile,outpath,offset,size):
	compchunk = datfile[offset:offset+size]
	decfile = uncompressChunk(compchunk)
	outfile = open(outpath,'wb')
	outfile.write(decfile)
	outfile.close()

def dumpFiles(datfile,filetable,outdir):

	for i in range(0,len(filetable)):
		outpath = outdir+"/"+"%s.file" % str(i)
		carveFile(datfile,outpath,filetable[i]['offset'],filetable[i]['size'])
		
def readEntries(datfile):

	firstoffset = struct.unpack("<I",datfile[0:4])[0]
	filetable = []

	
	for i in range(0,firstoffset,8):
		off_str = datfile[i:i+4]
		sx_str =  datfile[i+4:i+8]
		offset = struct.unpack("<I",off_str)[0]
		size = struct.unpack("<I",sx_str)[0]
		filetable.append({"offset":offset,"size":size})
	return filetable
	
def bufferDatFile(datname):
	dathandle = open(datname,"rb")
	datfile = dathandle.read()
	dathandle.close()
	return datfile

def uncompressChunk(compchunk):
	#TODO - EVERYTHING
	decdata = compchunk
	return decdata

if __name__ == "__main__":
	if(len(sys.argv)<3):
		print("Usage: main.py INPUT_DAT OUTPUT_DIR")
		exit(1)
	datname = sys.argv[1]
	outdir = sys.argv[2]
	try:
		open(datname,"r")
	except IOError:
		print("%s doesn't exist!" % datname)
		exit(1)
	
	if(not os.path.exists(outdir)):
		os.makedirs(outdir)
	
	datfile = bufferDatFile(datname)
	filetable = readEntries(datfile)
	print(filetable)
	dumpFiles(datfile,filetable,outdir)
