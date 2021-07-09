
import os,sys

f = open(sys.argv[1],"rb")

data = f.read()
f.close()

if(data[0x8:0xC] == "SCEN"):
	
	os.rename(sys.argv[1],os.path.splitext(sys.argv[1])[0]+".scen")


if(data[0x8:0xC] == "MODL"):
	
	os.rename(sys.argv[1],os.path.splitext(sys.argv[1])[0]+".modl")
	
if(data[0:4] == b'\x05\x00\x22\x56'):
	os.rename(sys.argv[1],os.path.splitext(sys.argv[1])[0]+".adx")