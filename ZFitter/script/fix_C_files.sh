folder=$1
for filename in $folder/*.C
do
	b=$(basename $filename .C)
	newb=$(echo $b |tr -- -. _)
	newfilename=$(dirname $filename)/${newb}.C
	sed "s/$b/$newb/" $filename >  $newfilename
	echo $b "-->" $newfilename
done
