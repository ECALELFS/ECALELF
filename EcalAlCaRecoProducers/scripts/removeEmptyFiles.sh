#!/bin/bash
prefix="root://eoscms/"

usage(){
    echo "Usage: `basename $0` -u ui_working_dir -i jobID"
}

# options may be followed by one colon to indicate they have a required argument
if ! options=$(getopt -u -o u:i: -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
    -u) ui_working_dir=$2; shift;;
    -i) jobID=$2; shift ;;
    (--) shift; break;;
    (-*) echo "$0: error - unrecognized option $1" 1>&2; exit 1;;
    (*) break;;
    esac
    shift
done

if [ ! -d "$ui_working_dir" ];then
    echo "[ERROR] ui_working_dir $ui_working_dir not found or not readable" >> /dev/stderr
    exit 1
fi

if [ -z "$jobID" ];then
    echo "[ERROR] jobID not defined" >> /dev/stderr
    exit 1
fi


jobReport=$ui_working_dir/res/crab_fjr_$jobID.xml

nEvents=`cat $jobReport | grep -A1 '<TotalEvents>' | tail -1 | sed 's|.*\([0-9]\)|\1|'`

if [ "$nEvents" == "0" ];then
#    echo "File vuoto" 
#    cat $jobReport
    echo $jobReport
    cat > xml_parser.pperl <<EOF
#!/usr/bin/perl

# use module
use XML::Simple;
use Data::Dumper;

# create object
\$xml = new XML::Simple;

# read XML file
\$data=\$xml->XMLin("$jobReport");

#print \$data->{File}->{TotalEvents};
print \$data->{File}->{PFN};
#print Dumper(\$data);


EOF
file=`perl xml_parser.pperl | sed '/^[ \t]*$/ d;s|[ \t]*root|root|'`

#echo "Removing $file"
eos.select rm `echo $file | sed "s|$prefix||"`

rm xml_parser.pperl

fi




