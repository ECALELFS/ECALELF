BEGIN{
  firstColumn=3
}

(NF!=0){
  cat=$1; 
  cats[cat]=NF
  for(i=firstColumn; i <=NF; i++){
    sum[cat,i]+=$i; 
    sum2[cat,i]+=$i*$i; 
  }
  n[cat]+=1
};

END{
  for(cat in cats){
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

