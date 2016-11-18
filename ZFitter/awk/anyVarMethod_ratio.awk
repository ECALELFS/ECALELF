BEGIN{
	# column 1 = catName
	# column 2 = scale
	# column 3 = modulo
    firstColumn=4 # first column to be considered for the ratio: nEvents
}


{
	line=$0
	if(NR==1){ # when processing the first line
		while(1==getline <FILENAME ){ # read once the full file looking for scale=1
			if(NF!=0 && !/#/){
				cat=$1; 
				cats[cat]=NF
				if($2==1){ #if scale==1
					for(i=firstColumn; i <=NF; i++){
						val[cat,i]=$i; 
					}
				}
			}
		}
	}
	$0=line
	cat=$1; 
	if(cat=="AAcatName") print $0 # this is the header
	else{
		printf("%s\t%.2f\t%d", cat, $2, $3) # catName and scale
		for(i=firstColumn; i <=NF; i++){
			ratio=$i/val[cat,i]
			printf("\t%f", ratio)
			i++
			printf("\t%f", ratio * sqrt($i/$(i-1))*($i/$(i-1))+(val[cat,i]/val[cat,(i-1)])*(val[cat,i]/val[cat,(i-1)]))
#    printf("\t%f", val[cat,i])
		}
		printf("\n")
	}
};


