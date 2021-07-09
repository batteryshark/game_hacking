# Unpack script bin files to directories for disassembly.
# Brought to you by Professor BatteryShark
import os,sys,struct


def dump_function(f,base_path):
	# Get Function Name
	func_name = f.read(32).replace("\x00","")
	print("Processing %s..." % func_name),
	func_sz = struct.unpack("<I",f.read(4))[0]
	func_of = struct.unpack("<I",f.read(4))[0]
	cloc = f.tell()
	f.seek(func_of)
	with open(os.path.join(base_path,func_name+".bin"),'wb') as g:
		g.write(f.read(func_sz))
	f.seek(cloc)
	print(" DONE!")

def unpack_bin(infile,base_path):
	# Read Header
	f = open(infile,'rb')
	hdr = f.read(20)
	if(hdr[:7] != "YS7_SCP"):
		print("Not an SCP File: %s" % infile)
		exit(1)
	# Get Num Functions
	num_functions = struct.unpack("<I",f.read(4))[0]
	for i in range(0,num_functions):
		dump_function(f,base_path)
	f.close()
if(__name__=="__main__"):
	try:
		infile = sys.argv[1]
	except:
		print("Usage %s infile" % sys.argv[0])
		exit(1)
	
	fname = os.path.split(infile)[1]
	fpath = os.path.split(infile)[0]
	fnx = os.path.splitext(fname)[0]
	base_path = os.path.join(fpath,fnx)
	if(not os.path.exists(base_path)):
		os.makedirs(base_path)
	unpack_bin(infile,base_path)
	
