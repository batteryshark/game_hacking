import os,sys

for root,dirs,files in os.walk("script"):
	for f in files:
		if(not f.endswith(".csv")):
			continue
		os.system("python script_asm.py \"%s\"" % os.path.join(root,f))