#!/bin/bash
case $# in 
    1)
	file=$1
	;;
    *)
	exit 1
	;;
esac

#original command
#cat $file | sed '/^#/d;s|EB-absEta_0_1|EBlowEta|;s|EB-absEta_1_1\.44*2|EBhighEta|;s|EE-absEta_1\.566_2|EElowEta|;s|EE-absEta_2_2\.5|EEhighEta|;s|-gold|Gold|;s|-bad|Bad|;s|_| |;s|\$||g;s|\\pm| \pm|g' 
#|  awk -F '&' -f awk/Hgg_resCorr.awk | sort | sed 's|EBlowEta|EB-absEta_0_1|;s|EBhighEta|EB-absEta_1_1.4442|;s|EElowEta|EE-absEta_1.566_2|;s|EEhighEta|EE-absEta_2_2.5|;s|Gold|-gold|;s|Bad|-bad|'

#command with ZPt option
cat $file | sed '/^#/d;s|EB-absEta_0_1|EBlowEta|;s|EB-absEta_1_1\.44*2|EBhighEta|;s|EE-absEta_1\.566_2|EElowEta|;s|EE-absEta_2_2\.5|EEhighEta|;s|-ZPt_25_35|lowZPt|g;s|-ZPt_35_55|midZPt|g;s|-ZPt_55_300|highZPt|g;s|-gold|Gold|;s|-bad|Bad|;s|_| |;s|\$||g;s|\\pm| \pm|g' |  awk -F '&' -f awk/Hgg_resCorr.awk | sort | sed 's|EBlowEta|EB-absEta_0_1|;s|EBhighEta|EB-absEta_1_1.4442|;s|EElowEta|EE-absEta_1.566_2|;s|EEhighEta|EE-absEta_2_2.5|;s|Gold|-gold|;s|Bad|-bad|; s|lowZPt|-ZPt_25_35|g; s|midZPt|-ZPt_35_55|g;s|highZPt|-ZPt_55_300|g;'






exit 0
