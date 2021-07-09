# -*- Mode: sh; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; coding: utf-8-unix; -*- 
# @version $Id: check_ip.sh 9380 2008-10-10 11:41:00Z mmiyoshi $ 
# @revision $Revision: 9380 $ 
# @author $Author: mmiyoshi $ 
# @date $Date: 2008-10-10 20:41:00 +0900 (金, 10 10 2008) $ 
 
rm -f /var/tmp/ip_check 
arping -D -c 1 -I eth0 $1 
arping_error=$? 
      
#arpingは 
# 0 かぶるIPがない 
# 1 かぶるIPがある 
# 2 eth自体がlinkupしていない 


echo $arping_error 


if [ $arping_error == 0 ]; then
#    echo 'not found' 
    exit 0 
fi 

if [ $arping_error == 2 ]; then 
#    echo 'not found' 
    exit 0 
fi 

exit 1
