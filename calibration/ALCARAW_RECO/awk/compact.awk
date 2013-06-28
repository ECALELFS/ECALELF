BEGIN{


first=-1
last=-1
}

(NF!=0 && first==-1){
  first=$1;
  last=$1-1;
}

(NF!=0 && first!=-1 && last!=-1){

  if ($1 == last+1){
    last=$1
  }else{
    if (first==last){
      intervals=sprintf("%s%d,",intervals,first)
#      print $1,intervals
    }else{
      intervals=sprintf("%s%d-%d,",intervals,first,last)
    }
    first=$1
    last=$1

  }
   

}


END{
    if (first==last){
      intervals=sprintf("%s%d,",intervals,first)
    }else{
      intervals=sprintf("%s%d-%d,",intervals,first,last)
    }

print intervals

}