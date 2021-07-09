'''
Culdcept Brute-Force Unpacker by Mario Mercaldi
'''

import os,sys,struct

def msg_log(msg):
	manifile = open("unpack.txt","a+")
	manifile.write(msg+"\n")
	manifile.close()

CHUNK_SIZE = 5242880

f = open("PS2_DATA","rb")
indata = f.read()
f.close()

pos = 0
count = 0

while(pos < len(indata)):
	if(indata[pos] == b'\x08' or indata[pos] == b'\x0C' or indata[pos] == b'\x05'):

		out = open("in.tmp","wb")
		out.write(indata[pos:pos+CHUNK_SIZE])
		out.close()
		result = os.system("delha.exe 1> nul")
		if(result != 1):
			if(os.path.getsize("out.tmp") > 0):
				msg_log("File Written Successfully from %04X - Uncompressed Size: %d bytes" % (pos,os.path.getsize("out.tmp")))
				count +=1
				os.rename("out.tmp","file_%d.bin" % count) 
			else:
				os.remove("out.tmp")
	pos +=1


