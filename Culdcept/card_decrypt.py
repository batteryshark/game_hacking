'''
There are a few cards in the PS2 version that are messed up after decrypting (they all have a 0x9) after them. 
One of them is 'Carbunfly' which is a cross between carbuncle and dragonfly and is hidden ingame. 
Omaiyasoft decided to 'encrypt' this card (probably so the engine couldnt read and wouldnt show it in the menus'.

At first, the card looks corrupt, we can see the start of the word 'Car' in one part and it looks like it still retains some form... 
what could they be doing I wonder?

Making the assumption that this has to be decrypted in memory and PDQ, 
I started looking online for values for carbunfly because it appears that the 
shot values are still in place (you can see the header size looks like its a 16 bit value still.)

so lets say that our ST value (40) which is 0x28 is SUPPOSED to actually BE 0x28. 
It's currently 0xFFDC, what's 0x28-FFDC? WELL - it's 0x4C!!!
We've run into some kind of chain cipher algorithm! Because we're 'scientists', 
let's actually confirm this. The next value should be 0x28 as well. What's 0x28 - 0x00? It's 0x28!!! 
One more! The next one is 100 for G (0x64) what's 0x64 - 0x3C? FUCKING 0x28!!!!! YESSIR!!!
Ok! Excitement aside, let's write a decryptor.

Basically, the algorithm is this:

Decrypt:

for every two bytes after the first:
- Add that value to the 16 bits behind it.
- Done!
'''
import os,sys,struct
f=open(sys.argv[1],"rb")
fsz = os.path.getsize(sys.argv[1])
#We're doing math ops on every 16 bit value, so
#it makes sense to throw them into an array first
#to do that more easily later on.
vals = []
vals.append(struct.unpack("<H",f.read(2))[0])
while(f.tell() < fsz):
	vals.append(struct.unpack("<H",f.read(2))[0])
f.close()
#Get our decrypted file ready.
out = open(os.path.splitext(sys.argv[1])[0]+".dec","wb")
out.write(struct.pack("<H",vals[0]))
#Why the &0xFFFF? We'll we're packing this into 16 bit values
#and expect there to be rollover with subtracting, we have to
#make sure it cuts off properly.
for i in range(1,len(vals)):
	out.write(struct.pack("<H",(vals[i] + vals[i-1]) & 0xFFFF))
	#Because the next in sequence depends on the plaintext value
	vals[i] += vals[i-1]
out.close()