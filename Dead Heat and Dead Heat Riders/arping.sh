# -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; coding: utf-8-unix; -*-

#/**
# * @file arping.sh
# * @brief PCBID衝突検知
# * @author himamura
# * @version $Id: network.cpp 6370 2008-10-20 09:54:19Z himamura $
# */

#echo $$
trap 'rm $2; exit 1' 15
while true; do
  # 使用するかチェック
  while true; do
    use0=0
    use1=0
    read use0 < save0/tmp/arp_use.txt
    if [ $use0 -eq 1 ]; then
      break;
    fi
    
    read use1 < save1/tmp/arp_use.txt
    if [ $use1 -eq 1 ]; then
      break;
    fi

    sleep 10
  done
  
  cat save0/tmp/arp_ip.txt | xargs arping -0ar -w 100000 -c 1 > $1;
  sleep 1;
  cat save1/tmp/arp_ip.txt | xargs arping -0ar -w 100000 -c 1 > $2;
  sleep 1;
done;
