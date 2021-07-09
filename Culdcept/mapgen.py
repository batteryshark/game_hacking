'''
Culdcept Map Compositor By atr0x
'''

import os,sys
from PIL import Image

TEX_W = 64
TEX_H = 64
row_size = int(sys.argv[1])
cur_row = 0
cur_col = 0
file_count = 0

for dirpath, dnames, fnames in os.walk("./"):
    for f in sorted(fnames):
        if "map_" in f:
			file_count +=1
col_size = 0x1A
background = Image.new('RGBA', (TEX_W*row_size,TEX_H*col_size), (255, 255, 255, 255))
bg_w,bg_h=background.size
			
for dirpath, dnames, fnames in os.walk("./"):
    for f in sorted(fnames):
        if "map_" in f:
			
			
			if(cur_col > int(sys.argv[1])):
				cur_row+=1
				cur_col = 0
			img=Image.open(os.path.join(dirpath, f),'r')
			img_w,img_h=img.size
			offset = (cur_col * TEX_W,cur_row * TEX_H)
			background.paste(img,offset)
			cur_col +=1
			
background.save('..\MAP_%s.png' % sys.argv[2])
