import os,sys,struct
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
			if(lf.endswith('.csv')):
				continue
			script_disasm.branch_lst = []
			script_asm.JMP_LST = []
			infile = os.path.join(root,lf)
			print("Testing %s ..." % infile)
			func_sz = os.path.getsize(infile)
			f = open(infile,'rb')
			instruction_lst = script_disasm.dis_function(f,func_sz)
			f.close()
			new_data = script_asm.asm_function(instruction_lst)
			f = open(infile,'rb')
			original_data = f.read()
			f.close()
			new_data = bytearray(new_data)
			for jmp in script_asm.JMP_LST:
				rel_offset = jmp['LBL_OFFSET'] - jmp['offset']
				rel_offset_bytes = struct.pack("<i",rel_offset)
				new_data[jmp['offset']-4:jmp['offset']] = rel_offset_bytes
			if(not original_data == new_data):
				print("Err: %s did not reassemble properly." % infile)
				with open('err.bin','wb') as g:
					g.write(new_data)
				exit(1)
			else:
				print("SUCCESS!")