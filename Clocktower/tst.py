#! /usr/bin/env python
# coding: utf-8
# coding=utf-8
# -*- coding: utf-8 -*-
# vim: fileencoding=utf-8

import codecs

ch_list = []
f = open('JIS0208.TXT', 'r')
for row in f:
    if row[0] != '#':
        c = row.split("\t")[2]
        ch_list.append(unichr(int(c, 16)))
f.close()

f = codecs.open('unicode.txt', 'w', 'utf-8')
txt = "".join(ch_list)
#txt = "\n".join(ch_list)
f.write(txt)
f.close()