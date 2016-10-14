#! /bin/bash

python macro/Closure_et.py -r barrel -b 0T
python macro/Closure_et.py -r endcap -b 0T
python macro/Closure_et.py -r barrel -b 0T -t smearing 
python macro/Closure_et.py -r endcap -b 0T -t smearing 

python macro/Closure_et.py -r barrel -b 38T
python macro/Closure_et.py -r endcap -b 38T
python macro/Closure_et.py -r barrel -b 38T -t smearing 
python macro/Closure_et.py -r endcap -b 38T -t smearing 