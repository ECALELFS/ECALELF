BEGIN{


first=-1
last=-1
}

(NF==1){
	print $0
	intervals=""
}

(NF>1 ){
	status=$1
	first=$2
	last=first-1
	intervals=""
	for(i=2; i <=NF; i++){
		
		if ($i == last+1){
			last=$i
		}else{
			if (first==last){
				intervals=sprintf("%s%d,",intervals,first)
#      print $1,intervals
			}else{
				intervals=sprintf("%s%d-%d,",intervals,first,last)
			}
			first=$i
			last=$i
			
		}
	}
    if (first==last){
      intervals=sprintf("%s%d,",intervals,first)
    }else{
      intervals=sprintf("%s%d-%d,",intervals,first,last)
    }
	printf("[%s] %s\n", status, intervals)

}


END{

}