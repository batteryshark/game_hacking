'''
Program that looks for the next byte being non null and returns the offset.
'''

import os,sys



f = open(sys.argv[1],'rb')
try:
    byte = f.read(1)
    while byte != "":
        if(not byte == b'\x00'):
			print("Offset here :D  %04X" % f.tell())
			exit(0)
        byte = f.read(1)
finally:
    f.close()