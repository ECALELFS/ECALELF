#! /bin/bash

for file in tmp/s[0-9]*_selected_chain.root tmp/d_selected_chain.root tmp/s_selected_chain.root
  do
  name=`basename $file .root | sed 's|_.*||'`
  echo $name
  hadd -f tmp/${name}_chain.root tmp/${name}_*_chain.root
  filelist="$filelist tmp/${name}_chain.root"
done
