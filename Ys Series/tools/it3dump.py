import os,sys,struct


def unpack_files(in_path,out_path):
	ctr = 0
	with open(in_path,'rb') as f:
		while f.tell() < os.path.getsize(in_path):
			f_suffix = f.read(4)
			f_sz = struct.unpack("<I",f.read(4))[0]
			f_path = os.path.join(out_path,'%d.%s' % (ctr,f_suffix))
			with open(f_path,'wb') as g:
				g.write(f_suffix)
				g.write(struct.pack("<I",f_sz))
				g.write(f.read(f_sz))
			ctr+=1

if(__name__=="__main__"):
	try:
		infile = sys.argv[1]
	except:
		print("Invalid Input File!")
		exit(1)
	fname = os.path.split(infile)[1]
	bname = os.path.splitext(fname)[0]
	if(not os.path.exists(bname)):
		os.makedirs(bname)
	unpack_files(infile,bname)