import os,sys,struct


def convert_texi(infile):
	with open(infile,'rb') as f:
		#Skip Header
		trash = f.read(8)
		out_base = f.read(36)
		out_base = out_base[:out_base.find("\x00")]
		with open(out_base+".itp",'wb') as g:
			g.write(f.read())
		

if(__name__=="__main__"):
	try:
		infile = sys.argv[1]
	except:
		print("Invalid Input File!")
		exit(1)

	convert_texi(infile)