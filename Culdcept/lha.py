import lhafile, sys

# Create Lhafile instance from filename
filename = sys.argv[1]
f = lhafile.Lhafile(filename)

# Print each file informaion in archive file.
for info in f.infolist():
   print info.filename

# Extract data from archive
data = f.read('newfile')
'''
outfile = open(filename+".dec","wb")
outfile.write(data)
outfile.close()
'''