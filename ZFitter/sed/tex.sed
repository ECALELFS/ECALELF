#s|-\([A-z]\)| \1|g
#s|[-]*absEta_0_1| $\\abs{\\eta} < 1$|
#s|[-]*absEta_1_1.4442| $\\abs{\\eta} > 1$|
#s|[-]*absEta_1.566_2| $\\abs{\\eta} < 2$|
#s|[-]*absEta_2_2.5| $\\abs{\\eta} > 2$|
s|[-]*absEta_0_1| $\|\\eta\| < 1$|g
s|[-]*absEta_1_1.4442| $\|\\eta\| > 1$|g
s|[-]*absEta_1.566_2| $\|\\eta\| < 2$|g
s|[-]*absEta_2_2.5| $\|\\eta\| > 2$|g
s|[-]*Et_\([0-9.]\+\)_\([0-9.]\+\)| $\1 < \\Et < \2$|g
s|^EB_EE |EB/EE |
s|^EB_EEp|EB/EE+|
s|^EB_EEm|EB/EE-|
s|^EBp|EB$+$|
s|^EBm|EB$-$|
s|^EEp|EE$+$|
s|^EEm|EE$-$|
#s|^EB | |
#s|^EE | |
s|[-]*runNumber_\([0-9]*\)_\([0-9]*\)| [\1 - \2]|g
#/[A-z]*_[.0-9]*_[.0-9]*/{s|\([A-z]*\)_\([.0-9]*\)_\([.0-9]*\)|$ \2 \\leq \1 < \3 $|g}
s|[-]gold| $R9 > 0.94$|g
s|[-]bad| $R9 < 0.94$|g
s|[-]highR9| $R9 > 0.94$|
s|[-]lowR9| $R9 < 0.94$|
s|Gold| $R9 > 0.94$|g
s|Bad| $R9 < 0.94$|g
s|EB/EE|%EB/EE|
s|EBhighEta|EB $\\abs{\\eta} > 1$|g
s|EBlowEta|EB $\\abs{\\eta} < 1$|g
s|EEhighEta|EE $\\abs{\\eta} > 2$|g
s|EElowEta|EE $\\abs{\\eta} < 2$|g
s|_EB| \\\& EB|g
s|_EE| \\\& EE|g
s|absEta| \\abs{\\eta}|g
s|SingleEle||g
#/abs_eta/ d
#/absEta/ d
/TB/ d
s|CenterModule|CM|
s|NoBorderModule|NB|
s|\(.*\)_SingleEle|ele1 \1 \& ele2 !\1|
s|entries ([0-9]* weighted) ||
#s| & \\| \\|
s|[0-9]*/[0-9]*/\([0-9]*\)|\1|
s|[-]EBGapEle1_5-EBGapEle2_5| GAP|
s|[-]!EBGapEle1_5-!EBGapEle2_5| noGAP|
