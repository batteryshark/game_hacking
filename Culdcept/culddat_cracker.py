'''
Culdcept DAT Extraction Utility by Mario Mercaldi
(C) 2013 HackItUp
'''

import os,sys,struct
'''
for dirpath, dnames, fnames in os.walk("./"):
    for f in fnames:
		if(f.endswith("dat")):
			print("Processing: "+f)
			if( not os.path.exists(os.path.splitext(f)[0])):
				os.mkdir(os.path.splitext(f)[0])
'''
override = sys.argv[2]

if( not os.path.exists(os.path.splitext(sys.argv[1])[0])):
	os.mkdir(os.path.splitext(sys.argv[1])[0])
infile = open(sys.argv[1],"rb")
indata = infile.read()
#Really Shoddy Detection Method... Revise
head_len = struct.unpack("<I",indata[:4])[0]
if(head_len < len(indata)):
	print("Directory Package")
	
	for i in range(0,0x128,8):

		offset = struct.unpack("<I",indata[i:i+4])[0]
		size = struct.unpack("<I",indata[i+4:i+8])[0]

		print("offset: %04X size: %04X" % (offset,size))
		outfile = open(".\\"+os.path.splitext(sys.argv[1])[0]+".\\"+sys.argv[1]+"_"+str(i)+".dat","wb")
		outfile.write(indata[offset:offset+size])
		outfile.close()
else:
	result = os.system("c:\\packer.exe d %s %s.dec" % (sys.argv[1],sys.argv[1]))
	print(result)
	if(result == 0x00):
		outfile = open(sys.argv[1]+".new","wb")
		outfile.write(indata[4:])
		outfile.close()
		
		result = os.system("c:\\packer.exe d %s %s_new.dec" % (sys.argv[1]+".new",sys.argv[1]+".new"))
		print(result)