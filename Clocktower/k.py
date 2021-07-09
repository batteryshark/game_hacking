import os,sys,binascii
from translate import Translator
translator=Translator(to_lang="en")

#Encoding Lookup Table
ENC_TBL={
"J":"SHIFT_JIS",
"K":"johab",
"K2":"EUC-KR"
}
'''
f = open("/Users/atrfx/Downloads/sgd.bin","rb")
data = f.read()
f.close()
lines = data.split("\x00")
for line in lines:
'''
line = sys.argv[2]
encoding_type = ENC_TBL[sys.argv[1]]
instr = binascii.unhexlify(line)
#print(instr.encode(encoding_type))
outstr = instr.decode(encoding_type).encode("UTF-8")
#f = open("out.txt","wb")
print(outstr)
print(translator.translate(outstr))
#f.write(bytearray(outstr.encode("UTF-8")
#f.close()
