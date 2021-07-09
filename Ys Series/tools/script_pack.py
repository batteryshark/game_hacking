import os,sys,struct


SCP_HDR = "YS7_SCP\x00\x00\x00\x00\x02\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC"

func_f_lst = []
if(__name__=="__main__"):
	try:
		indir = sys.argv[1]
	except:
		print("%s in_directory" % sys.argv[0])
		exit(1)
	in_path = os.path.abspath(indir)
	dname = os.path.basename(os.path.normpath(in_path))
	out_filename = dname+".bin"
	for root, dirs, files in os.walk(in_path):
		for f in files:
			if(not f.endswith(".bin")):
				continue
			fp = open(os.path.join(root,f),'rb')
			data = fp.read()
			fp.close()
			func_f_lst.append({
				"name":f[:-4],
				"data":data,
				"sz":len(data)
			})
	data_offset_start = (len(func_f_lst) * 40 ) + len(SCP_HDR) + 4
	running_offset = 0
	with open(out_filename,'wb') as g:
		g.write(SCP_HDR)
		g.write(struct.pack("<I",len(func_f_lst)))
		for entry in func_f_lst:
			g.write(entry['name'])
			g.write("\x00" * (32 - len(entry['name'])))
			g.write(struct.pack("<I",len(entry['data'])))
			g.write(struct.pack("<I",data_offset_start+running_offset))
			running_offset += len(entry['data'])
		for entry in func_f_lst:
			g.write(entry['data'])
			
			