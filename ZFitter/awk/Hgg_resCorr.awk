BEGIN{



}


(NF!=0){

  split($1,splitted, " ");
#  print splitted[0], splitted[1]
  split($5, deltaP_splitted, "pm");
#  print $5 , deltaP_splitted[1] , deltaP_splitted[2]
  deltaP_val=deltaP_splitted[1];
  deltaP_err=deltaP_splitted[2];
  
  if(length(splitted)>1){
    key=splitted[2];
    key_diag=splitted[1];
  }   else     key=splitted[1];
#  print key;
  if(length(splitted)>1)
    {

      err2=(4*deltaP_err * deltaP_err + 1/weight_sum[key_diag]);
      deltaP_vec[key]+=(2*deltaP_val-deltaP_vec[key_diag]/weight_sum[key_diag]) / err2;
      weight_sum[key]+=1/err2;
#      print key, key_diag, (2*deltaP_val-deltaP_vec[key_diag]/weight_sum[key_diag]),deltaP_vec[key_diag]/weight_sum[key_diag] , 2* deltaP_val, err2
      
    }
  else
    {

      deltaP_vec[key]+=deltaP_val/(deltaP_err * deltaP_err);
      weight_sum[key]+=1/(deltaP_err * deltaP_err);
    }
}


END{

  for (key in deltaP_vec){
    deltaP_vec[key]=deltaP_vec[key]/weight_sum[key];
    deltaP_vec_err[key]=1/sqrt(weight_sum[key]);
    print key, deltaP_vec[key], deltaP_vec_err[key];
  }



}