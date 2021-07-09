import os,sys,struct


pvhdr = "\x47\x42\x49\x58\x08\x00\x00\x00\x00\x00\x00\x00\x20\x20\x20\x20\x50\x56\x52\x54\x08\x20\x00\x00\x06\x0A\x00\x00\x00\x01\x80\x01"
	

infile = open(sys.argv[1],"rb")
width = struct.pack("<H",int(sys.argv[2]))
height = struct.pack("<H",int(sys.argv[3]))

indata = infile.read()
szstr = struct.pack("<I",len(indata)+8)

fin_header = pvhdr[:0x14]+szstr+pvhdr[0x18:0x1C]+width+height

outfile = open("out.pvr","wb")
outfile.write(fin_header+indata)
outfile.close()



result = os.system("c:\\pvr2png.exe out.pvr %s.png" % os.path.splitext(sys.argv[1])[0])
if(result == 0):
	print("Convert OK")


