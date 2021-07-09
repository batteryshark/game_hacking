'''
Culdcept Card Dumper by atr0x
'''

import os,sys,struct,json,StringIO,string
from PIL import Image
import traceback


class Card(object):
	def __init__(self,infile):
		self.hdr_sz = 0
		self.st = 0
		self.hp = 0
		self.mhp = 0
		self.g = 0
		self.unk_0x09 = 0
		self.rarity = ""
		self.unk_0x0B = 0
		self.unk_0x0C = 0
		self.unk_0x0D = 0
		self.type = ""
		self.land_restriction = ""
		self.item_restriction = ""
		self.unk_0x12 = 0
		self.unk_0x14 = 0
		self.unk_0x16 = 0
		self.id = 0
		self.unk_0x1A = 0
		self.unk_0x1C = 0
		self.card_offset = 0
		self.spr_offset = 0
		self.unkfile_offset = 0
		self.unk_0x24 = 0
		self.unk_0x26 = 0
		self.unk_0x28 = 0
		self.unk_0x2A = 0
		self.unk_0x2C = 0
		self.unk_0x2E = 0
		self.unk_0x30 = 0
		self.unk_0x32 = 0
		self.name = ""
		self.desc1 = ""
		self.desc2 = ""
		self.desc3 = ""
		self.sprite_compressed = ""
		self.sprite = ""
		self.unknown_file = ""
		self.card_gfx = ""
		self.read_file(infile)
	rarity_table = {0x00:"N",0x01:"S",0x02:"R",0x83:"E",0x03:"E"}
	type_table = {0x11:"Fire",0x12:"Water",0x13:"Earth",0x14:"Air",0x10:"Neutral",0x18:"Spell_Buff",0x28:"Spell_Curse",0x38:"Spell_Attack",0x48:"Spell_Curse2",0x90:"Weapon",0xA0:"Armor",0xB0:"Tool",0xC0:"Scroll"}
	item_limit_table = {0x00:"None",0x01:"Weapons",0x2:"Armor",0x3:"Weapons, Armor",0x8:"Scrolls",0x9:"Weapons, Scrolls",0xA:"Armor, Scrolls",0xB:"Weapons, Armor,Scrolls",0x0E:"Armor, Tools, Scrolls",0x0F:"Weapons, Armor, Tools, Scrolls"}
	land_limit_table = {0x00:"None",0x2:"Fire",0x4:"Water",0x06:"Fire, Water",0x8:"Earth",0x0A:"Fire, Earth",0x10:"Air",0x12:"Fire, Air",0x13:"Fire, Air",0x14:"Water, Air",0x1B:"Neutral, Fire, Earth, Air"}
	artist_table = {0x00:"Yuri Hanayama",0x02:"Naoyuki Katoh",0x03:"Ayano Koshiro",0x04:"Yuji Kaida",0x06:"Satoshi Nakai",0x07:"Ayano Koshiro"}
	
	def msg_log(self,msg):
		manifile = open(".\\card.txt","a+")
		manifile.write(msg+"\n")
		manifile.close()
	
	
	def dump_cardinfo(self,str_data):
		str_data = str_data.replace("\n","").strip()
		str_data = str_data.replace('"',"`")
		str_data = str_data.replace(b'\x83\xC0',"a ")
		str_data = str_data.replace(b'\x83\xBF',"b ")
		strings = str_data.split(b'\x00')
		card_metadata = {"name":strings[0]}
		ref_strings = []
			
		for i in range(0,len(strings)):
			strings[i] = strings[i].replace(strings[0],"")
			#Fix because they cant spell 
			strings[i] = strings[i].replace("Judgement","")
			#strings[i] = filter(lambda x: x in string.printable, strings[i])
			strings[i] = strings[i].replace(b'\x87\x41'," (Fire) ")
			strings[i] = strings[i].replace(b'\x87\x42'," (Water) ")
			strings[i] = strings[i].replace(b'\x87\x43'," (Earth) ")
			strings[i] = strings[i].replace(b'\x87\x44'," (Air) ")

			if(card_metadata['name'] != strings[i] and strings[i] != "" and strings[i] != b'\x00' and strings[i] != "\n"):
				ref_strings.append(strings[i])
		strings = ref_strings
		
		card_metadata["page_1"] = strings[len(strings) - 3]
		card_metadata["page_2"] = strings[len(strings) - 2]
		card_metadata["page_3"] = strings[len(strings) - 1]
		'''	
		for i in range(1,len(strings)):
			
			strings[i] = strings[i].replace(b"\x01\x1E\x01\x64","")
			strings[i] = strings[i].replace(b'\x87\x41'," (Fire) ")
			strings[i] = strings[i].replace(b'\x87\x42'," (Water) ")
			strings[i] = strings[i].replace(b'\x87\x43'," (Earth) ")
			strings[i] = strings[i].replace(b'\x87\x44'," (Air) ")
			strings[i] = strings[i].replace('@',"")
			strings[i] = strings[i].replace(b'\x04\x0C\x0D\x40','')
			strings[i] = strings[i].replace(b'\x80\x05','')
			strings[i] = strings[i].replace(b'\x09','')
			strings[i] = strings[i].replace(b'\x01','')
			#Just in case something snuck by...
			strings[i] = filter(lambda x: x in string.printable, strings[i])
			
			if(strings[0] == strings[i] and page_ctr == 1):
				page_ctr +=1
				continue
			if(strings[0] != strings[i] and strings[i] != "" and strings[i] != b'\x00' and strings[i] != "\n"):
				card_metadata["page_%d" % page_ctr] = strings[i]
				page_ctr+=1
		'''
		return card_metadata

	def uncompress_file(self,compdata):
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

	def ARGB1555toARGB8888(self,in_pixel):
		a = (in_pixel & 0x8000) & 0xFFFF
		r = (in_pixel & 0x7C00) & 0xFFFF
		g = (in_pixel & 0x03E0) & 0xFFFF
		b = (in_pixel & 0x1F) & 0xFFFF
		rgb = ((r << 9) | (g << 6) | (b << 3)) & 0xFFFFFFFF
		return ((a*0x1FE00) | rgb | ((rgb >> 5) & 0x070707)) & 0xFFFFFFFF	
	
	def pvr2png(self,curpath,indata,width,height,pvr_type,outname):

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
		outdata += bytearray(200048)
		#Because ARGB -> ABGR
		
		#STUPID FUCKING DREAMCAST FORMAT BEING ABGR!!!!
		#outdata = channelSwap(outdata,32)
			
		im = Image.frombuffer(colortype,(width,height),outdata,'raw',colortype,0,1)
		im.save("tmp.png","PNG")
		f = open("tmp.png","rb")
		outpng = f.read()
		f.close()
		os.remove("tmp.png")
		return outpng

	def dump_cardgfx(self,card_data):
		outpng = self.pvr2png(".\\",card_data,256,320,"ARGB1555","card.png")
		return outpng
	
	def dump_sprite(self):
		pal16 = self.sprite[0:32]
	
		while (len(pal16) < 512):
			pal16 += pal16
	
		#Convert Palette from 16bit to 32 bit
		pal32 = []
		for i in range(0,len(pal16),2):
			pal32.append(self.ARGB1555toARGB8888(struct.unpack("<H",pal16[i:i+2])[0]))

		#Create 32 bit pixel data by using our clut against the pixels
		outpixels = ""
		for pixel in self.sprite:
			outpixels += struct.pack("<I",pal32[struct.unpack("B",pixel)[0]])
	

		#Make new image
		im = Image.frombuffer("RGBA",(int(64),int(128)),outpixels,'raw',"RGBA",0,1)
		im.save("tmp.png","PNG")
		f = open("tmp.png","rb")
		outpng = f.read()
		f.close()
		os.remove("tmp.png")
		return outpng

	def read_file(self,infile):
		#Check Flag
		f = open(infile,"rb")
		dec_byte = f.read(1)
		f.close()
		
		os.system("unlha d %s %s 1>nul" % (infile,infile))
		
		if(dec_byte == b'\x09'):
			os.system("c:\\python27\\python.exe card_dcrypt.py %s" % infile)
		f = open(infile,"rb")
		data = f.read()
		f.close()
		
		self.hdr_sz = struct.unpack("B",data[0x00])[0]
		self.st = struct.unpack(">H",data[1:3])[0]
		self.hp = struct.unpack(">H",data[3:5])[0]
		self.mhp = struct.unpack(">H",data[5:7])[0]
		self.g = struct.unpack(">H",data[7:9])[0]
		self.unk_0x09 = struct.unpack("B",data[0x09])[0]
		try:
			self.rarity = self.rarity_table[struct.unpack("B",data[0x0A])[0]]
			self.unk_0x0B = struct.unpack("B",data[0x0B])[0]
			self.unk_0x0C = struct.unpack("B",data[0x0C])[0]
			self.unk_0x0D = struct.unpack("B",data[0x0D])[0]
			self.type = self.type_table[struct.unpack("B",data[0x0E])[0]]
			self.land_restriction = self.land_limit_table[struct.unpack("B",data[0x0F])[0]]
			self.item_restriction = self.item_limit_table[struct.unpack("B",data[0x10])[0]]
		except KeyError:
			exc_type, exc_value, exc_traceback = sys.exc_info()
			self.msg_log("%s" % sys.argv[1])
			lines = traceback.format_exception(exc_type, exc_value, exc_traceback)
			self.msg_log("%s" % ''.join('!! ' + line for line in lines))
			#os.rename(sys.argv[1],".\\v\\"+sys.argv[1])
			exit(1)
		self.unk_0x12 = struct.unpack("<H",data[0x12:0x14])[0]
		self.unk_0x14 = struct.unpack("<H",data[0x14:0x16])[0]
		self.unk_0x16 = struct.unpack("<H",data[0x16:0x18])[0]
		self.id = struct.unpack("<H",data[0x18:0x1A])[0]
		self.unk_0x1A = struct.unpack("<H",data[0x1A:0x1C])[0]
		self.unk_0x1C = struct.unpack("<H",data[0x1C:0x1E])[0]
		self.card_offset = struct.unpack("<H",data[0x1E:0x20])[0]
		self.spr_offset =  struct.unpack("<H",data[0x20:0x22])[0]
		self.unkfile_offset = struct.unpack("<H",data[0x22:0x24])[0]
		self.unk_0x24 =  struct.unpack("<H",data[0x24:0x26])[0]
		self.unk_0x26 =  struct.unpack("<H",data[0x26:0x28])[0]
		self.unk_0x28 =  struct.unpack("<H",data[0x28:0x2A])[0]
		self.unk_0x2A =  struct.unpack("<H",data[0x2A:0x2C])[0]
		self.unk_0x2C =  struct.unpack("<H",data[0x2C:0x2E])[0]
		self.unk_0x2E =  struct.unpack("<H",data[0x2E:0x30])[0]
		self.unk_0x30 =  struct.unpack("<H",data[0x30:0x32])[0]
		self.unk_0x32 =  struct.unpack("<H",data[0x32:0x34])[0]
		if(self.spr_offset == 0):
			card_metadata = self.dump_cardinfo(data[0x34:self.card_offset])
		else:
			card_metadata = self.dump_cardinfo(data[0x34:self.spr_offset])
		self.name = card_metadata["name"]
		self.desc1 = card_metadata["page_1"]
		self.desc2 = card_metadata["page_2"]
		self.desc3 = card_metadata["page_3"]
		if(self.spr_offset != 0):
			self.sprite_compressed = data[self.spr_offset:self.unkfile_offset]
			self.sprite = self.uncompress_file(self.sprite_compressed)
			self.unknown_file = data[self.unkfile_offset:self.card_offset]
			self.sprite = self.dump_sprite()
		self.card_gfx = data[self.card_offset:]
	
	def gen_json(self):
	
		json_data = '[{"card_id":%d,"data":{' % self.id+\
			'"id":%d,' % self.id + \
			'"st":%d,' % self.st + \
			'"hp":%d,' % self.hp + \
			'"mhp":%d,' % self.mhp + \
			'"g":%d,' % self.g + \
			'"name":"%s",' % bytes(self.name)+ \
			'"desc1":"%s",' % self.desc1+ \
			'"desc2":"%s",' % self.desc2 + \
			'"desc3":"%s",' % self.desc3 + \
			'"rarity":"%s",' % self.rarity + \
			'"type":"%s",' % self.type +  \
			'"land_restriction":"%s",' % self.land_restriction + \
			'"item_restriction":"%s",' % self.item_restriction  +  \
			'"unk_0x09":%d,' %self.unk_0x09  + \
			'"unk_0x0B":%d,' % self.unk_0x0B + \
			'"unk_0x0C":%d,' % self.unk_0x0C + \
			'"unk_0x0D":%d,' % self.unk_0x0D + \
			'"unk_0x12":%d,' % self.unk_0x12 + \
			'"unk_0x14":%d,' % self.unk_0x14 + \
			'"unk_0x16":%d,' % self.unk_0x16 + \
			'"unk_0x1A":%d,' % self.unk_0x1A + \
			'"unk_0x1C":%d,' % self.unk_0x1C + \
			'"unk_0x24":%d,' % self.unk_0x24 + \
			'"unk_0x26":%d,' % self.unk_0x26 + \
			'"unk_0x28":%d,' % self.unk_0x28 + \
			'"unk_0x2A":%d,' % self.unk_0x2A + \
			'"unk_0x2C":%d,' % self.unk_0x2C + \
			'"unk_0x2E":%d,' % self.unk_0x2E + \
			'"unk_0x30":%d,' % self.unk_0x30 + \
			'"unk_0x32":%d}}]' % self.unk_0x32
		

		return json_data
	
	def dump_card(self):
		#Dump Metadata to JSON
		out = open("%d.json" % self.id,"wb")
		out.write(self.gen_json())
		out.close()		
		if(self.spr_offset != 0):
			#Dump Sprite (if available)
			out = open("%d_spr.png" % self.id,"wb")
			out.write(self.sprite)
			out.close()
			#Dump Sprite_Unk File (if available)
			out = open("%d.bin" % self.id,"wb")
			out.write(self.unknown_file)
			out.close()
		#Dump Card Graphic
		out = open("%d_card.png" % self.id,"wb")
		out.write(self.dump_cardgfx(self.card_gfx))
		out.close()
	
if(__name__ == "__main__"):
	card = Card(sys.argv[1])
	card.dump_card()
	#os.remove(sys.argv[1])
	
		