BEGIN{
	header="AAcatName"
}
($1==header){
	line=$0
}

($1!=old && $1!=header){
	old=$1; 
	printf("\n\n# [%s]\n",$1)
	print line
}
($1==old && $1!=header){
	print $0
}
