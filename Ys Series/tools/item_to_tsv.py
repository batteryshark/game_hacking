f = open('item.txt','r')
lines = f.readlines()
f.close()

FIELD_PER_ROW = 32

ctr = 0
out_data = ""
for i in range(0,len(lines)):
	out_data += lines[i].replace("\n","")
	if(ctr == FIELD_PER_ROW - 1):
		ctr = 0
		out_data+="\n"
	else:
		ctr +=1
		out_data+="\t"
		
with open("item.tsv",'wb') as f:
	f.write(out_data)