BEGIN{


}

(match(FILENAME,"res_corr")){
  cat=$1;
  if(NF==4){
    newSubcatList[cat]=$2;
    scale[cat,$2]=$3;
    err[cat,$2]=$4;
  } else{
    newSubcatList[cat]=$1;
    scale[cat,cat]=$2;
    err[cat,cat]=$3;
  }
  #print cat, newSubcatList[cat]
}

(!match(FILENAME,"res_corr")){
  cat=$1;
  if(cat in newSubcatList){
    for(i in scale){
      split(i,a,SUBSEP);
     # print cat #, a[1], a[2]
      if(cat == a[1]){
	e=err[cat,a[2]];
	if(cat==a[2])
	  printf("%s\t%s\t%s\t%s\t%.4f\t%.4f\n", cat,  $2, $3,$4,(2-scale[cat,a[2]])*$5, sqrt($6*$6+e*e));
	else
	  printf("%s-%s\t%s\t%s\t%s\t%.4f\t%.4f\n", cat, a[2], $2, $3,$4,(2-scale[cat,a[2]])*$5, sqrt($6*$6+e*e));
      }
    }
  } else{
    print $0
  }
}
END{


}