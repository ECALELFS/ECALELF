BEGIN{
    print "%\\documentclass[9pt, xcolor=dvipsnames]{beamer}"
    print "%\\begin{document}"
    print "\\begin{table}[htb]"
    print " \\begin{center}"
}

(NR==1){
    printf "\\begin{tabular}{"
    for(i=1; i<=(NF/3 -1 + 3);i++){ 
	printf "c"
    }
    printf "}"
    print "\\hline"
    printf "Category & \\Delta P [\\%]  & \Delta[\\%]"
    for(i=1; i<=(NF/3 -1);i++){ 
	printf " & Uncertainty[\\%]"
    }
    printf" \\\\ \\hline \n" #I want to write \\ -> you have to escape \ 

}
#NF is number of fields
(!/^#/ && NF>5){
    printf("%s \t %s %.2f \t %s %.2f", $1, "&", $2, "&", $3)
    for(i=5; i<=NF;i+=3){
	v=$i #only diff
	if(v<0) v*=-1
	printf("\t %s %.2f", "&", v)
    }
    printf(" \\\\ \n")
}

#(/^#/){
#print 
#}



END{

    print "   \\hline"
    print "    \\end{tabular}"
    print "  \\end{center}"
    print "\\end{table}"
    print "%\\end{document}"


}




