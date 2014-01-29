#!/bin/bash
if [ ! -d "doc/doxygen/fulldoc/html" ];then
    cd doc/doxygen/fulldoc/
    git clone -b gh-pages git@github.com:ECALELFS/ECALELF.git html
    cd -
fi

doxygen fulldoc


