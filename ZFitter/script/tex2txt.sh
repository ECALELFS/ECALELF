#!/bin/bash
usage(){
    echo "`basename $0` [--format]"
    echo " --format: 2 decimals"
}

if ! options=$(getopt -u -o h -l help,format -- "$@")
then
    # something went wrong, getopt will put out an error message for us
    exit 1
fi

set -- $options

while [ $# -gt 0 ]
do
    case $1 in
	-h|--help) usage; exit 0;;
	--format) format="y";;
	(--) shift; break;;
	(-*) usage; echo "$0: error - unrecognized option $1" 1>&2; usage >> /dev/stderr; exit 1;;
	(*) break;;
    esac
    shift
done

case $# in
    1)
	;;
    *)
	echo -n "Error." > /dev/stderr
	exit 1
	;;
esac

texfile=$1
if [ -z "$format" ];then

    cat $texfile  | sed -e '/\\begin{tabular}/ d;/\\end{tabular}/ d; s|\$||g;s|&|\t|g; s|\\\\||g; s|\\pm|\t|g; s|   | |g; s|  | |g;' 
else
    cat $texfile  | awk -F '[&]' -f awk/format.awk | sed -e '/\\begin{tabular}/ d;/\\end{tabular}/ d; s|\$||g;s|&|\t|g; s|\\\\||g; s|\\pm|\t|g; s|   | |g; s|  | |g;' 
fi

