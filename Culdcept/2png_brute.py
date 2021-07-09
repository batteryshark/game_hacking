import os,sys,struct

width_sz = [32,64,128,256,512,1024]
height_sz = [32,64,128,256,512,1024]
cur_out = 0
pvhdr = "\x47\x42\x49\x58\x08\x00\x00\x00\x00\x00\x00\x00\x20\x20\x20\x20\x50\x56\x52\x54\x08\x20\x00\x00\x00\x0B\x00\x00\x00\x01\x80\x01"
for t0 in range(0,7):
	for t1 in range(0x0,0x13):
		infile = open(sys.argv[1],"rb")
		width = struct.pack("<H",64)
		height = struct.pack("<H",128)
		format = struct.pack("<H",t0)
		type = struct.pack("<H",t1)
		indata = infile.read()
		infile.close()
		szstr = struct.pack("<I",len(indata)+8)

		fin_header = pvhdr[:0x14]+szstr+format[0]+type[0]+pvhdr[0x1A:0x1C]+width+height

		outfile = open("out.pvr","wb")
		outfile.write(fin_header+indata)
		outfile.close()



		result = os.system("pvr2png.exe out.pvr %s_%dx%d_%d_%d.png" % (os.path.splitext(sys.argv[1])[0],64,128,t0,t1))
		if(result == 0):
			print("Convert OK")
			cur_out+=1	
'''
for i in range(0,len(width_sz)):

	for height in height_sz:

		infile = open(sys.argv[1],"rb")
		width = struct.pack("<H",width_sz[i])
		height = struct.pack("<H",height)

		indata = infile.read()
		infile.close()
		szstr = struct.pack("<I",len(indata)+8)

		fin_header = pvhdr[:0x14]+szstr+pvhdr[0x18:0x1C]+width+height

		outfile = open("out.pvr","wb")
		outfile.write(fin_header+indata)
		outfile.close()



		result = os.system("c:\\pvr2png.exe out.pvr .\\test\\%s_%d.png" % (os.path.splitext(sys.argv[1])[0],cur_out))
		if(result == 0):
			print("Convert OK")
			cur_out+=1
				
'''
