#!/bin/bash
TABLEFILE=test/dato/fitres/invMass_SC_corr.dat
xVar=runNumber


tmpFile=p.dat
index=0
runListFile=tmp/stability/runList-${index}.dat

# create the substitution of runMin\trunMax by runMin\trunMax\tunixMin\tunixMax
# this way one can always add the unix time
awk '(NF==3 && !/#/){print $1,$3}'  data/runRanges/*.dat | sort | uniq | sed -r 's|[ ]+|\t|g;s|[\t]+|\t|g' | sed 's|\t|\\)/\\1\t|;s|-|\t|g;s|^|s/\\\(|;s|$|/|' > unixTime.sed

grep -v '#' $TABLEFILE | grep ${xVar} | sed "s|[-]*${xVar}_\([^_]*\)_\([^- ]*\)\([^& ]*\)|\3\t${xVar}\t\1\t\2|;s|^\t${xVar}|noname\t${xVar}|;s|^-||"  > $tmpFile


sed -i -f unixTime.sed $tmpFile
