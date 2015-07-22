BEGIN{



}

(NF!=0){ 
#   if(NF!=16){ 
#     print "Error: NF = ", NF; next
#   }; 
#   if($1!=$9){ 
#     print $1, $9, "Error"; next
#   }; 
# $1 = region  -> printed
# $2 = nEvents -> printed 
# $3 = deltaM_data          -> used 
# $4 = err_deltaM_data      -> used 
# $5 = deltaM_MC            -> used
# $6 = err_deltaM_MC        -> used
# $7 = deltaP               -> not used
# $8 = err_deltaP           -> not used
# $9 = sigmaCB_data         -> used
# $10 = err_sigmaCB_data    -> used
# $11 = sigmaCB_MC          -> used
# $12 = err_sigmaCB_MC      -> used 
# $13 = old_addSmearing     -> not used
# $14 = err_old_addSmearing -> not used

  b=(91.188+$3); b2=b*b; 
  c=(91.188+$5); c2=c*c; 
  
  
#  printf("%s\t& %s", $1, $2);
  # resolution in data
  resData=$9/b*100;
  resData2=resData*resData;
  err_resData=100/b*sqrt($10*$10+$9*$9*$4*$4/b2);
  printf("\t& $%f \\pm %f$", resData, err_resData);
  #resolution in MC
  resMC=$11/c*100;
  resMC2=resMC*resMC;
  err_resMC=100/c*sqrt($12*$12+$11*$11*$6*$6/c2);
  printf("\t& $%f \\pm %f$", resMC, err_resMC);
  # additional smearing

  if(resData <= resMC)  printf("\t& --");
  else {
    addSmearing=sqrt(2*(resData2-resMC2));
    err_addSmearing=2*sqrt(			 (resData*err_resData/addSmearing)*(resData*err_resData/addSmearing) +  			 (resMC*err_resMC/addSmearing)*(resMC*err_resMC/addSmearing) 			 );
    printf("\t& $%f \\pm %f$", addSmearing, err_addSmearing);
  }

#  printf("\t \\\\ \n");
  #, , 
}


END{



}
