import os,sys,struct


def channelSwap(pixeldata,bpp):
	pixeldata = bytearray(pixeldata)
	channelcount = 0
	while(channelcount < len(pixeldata)):
		tmpixB = pixeldata[channelcount]
		pixeldata[channelcount] = pixeldata[channelcount+2]
		pixeldata[channelcount+2] = tmpixB
		channelcount+=int(bpp / 8)
	return pixeldata

def ARGB1555toARGB8888(in_pixel):
	a = (in_pixel & 0x8000) & 0xFFFF
	r = (in_pixel & 0x7C00) & 0xFFFF
	g = (in_pixel & 0x03E0) & 0xFFFF
	b = (in_pixel & 0x1F) & 0xFFFF
	rgb = ((r << 9) | (g << 6) | (b << 3)) & 0xFFFFFFFF
	return ((a*0x1FE00) | rgb | ((rgb >> 5) & 0x070707)) & 0xFFFFFFFF
	
f = open(sys.argv[1],"rb")
data = f.read()
f.close()
outdata = ""
for i in range(0,len(data),2):
	outdata += struct.pack("<I",ARGB1555toARGB8888(struct.unpack("<H",data[i:i+2])[0]))
	
outdata = channelSwap(outdata,32)

f = open(os.path.splitext(sys.argv[1])[0]+".outpal","wb")
f.write(outdata)
f.close()
