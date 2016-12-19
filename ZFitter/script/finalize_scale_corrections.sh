awk -F" " '{
if($1 == "absEta_0_1-highR9"){printf $0; printf " %.4lf %.4lf \n",0.0001,0.0002};         
if($1 == "absEta_0_1-lowR9"){printf $0; printf " %.4lf %.4lf \n",0.0001,0.0002};         
if($1 == "absEta_1_1.4442-highR9"){printf $0; printf " %.4lf %.4lf \n",0.0004,0.0005};         
if($1 == "absEta_1_1.4442-lowR9"){printf $0; printf " %.4lf %.4lf \n",0.0001,0.0006};         
if($1 == "absEta_1.566_2-highR9"){printf $0; printf " %.4lf %.4lf \n",0.0004,0.0009};         
if($1 == "absEta_1.566_2-lowR9"){printf $0; printf " %.4lf %.4lf \n",0.0002,0.0007};         
if($1 == "absEta_2_2.5-highR9"){printf $0; printf " %.4lf %.4lf \n",0.0002,0.0005};         
if($1 == "absEta_2_2.5-lowR9"){printf $0; printf " %.4lf %.4lf \n",0.0003,0.0013};         
}' $1 > scale_full.dat
