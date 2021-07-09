import os,sys


f = open(sys.argv[1],"rb")

data = f.read()
f.close()

for i in range(0,len(data)):
	if(data[i] != data[0]):
		exit(0)
	
os.remove(sys.argv[1])
exit(0)