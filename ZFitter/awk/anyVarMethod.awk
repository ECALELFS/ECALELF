BEGIN{
	# catName varName nEvents
	firstColumn=3
}

(NF!=0){
  cat=$1;
  cats[cat]=NF
  if(cat=="catName"){  # save the header
	  header[-1]="AAcatName" ## the AA is needed to sort and get it as first line
	  header[0]="scale"
	  header[1]="nToys"
	  header[2]=""
	  for(i=firstColumn; i <=NF; i++){
		  header[i-1]=sprintf("%s\terr_%s", $i, $i)
	  }
  }  else{
	  for(i=firstColumn; i <=NF; i++){
		  sum[cat,i]+=$i; 
		  sum2[cat,i]+=$i*$i; 
	  }
	  n[cat]+=1
  }
};

END{
  for(cat in cats){
	  if(cat=="catName"){ # print the header
		  for(i=-1; i <= cats[cat]; i++){
#			  print cats[cat]
			  printf("%s\t", header[i])
		  }
		  printf("\n")
	  }else{
		  printf("%s\t%.2f", cat, scale) 
		  printf("\t%d", n[cat]);
		  for(i=firstColumn; i <= cats[cat]; i++){
			  if(n[cat] == 0){
				  printf("\t%f\t%f", 0, 0);
			  } else {
				  mean   =  sum[cat,i]/n[cat]             
					  stdDev =  sqrt(sum2[cat,i]/n[cat]-mean*mean)
				  printf("\t%f\t%f", mean, stdDev);
			  }
		  }
		  printf("\n");
	  }
  }
}

