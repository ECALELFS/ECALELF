BEGIN{


}
(NF!=0 && /^#/){
  print $0
}

(NF!=0 && !/^#/){
  for(c=1; c<=NF; c++){
    if(match($c,"^[ ]*[$].*pm")){
      column=$c
      gsub("[$]","",column)
      split(column,splitted,"\\\\pm")
      gsub("\\\\pm","",splitted[2])
      printf("$%.2f \\pm ", splitted[1])
      printf("%.2f$", splitted[2])
      if(c==NF) printf(" \\\\")
      else printf(" & ")
    } else 
      if(match($c,"[$][ ]*[-]?[0-9]+")){
	column=$c
	gsub("[$]","",column)
	printf("$%.2f$", column)
	if(c==NF) printf(" \\\\")
	else printf(" & ")
      } else
	printf("%s & ",$c)
  }
  printf("\n")
}






END{



}