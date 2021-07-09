#!/bin/bash
rm /tmp/stv_snd
rm /tmp/stv_snd_cat_pid
mkfifo /tmp/stv_snd
cat > /tmp/stv_snd &
echo $! > /tmp/stv_snd_cat_pid 
cat /tmp/stv_snd | padsp ./stvsnd ./arom/ &