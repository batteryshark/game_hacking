import os,sys,struct
from random import randint
from PIL import Image

color_table = [b'\x00\x00\x00']

for i in range(0,254):
	color_table.append(struct.pack("B",randint(0,0xFE))+struct.pack("B",randint(0,0xFE))+struct.pack("B",randint(0,0xFE)))



f = open("statue.data","rb")
indata = f.read()
f.close()

outdata = ""
for i in range(0,len(indata)):
	outdata += color_table[struct.unpack("B",indata[i])[0]]
	
im = Image.frombuffer("RGB",(int(sys.argv[1]),int(sys.argv[2])),outdata,'raw',"RGB",0,1)
im.save("tst.png","PNG")