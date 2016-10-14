#! /bin/bash

####reReco
./script/calibration_8_categories.sh Rereco_December2015 invMass_SC_corr reReco_Ele_regrCorr --initFile=step2/init_1_rereco.txt --initFile=step2/init_2_rereco.txt CatOnly
#./script/calibration_8_categories.sh Rereco_December2015 invMass_SC_pho_regrCorr reReco_Pho_regrCorr --initFile=step2/init_1_rereco.txt --initFile=step2/init_2_rereco.txt slide

####promptReco => not useful anymore
#./script/calibration_8_categories.sh miniAOD_November2015_withPho invMass_SC_pho_regrCorr promptReco_Pho_regrCorr --initFile=step2/init_1.txt --initFile=step2/init_2.txt slide


