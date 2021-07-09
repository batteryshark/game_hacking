'''
This program unpacks the larger dat files. (File_102)
'''
import os,sys,struct,shutil

PACKER_PATH = "../../packer.exe"
PVR2PNG_PATH = "../../pvr2png.exe"

def detect_lh5(src):
	infile = open(src,"rb")
	magic = infile.read(1)
	fsz = struct.unpack(">H",infile.read(2))[0]
	if(magic == b'\x08' and fsz < os.path.getsize(sys.argv[1])):
		print("LH5 Detected!")
		retval = os.system("mono %s d %s %s.bin" % (PACKER_PATH,src,src))
		print(retval)
		if(retval == 0):
			pass
		else:
			print("Error in Decompression")
			exit(1)
	else:
		infile.close()
		detect_lh5v(src)
		detect_pvr(src)

def detect_lh5v(src):
	infile = open(src,"rb")
	indata = infile.read()

	startptr = 0
	part_counter = 0

	for i in range(0,len(indata)-4):

		if(indata[i] == b'\x00' and indata[i+1] ==  b'\x08'):
			fsz = struct.unpack("<H",indata[i+2:i+4])[0]
			if(fsz < len(indata)):
				if(part_counter != 0):
				
					outfile = open(src+".part"+str( part_counter)+".lh5v","wb")
					outfile.write(indata[startptr+1:])
					outfile.close()
					detect_lh5(src+".part"+str( part_counter)+".lh5v")
					detect_pvr(src+".part"+str( part_counter)+".lh5v"+".bin")		
				part_counter+=1
				startptr = i

			if(part_counter == 1 and startptr != 0):
				print("Only one File... printing")
				part_counter+=1
				outfile = open(src+".part"+str( part_counter)+".lh5v","wb")
				outfile.write(indata[startptr+1:])
				outfile.close()
				detect_lh5(src+".part"+str( part_counter)+".lh5v")
				detect_pvr(src+".part"+str( part_counter)+".lh5v"+".bin")


def detect_pvr(src):
	pvhdr = "\x47\x42\x49\x58\x08\x00\x00\x00\x00\x00\x00\x00\x20\x20\x20\x20\x50\x56\x52\x54\x08\x20\x00\x00\x01\x09\x00\x00\x00\x01\x80\x01"
	infile = open(src,"rb")
	if( os.path.getsize(src) == 8192):
		width = struct.pack("<H",64)
		height = struct.pack("<H",64)
	else:	
		width = struct.pack("<H",int(sys.argv[2]))
		height = struct.pack("<H",int(sys.argv[3]))

	indata = infile.read()
	szstr = struct.pack("<I",len(indata)+8)

	fin_header = pvhdr[:0x14]+szstr+pvhdr[0x18:0x1C]+width+height

	outfile = open("out.pvr","wb")
	outfile.write(fin_header+indata)
	outfile.close()
	result = os.system("wine %s out.pvr %s.png" % (PVR2PNG_PATH,os.path.splitext(src)[0]))
	os.remove("out.pvr")
	if(result == 0):
		print("Convert OK")
		pass
	else:
		pass




def main():
	if(len(sys.argv) < 2):
		print("Usage: undat.py [infile]")
		exit(1)

	try:
		infile = open(sys.argv[1],"rb")
	except IOError:
		print("ERR: Could not open %s" % sys.argv[1])
		exit(1)



	#Get the header
	hdr = infile.read(4)

	dat_size = os.path.getsize(sys.argv[1])

	if(struct.unpack("<I",hdr)[0] > dat_size):
		print("Not a Dat file")
		infile.close()
		detect_lh5(sys.argv[1])
		exit(0)

	if(not os.path.exists(os.path.splitext(sys.argv[1])[0])):
		if(".part" in os.path.splitext(sys.argv[1])[0]):
			os.mkdir(sys.argv[1])
		else:
			os.mkdir(os.path.splitext(sys.argv[1])[0])

	num_files = (struct.unpack("<I",hdr)[0]) / 8
	print("Number of files in DAT: %d" % num_files)

	infile.seek(0,0)

	off_ptr = 0
	file_data = []
	file_num = 0
	for i in range(0,num_files):
		file_data.append((struct.unpack("<I",infile.read(4))[0],struct.unpack("<I",infile.read(4))[0]))

	for f in file_data:
		print("File %d: Offset:%04X Size:%04X"  % (file_num,f[0],f[1]))
		if(f[0] > dat_size):
			print("ERROR: Offset not in current file")
			exit(1)
		infile.seek(f[0])
		file_num +=1
		outfile = open(os.path.splitext(sys.argv[1])[0]+"/"+os.path.splitext(sys.argv[1])[0]+".part"+str(file_num),"wb")
	
		outdata = infile.read(f[1])
		outfile.write(outdata)
		outfile.close()

	infile.close()

if(__name__=="__main__"):
	main()
