'''
Culdcept PS2/DC Card Ripper by atr0x
'''

import os,sys,struct
from PIL import Image

def pvr2png(curpath,indata,width,height,pvr_type,outname):

	if(pvr_type is "ARGB1555"):
		colortype = "RGBA"
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
		
	
	outdata = bytearray()

	for i in range(0,len(indata),2):
		outdata += struct.pack("<I",ARGB1555toARGB8888(struct.unpack("<H",indata[i:i+2])[0]))

	#Padding for larger resolutions because PIL is a fuckhead.
	outdata += bytearray(1028)
	#Because ARGB -> ABGR
	outdata = channelSwap(outdata,32)
		
	im = Image.frombuffer(colortype,(width,height),outdata,'raw',colortype,0,1)
	im.save(curpath+"/"+outname,"PNG")

card = {
'hdr_size':0,
'name':"",
'st':0,
'hp':0,
'mhp':0,
'g':0,
'desc':"",
'ability1':"",
'ability2':"",
'ability3':"",
'card_gfx':"",
'unk':"",
'guard':"",
'sprite_comp':"",
'sprite':"",
'restriction':""
}

restriction = ["","","Armor","","","","","","Scrolls","Weapons Scrolls"]

		


def get_cstr(instr):
	instr = instr.split(b'\x00')[0]
	return instr

def uncompress_file(compdata):
	if(compdata[0] != b'\x08'):
		print("ERROR - Not an LH5 Compressed File!")
		exit(1)
	
	tfp = open("in.tmp","wb")
	tfp.write(compdata)
	tfp.close()
	result = os.system("delha 1>nul")

	if(result == 1):
		print("ERROR IN Extraction!")
		exit(1)
	os.remove("in.tmp")
	tfp = open("out.tmp","rb")
	outdata = tfp.read()
	tfp.close()
	os.remove("out.tmp")
	
	return outdata
	
def dump_cardgfx(card_data):
	pvr2png(".\\",card_data,256,320,"ARGB1555","card.png")

def dump_sprite(sprite_data):
	pal16 = sprite_data[0:32]
	
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
	pvr2png(".\\",sprite_data,32,256,"ARGB1555","sprite.png")
	
def dump_rando_file(rando_data):
	outfile = open(".\\"+"unknown.bin","wb")
	outfile.write(rando_data)
	outfile.close()
	
def read_header():
	
	f = open(sys.argv[1],"rb")
	indata = f.read()
	f.close()

	card['hdr_size'] = struct.unpack("<H",indata[0:2])[0]
	card['st'] = struct.unpack("<H",indata[2:4])[0]
	card['hp'] = struct.unpack("<H",indata[4:6])[0]
	card['mhp'] = struct.unpack("<H",indata[6:8])[0]
	card['g'] = struct.unpack("<H",indata[8:10])[0]
	card['name'] = get_cstr(indata[0x34:card['hdr_size']])
	card['restriction'] = restriction[struct.unpack("<H",indata[0x10:0x12])[0]]
	card['sprite_comp'] = indata[struct.unpack("<H",indata[0x20:0x22])[0]:struct.unpack("<H",indata[0x22:0x24])[0]]
	card['sprite'] = uncompress_file(card['sprite_comp'])
	rando_offset = struct.unpack("<H",indata[0x22:0x24])[0]
	
	rando_len = struct.unpack("<H",indata[rando_offset:rando_offset+2])[0]
	
	dump_rando_file(indata[rando_offset:rando_offset+rando_len+0x202])
	
	card['card_gfx'] = indata[rando_offset+0x202+rando_len:]
	dump_cardgfx(card['card_gfx'])
	dump_sprite(card['sprite'])
	outfile = open(".\\"+"info.json","wb")
	outfile.write("{name:%s,st:%d,hp:%d,mhp:%d,g:%d}" % (card['name'],card['st'],card['hp'],card['mhp'],card['g']))
	outfile.close()
	
read_header()