#!/bin/bash

file=$1


cat $file | awk -F '[&]' -f awk/format.awk

exit 0
# associative array
declare -A columns=([3]=1 [4]=1 [6]=1)

end=`cat ${file} | awk -F '&' '{n=NF>n ? NF:n};END{print n}'`

for column in `seq 1 $end`
  do
#  echo $column
  if [ ! ${columns[$column]} ]; then echo $column; fi
  
done



