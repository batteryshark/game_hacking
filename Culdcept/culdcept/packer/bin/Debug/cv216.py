import os,sys,struct

f = open(sys.argv[1],"rb")
indata = f.read()
f.close()

outdata = ""
for pixel in indata:
	outdata += struct.pack("<H",struct.unpack("B",pixel)[0] << 8)
	
out = open("out.rgb16","wb")
out.write(outdata)
out.close()
	