import os,sys

for root,dirs,files in os.walk("gfx"):
	for f in files:
		if(not f.endswith(".itp")):
			continue
		fpath = os.path.join(root,f)
		os.system("python itp_extract.py %s" % fpath)