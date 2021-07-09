from PIL import Image
from PIL import ImageDraw

im = Image.new("P", (64, 128), 0)

im.putpalette([
0,0,0,0,
136,0,0,0,
136,51,68,136,
136,119,136,136,
136,102,170,204,
153,153,0,153,
153,204,238,204,
170,204,102,85,
170,34,85,221,
187,0,221,85,
187,136,187,153,
204,51,51,34,
204,102,153,238,
221,238,85,34,
221,187,17,255,
255,238,187,51,

])

d = ImageDraw.ImageDraw(im)
d.setfill(1)

for i in range(0,16):
	d.setink(i)
	d.rectangle((i+20,i+20,i+40,i+40))

im.save("out.gif")