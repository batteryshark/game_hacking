'''
Null File Deleter
Deletes files containing all \x00
'''
import os,sys
f = open(sys.argv[1],"rb")
data = f.read()
f.close()
if(data.count(b'\x00') == len(data)):
	os.remove(sys.argv[1])