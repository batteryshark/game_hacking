import binascii,struct
from Crypto.Cipher import AES
"""
0xffffd544:	0x00	0x1b	0x27	0x7b	0x51	0x1c	0x00	0x4c
0xffffd54c:	0x5c	0x5d	0x5c	0x57	0x05	0x7b	0x49	0x5a
0xffffd554:	0x5b	0x0b	0x07	0x21	0x5c	0x0a	0x40	0x0a
0xffffd55c:	0x06	0x10	0x03	0x0f	0x00	0x5c	0x1d	0x5a


0xffffd564:	0x00	0x0b	0x1d	0x12	0x05	0x1b	0x5a	0x47
0xffffd56c:	0x14	0x07	0x41	0x09	0x4e	0x41	0x22	0x55
"""

f = open("/g3/T4Data/econf/sounds.txt","rb")
encrypted = f.read()
f.close()
IV = binascii.unhexlify("000b1d12051b5a47140741094e412255")

KEY = binascii.unhexlify("001b277b511c004c5c5d5c57057b495a5b0b07215c0a400a0610030f005c1d5a")

aes = AES.new(KEY, AES.MODE_CFB, IV)
dd = aes.decrypt(encrypted)

g = open("OUT.bin","wb")
g.write(dd)
g.close()
