#this script to translate the step2.dat file with scale corrections in H4l Misha's format
BEGIN{
  categories[0]="EB-absEta_0_1-bad"
  categories[1]="EB-absEta_0_1-gold"
  categories[2]="EB-absEta_1_1.4442-bad"
  categories[3]="EB-absEta_1_1.4442-gold"
  categories[4]="EE-absEta_1.566_2-bad"
  categories[5]="EE-absEta_1.566_2-gold"
  categories[6]="EE-absEta_2_2.5-bad"
  categories[7]="EE-absEta_2_2.5-gold"

}

(NF!=0){
  
  runMin=$3
  runMax[runMin]=$4
  for(cat in categories){
    if($1 == categories[cat]){
      corrCat[runMin,cat]=$5
      errCorrCat[runMin,cat]=$6
#      print $0
#      print categories[cat], "runNumber", runMin, runMax[runMin], corrCat[runMin,cat], errCorrCat[runMin,cat]
#      print ""
      break
    }
  }
  
  #corrCat0 is correction value for EB, eta<1, r9<0.94
  #corrCat1 is correction value for EB, eta<1, r9>0.94
  #corrCat2 is correction value for EB, eta>1, r9<0.94
  #corrCat3 is correction value for EB, eta>1, r9>0.94
  #corrCat4 is correction value for EE, eta<2, r9<0.94
  #corrCat5 is correction value for EE, eta<2, r9>0.94
  #corrCat6 is correction value for EE, eta>2, r9<0.94
  #corrCat7 is correction value for EE, eta>2, r9>0.94

}


END{
#nRunMin, nRunMax, corrCat0, corrCat1, corrCat2, corrCat3, corrCat4, corrCat5, corrCat6, corrCat7

  for(runMin in runMax){
    printf("%d,\t%d", runMin, runMax[runMin]);
    for(cat=0; cat < 8; cat++){
      printf(",\t%.4f", corrCat[runMin,cat]) #, errCorrCat[runMin,cat]
      #printf("%d\t", cat);
    }
    printf("\n")
  }

}
