($1!=old){
	old=$1; 
	printf("\n\n# [%s]\n",$1)
}
($1==old){
	print $0
}
