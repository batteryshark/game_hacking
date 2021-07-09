import os,sys,struct


def ARGB1555toARGB8888(in_pixel):
	a = (in_pixel & 0x8000) & 0xFFFF
	r = (in_pixel & 0x7C00) & 0xFFFF
	g = (in_pixel & 0x03E0) & 0xFFFF
	b = (in_pixel & 0x1F) & 0xFFFF
	rgb = ((r << 9) | (g << 6) | (b << 3)) & 0xFFFFFFFF
	return struct.pack("<I",((a*0x1FE00) | rgb | ((rgb >> 5) & 0x070707)) & 0xFFFFFFFF)

def RGB565toRGB(in_pixel):
	r = (in_pixel & 0xf800) >> 11
	g = (in_pixel & 0x07e0) >> 5
	b = in_pixel & 0x001f
	print("%d,%d,%d," %  ((r << 3)& 0xFF,(g << 2)& 0xFF,(b << 3) & 0xFF ))
	return  struct.pack("BBB",(r << 3)& 0xFF,(g << 2)& 0xFF,(b << 3) & 0xFF)

def RGB4444toARGB8888(in_pixel):
	a = ((in_pixel >> 12) & 0xf) * 0x11
	r = ((in_pixel >> 8) & 0xf) * 0x11
	g = ((in_pixel >> 4) & 0xf) * 0x11
	b = ((in_pixel >> 0) & 0xf) * 0x11
	print("%d,%d,%d,%d," %  (a,b,g,r))
	return  struct.pack("BBBB",a,b,g,r)
	
def channelSwap(pixeldata,bpp):
	
	channelcount = 0
	while(channelcount < len(pixeldata)):
		tmpixB = pixeldata[channelcount]
		pixeldata[channelcount] = pixeldata[channelcount+2]
		pixeldata[channelcount+2] = tmpixB
		channelcount+=int(bpp / 8)
	return pixeldata
	
infile = open("rat.pal","rb")

insz = os.path.getsize("rat.pal")

outdata = ""
while( infile.tell() < insz):
	#outdata += RGB565toRGB(struct.unpack("<H",infile.read(2))[0])
	#outdata += ARGB1555toARGB8888(struct.unpack("<H",infile.read(2))[0])
	outdata += RGB4444toARGB8888(struct.unpack("<H",infile.read(2))[0])
outfile = open("new.pal","wb")
outfile.write(channelSwap(bytearray(outdata),24))
outfile.close()
	