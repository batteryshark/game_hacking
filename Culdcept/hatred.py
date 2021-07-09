

'''
compressed file carver
General Execution Flow:

1. open file

find first 0x00 0x08 - set as start
global start

for the rest of the file

if find a new 0x00 0x08

write out file as start -> i

set new start as i

continue


'''

import os,sys,struct


infile = open(sys.argv[1],"rb")
indata = infile.read()

startptr = 0
part_counter = 1
if(not os.path.exists("dir_"+sys.argv[1])):
	os.makedirs("dir_"+sys.argv[1])
for i in range(0,len(indata)-4):

	if(indata[i] == b'\x00' and indata[i+1] ==  b'\x08'):
		fsz = struct.unpack("<H",indata[i+2:i+4])[0]
		if(fsz < len(indata)):
			
			outfile = open("tmp.lh5","wb")
			outfile.write(indata[i+1:])
			outfile.close()
			print("c:\\packer.exe d tmp.lh5 .\\dir_"+sys.argv[1]+"\\"+sys.argv[1]+".part"+str( part_counter)+".bin")
			result = os.system("c:\\packer.exe d tmp.lh5 .\\dir_"+sys.argv[1]+"\\"+sys.argv[1]+".part"+str( part_counter)+".bin" )
			if(os.path.exists(".\\dir_"+sys.argv[1]+"\\"+sys.argv[1]+".part"+str( part_counter)+".bin")):
					part_counter+=1
			if(result == 0):
				print("Extraction Complete!")
			



	
