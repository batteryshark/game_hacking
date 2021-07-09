#Culdcept Map Dumper by atr0x
#Dreamcast Maps need a channel swap from ABGR to ARGB
import os,sys,struct
from PIL import Image

#Don't let this scare you - it's just reading RGB numbers from 16 bit to 32 bit.
def ARGB1555toARGB8888(in_pixel):
	a = (in_pixel & 0x8000) & 0xFFFF
	r = (in_pixel & 0x7C00) & 0xFFFF
	g = (in_pixel & 0x03E0) & 0xFFFF
	b = (in_pixel & 0x1F) & 0xFFFF
	rgb = ((r << 9) | (g << 6) | (b << 3)) & 0xFFFFFFFF
	return ((a*0x1FE00) | rgb | ((rgb >> 5) & 0x070707)) & 0xFFFFFFFF

#This will never change - like a bad pizza place.
BLOCK_SZ = 64

f=open(sys.argv[1],'rb')
#Theres a 1 padding column that the game uses - I don't need it.
w_blocks = struct.unpack("<H",f.read(2))[0] - 1
h_blocks = struct.unpack("<H",f.read(2))[0]
#Yes - I load it all into memory.
fdata = f.read()

#Get the start point for each map chunk - we have to add the 1 back :)
file_offsets = []
for i in range(0,(w_blocks+1)*h_blocks):
	file_offsets.append(struct.unpack("<I",fdata[(i*4):(i*4)+4])[0])
	
#The end doesn't have an offset - sooo...
file_offsets.append(len(fdata))

#Let's make a new image to hold our final map.
outmap = Image.new('RGBA', (BLOCK_SZ*w_blocks,h_blocks*BLOCK_SZ), (255, 255, 255, 255))

#The big loop - extract, convert, composite
cur_row = 0
cur_col = 0
for i in range(0,len(file_offsets)-1):

	#I write the data to a temp file and use a C uncompressor for LHA.
	itmp = open("in.tmp","wb")
	itmp.write(fdata[file_offsets[i]:file_offsets[i+1]])
	itmp.close()
	os.system("delha 1>nul")
	#Read the uncompressed file into the program.
	otmp = open("out.tmp","rb")
	img16_data = otmp.read()
	otmp.close()
	
	#Now we convert the ARGB1555 pixels to ARGB8888
	img32_data = ""
	for i in range(0,len(img16_data),2):
		img32_data += struct.pack("<I",ARGB1555toARGB8888(struct.unpack("<H",img16_data[i:i+2])[0]))
	im32 = Image.frombuffer("RGBA",(BLOCK_SZ,BLOCK_SZ),img32_data,'raw',"RGBA",0,1)
	#Basically, if we hit the width, go to the next row.
	if(cur_col > w_blocks):
		cur_row+=1
		cur_col = 0
	#Paste the current chunk in the appropriate spot.	
	outmap.paste(im32,(cur_col*BLOCK_SZ,cur_row*BLOCK_SZ))
	cur_col+=1
	
#Write our image out to disk.
outmap.save('%s.png' % os.path.splitext(sys.argv[1])[0])