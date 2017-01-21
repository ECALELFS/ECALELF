BEGIN{



}

(NF!=0){
    if ( $1 ~ /runNumber/){
      split($1,name,"-runNumber_");
      split(name[2],runNumbers,"_")
      sub("e",".",name[3]);
      sub("e",".",name[4]);
      print name[1], "runNumber" , runNumbers[1], runNumbers[2], 1-$7/100., $8/100., 0, 0 #Two extra columns at the end are necessary for the new EnergyScaleCorrections_class.cc (total stat and syst errors)
    }
}


#olde recalib with old region definition
# (NF!=0){
#     if ( $1 ~ /runNumber/){
#       split($1,name,"_runNumber_");
#       split(name[2],runNumbers,"_")
#       sub("e",".",name[3]);
#       sub("e",".",name[4]);
#       print name[1], "runNumber" , runNumbers[1], runNumbers[2], 1-$7/100., $8/100. #$3, $4, $5, $6
#     }
# }


END{



}