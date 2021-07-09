import sys
   
#Default Key for "I Wanna be the Boshy"
BOSHY_KEY = b"BLOB"

# INI++ Module Encrypt/Decrypt for MultimediaFusion 2 stuff.
# Yes, it's just RC4...
def crypt(data: bytes, key: bytes) -> bytes:
    """RC4 algorithm (now with more bytes)"""
    x = 0
    box = list(range(256))
    for i in range(256):
        x = (x + int(box[i]) + int(key[i % len(key)])) % 256
        box[i], box[x] = box[x], box[i]
    x = y = 0
    out = []
    for char in data:
        x = (x + 1) % 256
        y = (y + box[x]) % 256
        box[x], box[y] = box[y], box[x]
        out.append(char ^ box[(box[x] + box[y]) % 256])

    return bytearray(out)

# Open INI/Save/Options File
in_path = sys.argv[1]
out_path = "%s.converted" % in_path

with open(in_path,"rb") as f:
    with open(out_path,"wb") as g:
        g.write(crypt(f.read(),BOSHY_KEY))
