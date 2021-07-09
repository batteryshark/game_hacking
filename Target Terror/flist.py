import os,sys,struct,zlib

# Get completed Set
"""

print("Hashing All Known Files...")
good_set = {}
for root,dirs,files in os.walk("/TargetTerror_Cracked/data/"):
    for fl in files:
        fpath = os.path.join(root,fl)
        f = open(fpath,"rb")
        good_set[zlib.adler32(f.read())&0xFFFFFFFF] = fpath
        f.close()
"""

for root,dirs,files in os.walk("."):
    for f in files:
        fpath = os.path.join(root,f)
        fd = open(fpath,"rb")
        #adlr = zlib.adler32(fd.read())&0xFFFFFFFF
        #fd.close()
        #if(adlr in good_set.keys()):
            #print("Duplicate Found! %s -> %s" % (fpath,good_set[adlr]))
            #os.remove(fpath)


        try:
            uncompressed_size = struct.unpack("<I",fd.read(4))[0]

            name_size = struct.unpack("<H",fd.read(2))[0]
            crapola = fd.read(2)
            if(crapola != "\xff\xbf"):
                continue
        
            filename = bytearray(fd.read(name_size))
            fd.close()
            #print(fpath)
        
            for i in range(0,name_size):
                filename[i] = ~filename[i] & 0xFF
            #print(filename)
            filename = filename.lower().replace("\\","/")
        except:
            print("Error Reading: %s" % filename)
            continue
        if(not os.path.exists("/TargetTerror_Cracked/data/%s" % filename)):
            print("Undumped File: %s" % filename)
            base,fll = os.path.split(filename)
            fbase = "/tt/data/%s" % base
            if(not os.path.exists(fbase)):
                os.makedirs(fbase)
            out_path = "/tt/data/%s" % filename
            os.rename(fpath,out_path)
            pass
        else:
            print("Dumped File!: %s" % filename)
            os.remove(fpath)

        

