# Celceta ITP Translation Extractor

import os,sys,flz,struct
from PIL import Image
ITP_DB = {
"\x01\x10\x13\x50":{"mode":"RGBA","BPP":32,'width':4,'height':8,'palsz':12,'comp':16},
"\x01\x10\x11\x50":{"mode":"RGBA","BPP":32,'width':4,'height':8,'palsz':12,'comp':16},
}

def get_palette_and_data(data,pal_sz,bpp):
	bypp = bpp/8
	
	img_palette = bytearray(data[:pal_sz*bypp])
	# Convert to Palette Index
	palette = []

	for i in range(0,pal_sz*bypp,bypp):
		if(bypp == 1):
			palette.append((img_palette[i]))
		elif(bypp == 3):
			palette.append((
			img_palette[i],
			img_palette[i+1],
			img_palette[i+2],		
			))		
		else:
			palette.append((
			img_palette[i],
			img_palette[i+1],
			img_palette[i+2],
			img_palette[i+3],
			
			))
	return palette,data[pal_sz*bypp:]

def get_pal_img(data,w,h,stride=32):
	data = str(data)
	im_arrs = []
	background = Image.new('P', (w, h), (0, 0, 0, 0))
	for i in range(0,len(data),stride*stride):
		im_arrs.append(Image.frombytes("P",(stride,stride), data[i:i+stride*stride]))
	curr_x = 0
	curr_y = 0
	for m in im_arrs:
		if(curr_x*stride == w):
			curr_y+=1
			curr_x=0
		background.paste(m,(curr_x*stride,curr_y*stride))
		curr_x+=1
	return background
	
def p_to_rgba(img,palette,w,h):
	img = img.convert("RGBA")
	#Draw the colors.
	pixels = img.load()
	px_lst = ""
	for x in range(w):
		for y in range(h):
			cindx = pixels[x,y][0]
			pixels[x,y] = (palette[cindx])
	return img

def extract_itp(data,outpath):
	if(not data[:4] in ITP_DB):
		print("ITP Format not recognized")
		exit(1)
	itp_metadata = ITP_DB[data[:4]]
	itp_width = struct.unpack("<I",data[itp_metadata['width']:itp_metadata['width']+4])[0]
	itp_height = struct.unpack("<I",data[itp_metadata['height']:itp_metadata['height']+4])[0]
	itp_palsz = struct.unpack("<I",data[itp_metadata['palsz']:itp_metadata['palsz']+4])[0]
	print("%s %d x %d %d %dBPP" % (sys.argv[1],itp_width,itp_height,itp_palsz,itp_metadata['BPP']))
	ucdata = flz.FLZ(data[itp_metadata['comp']:]).uncompressed_data
	ipal,pxdata =  get_palette_and_data(ucdata,itp_palsz,itp_metadata['BPP'])
	pimg = get_pal_img(pxdata,itp_width,itp_height)
	fimg = p_to_rgba(pimg,ipal,itp_width,itp_height)

	fimg.save(outpath)


if(__name__=="__main__"):
	try:
		f = open(sys.argv[1],'rb')
	except:
		print("Usage %s infile" % sys.argv[0])
		exit(1)

	data = f.read()
	f.close()
	outpath,fname = os.path.split(sys.argv[1])
	outpath = os.path.join(outpath,fname.replace(".itp",".png"))
	data = extract_itp(data,outpath)

		