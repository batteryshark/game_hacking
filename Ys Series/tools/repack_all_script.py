import os,sys

dirs = os.listdir("script")
cwd = os.getcwd()
os.chdir("script")
for d in dirs:
	os.system("python ..\script_pack.py \"%s\"" %  d)

	
