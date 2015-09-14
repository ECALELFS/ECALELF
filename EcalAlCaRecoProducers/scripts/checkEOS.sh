#!/bin/bash

fileList=$1
if [ ! -r "${fileList}" ];then 
    echo "[ERROR] File ${fileList} not found or not readable" >> /dev/stderr
    exit 1
fi

echo "[INFO] Checking eos files for ${fileList}"
datasets=`parseDatasetFile.sh ${fileList}`
if [ -z "${datasets}" ];then
    echo "[ERROR] No rereco found in alcarereco_datasets.dat" >> /dev/stderr
    exit 1
fi


# set IFS to newline in order to divide using new line the datasets
IFS=$'\n'


for dataset in $datasets
  do
  ./scripts/filelistDatasets.sh --check $dataset &> /dev/null
  if [ "$?" == "2" ];then
      echo "[WARNING] Following dataset not found on EOS: $dataset"
  fi
done
