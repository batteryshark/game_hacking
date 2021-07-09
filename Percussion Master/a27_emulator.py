# A27 PCI Device Emulator for PercussionMaster (Linux)

import sys

# Unmap memory which... we won't have to worry about.
def pccard_cleanup:
	print("unmap ExCA:%lx,MEM:%lx" % (0,0))
	#Basically iounmap
	
def pccard_init:
	#Basically, create the device node.
	f = open("/dev/pccard","w+")
	f.close()
	
def pccard_release():
	return 0
