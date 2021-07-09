#Falcom ITP Conversion Module

import os,sys,struct,flz
from PIL import Image
#Note - converting back - itpcnv v6 seems to like this one: 01 10 41 60
NO_PALETTE_DB = [0x50111008]
LA_DB = [0x60111004]
CCPI_DB = [0x50430801,0x50410801,0x50411001,0x60411001,0x60411004]
LEGACY_CCPI_MAGIC = '\xee\x03\x00\x00'
LEGACY_DB = [0x3EE,0x3E8,0x3EA,0x3EB,0x3EC,0x3ED,0x3E9]
class ITP(object):
	def __init__(self,path):
		self.flname = os.path.splitext(path)[0]
		self.stride = 32
		with open(path,'rb') as f:
			self.magic = struct.unpack("<I",f.read(4))[0]
			if(self.magic in CCPI_DB):
				
				with open("tmp.itp",'wb') as g:
					g.write(LEGACY_CCPI_MAGIC+f.read())
				os.system("itpcnv.exe -p tmp.itp")
				if(not os.path.exists("tmp.png")):
					print("ERR - Something went wrong with CCPI for %s" % self.flname)
					exit(1)
				os.rename("tmp.png",self.flname+'.png')
				os.remove("tmp.itp")
				exit(0)
			if(self.magic in LEGACY_DB):
				with open("tmp.itp",'wb') as g:
					g.write(struct.pack("<I",self.magic)+f.read())
				os.system("itpcnv.exe -p tmp.itp")
				if(not os.path.exists("tmp.png")):
					print("ERR - Something went wrong with CCPI for %s" % self.flname)
					exit(1)
				os.rename("tmp.png",self.flname+'.png')
				os.remove("tmp.itp")
				exit(0)				
			self.width = struct.unpack("<I",f.read(4))[0]
			if(self.width < 32):
				self.stride = self.width
			self.height = struct.unpack("<I",f.read(4))[0]
			print("ITP File:")
			print("Magic: %#4x" % self.magic)
			print("Size: %d x %d " % (self.width,self.height))
			if(not self.magic in NO_PALETTE_DB):
				self.pal_sz = struct.unpack("<I",f.read(4))[0]

				print("Palette Size: %d Colors" % self.pal_sz)
			print("Analyzing FZ Container...")
			self.image_flz = flz.FLZ(f.read()) # Will have to change this later when I do the ICCP type.
			print("Decompression Complete!")
			self.image_data = self.image_flz.uncompressed_data
		if self.magic in NO_PALETTE_DB:
			if(self.magic in LA_DB):
				self.type = "LA"
			else:
				self.type = "RGB"
		else:
			self.type = "RGBA" #Will have to change this later too.
		
		
	def save(self,format="PNG"):
		img_palette = []
		img_palette_sorted = []
		if(self.type == "RGBA"):
			img_palette = bytearray(self.image_data[:self.pal_sz * 4])
			for i in range(0,len(img_palette),4):
				img_palette_sorted.append((img_palette[i],img_palette[i+1],img_palette[i+2],img_palette[i+3]))
			with open('tmp','wb') as g:
				g.write(self.image_data[self.pal_sz * 4:])
			
		if(self.type == "RGB"):
			img_palette = bytearray(self.image_data[:self.pal_sz * 3])
			for i in range(0,len(img_palette),3):
				img_palette_sorted.append((img_palette[i],img_palette[i+1],img_palette[i+2]))
			with open('tmp','wb') as g:
				g.write(self.image_data[self.pal_sz * 3:])
		if(self.type == "L" or self.type == "LA"):
			with open('tmp','wb') as g:
				g.write(self.image_data)
		img_palette = img_palette_sorted				
		out_img = Image.new(self.type, (self.width,self.height))
		x = 0 # current x offset in image
		y = 0 # current y offset in image.
		block_count = 0
		f = open('tmp','rb')
		total_blocks = (self.width / self.stride) * (self.height / self.stride)
		while block_count < total_blocks:		
			quad_data = f.read(self.stride*self.stride)
			quad_buffer = generate_quad_matrix(quad_data,self.stride,img_palette,self.type)
			out_img.paste(quad_buffer, (x,y))
			
			x += self.stride
			if(x >= self.width):

				x = 0
				y += self.stride
			block_count +=1
		out_img.save(self.flname+'.png')
		f.close()

# Takes a 1d buffer and maps it to a 2d array with a given height, width.
def generate_quad_matrix(data,msz,img_palette,type):
	#qbuffer = array('c'[array('c',[0 for x in range(msz)]) ,[for x in range(msz)])
	#qbuffer = numpy.zeros((msz,msz),dtype=numpy.int8)
	if(type == "LA"):
		qbuffer = Image.new("RGBA", (msz,msz))
	else:
		qbuffer = Image.new(type, (msz,msz))
	pixels = qbuffer.load()
	data_offset = 0
	data = bytearray(data)
	while data_offset < len(data):
		for y in range(0,msz):
			for x in range(0,msz):
				if(type == "RGB"):
					pixels[x,y] = (data[data_offset],data[data_offset+1],data[data_offset+2])
					data_offset+=3
					continue
				if(type == "LA"):
					pixels[x,y] = (0,0,0,data[data_offset])
				else:
					pixels[x,y] = img_palette[data[data_offset]]
				data_offset+=1
			y+=1
	return qbuffer

if(__name__=="__main__"):
	itpf = ITP(sys.argv[1])
	print("Saving to PNG...")
	itpf.save("PNG")
	print("Done!")
	