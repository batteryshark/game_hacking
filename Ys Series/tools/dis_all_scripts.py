import os,sys,struct,subprocess
import script_disasm
import script_asm


if(__name__=="__main__"):
	try:
		inpath = sys.argv[1]
	except:
		print("Usage: %s in_file" % sys.argv[0])
		exit(1)
	# First - Disassemble.
	for root,dirs,files in os.walk(inpath):
		for lf in files:
			if(not lf.endswith('.bin')):
				continue
			script_path = os.path.join(root,lf)
			print(script_path)
			subprocess.call(['python','script_disasm.py',script_path])
