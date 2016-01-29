BEGIN{



}

#NF is number of fields
(!/^#/ && NF>5){
    printf("%s \t %s %.2f \t %s %.2f", $1, "&", $2, "&", $3)

    for(i=5; i<=NF;i+=3){
	v=$i-$2
	if(v<0) v*=-1
	printf("\t %s %.2f", "&", v)
    }
    printf("\\\\ \n")
}



END{



}