'''
Culdcept Sprite Converter by atr0x
'''
import os,sys,struct
from PIL import Image



#1. Read header, get header size, resolution,etc
#2. Read CLUT
#3. Convert CLUT to ARGB1555
#4. Read Pixel Data
#4. Create new image.
#4. For every pixel in indata:
	#Find its color.
	#Draw that color on the image in the right spot.
#5. Done!

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
	



inf = sys.argv[1]
inf_noext = os.path.splitext(sys.argv[1])[0]
f = open(inf,"rb")
data = f.read()

if(data[0] == b'\x10'):
	header = data[0:16]

	header_len = struct.unpack("<H",data[0:2])[0]
	unk1 = struct.unpack("<H",data[2:4])[0]
	width = struct.unpack("<H",data[4:6])[0]
	height = struct.unpack("<H",data[6:8])[0]
	unk2 = struct.unpack("<H",data[8:10])[0]
	unk3 = struct.unpack("<H",data[10:12])[0]
	unk4 = struct.unpack("<H",data[12:14])[0]
	unk5 = struct.unpack("<H",data[14:16])[0]
	#Eat the header
	data = data[0x10:]		
	pal16 = data[:0x200]
	#Eat the Palette
	data = data[0x200:]

	#Convert Palette from 16bit to 32 bit
	pal32 = []
	for i in range(0,len(pal16),2):
		pal32.append(ARGB1555toARGB8888(struct.unpack("<H",pal16[i:i+2])[0]))
	
	#pal32 = channelSwap(pal32,32)
	
	#Create 32 bit pixel data by using our clut against the pixels
	outpixels = ""
	for pixel in data:
		
		outpixels += struct.pack("<I",pal32[struct.unpack("B",pixel)[0]])
	

	#Make new image
	im = Image.frombuffer("RGBA",(width,height),outpixels,'raw',"RGBA",0,1)
	
	im.save(inf_noext+".png","PNG")
	
	
else:
	pal16 = data[0:32]
	
	while (len(pal16) < 512):
		pal16 += pal16
	
		#Convert Palette from 16bit to 32 bit
	pal32 = []
	for i in range(0,len(pal16),2):
		pal32.append(ARGB1555toARGB8888(struct.unpack("<H",pal16[i:i+2])[0]))

	#Create 32 bit pixel data by using our clut against the pixels
	outpixels = ""
	for pixel in data:
		
		outpixels += struct.pack("<I",pal32[struct.unpack("B",pixel)[0]])
	

	#Make new image
	im = Image.frombuffer("RGBA",(int(sys.argv[2]),int(sys.argv[3])),outpixels,'raw',"RGBA",0,1)
	
	im.save(inf_noext+".png","PNG")