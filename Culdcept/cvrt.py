import os,sys,struct
from PIL import Image

def pvr2png(curpath,indata,width,height,pvr_type):

	if(pvr_type is "ARGB1555" or pvr_type is "RGB4444"):
		colortype = "RGBA"
		depth = 32
	if(pvr_type is "RGB565"):
		colortype = "RGB"
		depth = 24
		
	def channelSwap(pixeldata,bpp):
		
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
		
	def RGB565toRGB(in_pixel):
		r = (in_pixel & 0xf800) >> 11;
		g = (in_pixel & 0x07e0) >> 5;
		b = in_pixel & 0x001f;
		return  struct.pack("BBB",(r << 3)& 0xFF,(g << 2)& 0xFF,(b << 3) & 0xFF)
		
	def RGB4444toARGB8888(in_pixel):
		a = ((word >> 12) & 0xf) * 0x11
		r = ((word >> 8) & 0xf) * 0x11
		g = ((word >> 4) & 0xf) * 0x11
		b = ((word >> 0) & 0xf) * 0x11
		return  struct.pack("BBBB",a,b,g,r)
		
		
	outdata = bytearray()

	if(pvr_type is "ARGB1555"):
		for i in range(0,len(indata),2):
			outdata += struct.pack("<I",ARGB1555toARGB8888(struct.unpack("<H",indata[i:i+2])[0]))
	else:
		for i in range(0,len(indata),2):
	
			outdata += RGB565toRGB(struct.unpack("<H",indata[i:i+2])[0])

	#Padding for larger resolutions because PIL is a fuckhead.
	outdata += bytearray(5095535)
	#Because ARGB -> ABGR
	if(pvr_type is "ARGB1555"):
		#outdata = channelSwap(outdata,depth)
		pass
	im = Image.frombuffer(colortype,(width,height),str(outdata),'raw',colortype,0,1)
	im.save(curpath+os.path.splitext(sys.argv[1])[0]+".png","PNG")

infile = open(sys.argv[1],"rb")
indata = infile.read()
infile.close()
width = int(sys.argv[2])
height = int(sys.argv[3])
if(indata[0] == b'\x10'):
	width = struct.unpack("<H",indata[4:6])[0]
	height = struct.unpack("<H",indata[6:8])[0]
	indata = indata[16:]
pvr2png(".\\",indata,width,height,"ARGB1555")