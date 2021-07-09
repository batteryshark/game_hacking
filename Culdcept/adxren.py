import os,sys

f = open(sys.argv[1],"rb")

data = f.read()
f.close()

if(data[0:4] == b'\x05\x00\x22\x56'):
	os.rename(sys.argv[1],os.path.splitext(sys.argv[1])[0]+".adx")
