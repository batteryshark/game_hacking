'''
CLOCKTOWER ADT ADJUSTER
	(c) 2014 rFx
'''
import struct,os,sys
ADJ_VAL = 0x1

f = open("CT.ADT","rb")
data = bytearray(f.read())
f.close()

for i in range(0,len(data),4):
	val = struct.unpack("<I",data[i:i+4])[0]
	val += ADJ_VAL
	data[i:i+4] = struct.pack("<I",val & 0xFFFFFFFF)


f = open("CT.ADT","wb")
f.write(data)
f.close()



