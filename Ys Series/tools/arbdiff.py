# Prints different arb files - ignoring the header.
import os,sys

PATH_1 = "C:\\Falcom\\CelcetaEnglish\\map"
PATH_2 = "C:\\Program Files (x86)\\joyoland\\ysc\\map"
map_1 = {}
map_2 = {}

for root,dirs,files in os.walk(PATH_1):
	for f in files:
		if(f.endswith(".arb")):
			map_1[f] = os.path.join(root,f)
			
for root,dirs,files in os.walk(PATH_2):
	for f in files:
		if(f.endswith(".arb")):
			map_2[f] = os.path.join(root,f)
			
for fk in map_1.keys():
	f1 = map_1[fk]
	f2 = map_2[fk]
	f = open(f1,'rb')
	f.seek(0x14)
	data_1 = f.read()
	f.close()
	f = open(f2,'rb')
	f.seek(0x14)
	data_2 = f.read()
	if(data_1 != data_2):
		print(fk)