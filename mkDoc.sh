#!/bin/bash
if [ ! -d "doc/doxygen/fulldoc/" ];then
    mkdir -p doc/doxygen/fulldoc/
fi
if [ ! -d "doc/doxygen/fulldoc/html" ];then
    cd doc/doxygen/fulldoc/
    git clone -b gh-pages git@github.com:ECALELFS/ECALELF.git html
    cd -
fi

doxygen fulldoc

cd doc/doxygen/fulldoc/
git pull
git add *
git commit -m "updated documentation" -a
git commit -m "updated documentation" -a
git push
cd -

