import os,sys,shutil

f = open(sys.argv[1],"rb")
header = f.read(12)
f.close()
if(header[8:0xC] == 'MODL'):
	shutil.move(sys.argv[1],os.path.splitext(sys.argv[1])[0]+".modl")

