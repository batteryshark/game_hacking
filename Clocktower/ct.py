#String table range for Clock Tower = 0x1FDCB - 0x20293

'''
Clock tower string extractor by rFx
''' 
import os,struct,sys

f = open("ct32.exe","rb")
f.seek(0x1FDCB,0)
strdata = f.read(0x4C8)
lines = strdata.split('\x00')
for line in lines:
	print(line.decode('shift-jis'))