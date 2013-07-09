#!/bin/bash

usage(){
    echo "Usage: `basename $0` -d dir"
    echo "       dir: directory with log files" 
}

while getopts d: option
  do
  case $option in
      d)
	  dir=$OPTARG
	  if [ ! -d "$dir" ];then
	      echo "[ERROR] directory $dir specified dows not exist" >> /dev/stderr
	      exit 1
	  fi
	  ;;
      *)
	  ;;
  esac
done

grep passed $dir* | sort -n -t '_' -k 4 | awk '{total+=$5;passed+=$8; failed+=$11};END{print total, passed, failed, passed/total*100, failed/total*100}'


