#! /bin/sh


pingTarget() {
  ping -i 1 -c 1 -s 1 -q $1 > /dev/null
  if [ $? != 0 ]; then
    echo $1 | sed 's/\./ /g' >> 'save0/ping-reports_tmp.txt'
  fi
}

# sending target
echo > 'save0/ping-reports_tmp.txt'
for target in $@; do pingTarget $target; done
mv save0/ping-reports_tmp.txt save0/ping-reports.txt

