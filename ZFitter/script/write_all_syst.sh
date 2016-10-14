Nom_file="test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step2-invMass_SC_corr-loose-Et_20-noPF-HggRunEtaR9.dat"

Syst_files="test/dato/December2015_Rereco_C_D_withPho/loose/invMass_SC_pho_regrCorr/table/outFile-step2-invMass_SC_pho_regrCorr-loose-Et_20-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4amctnlo-invMass_SC_corr-loose-Et_20-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4madgraph-invMass_SC_corr-loose-Et_20-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4Et_22-invMass_SC_corr-loose-Et_22-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4Et_25-invMass_SC_corr-loose-Et_25-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4medium-invMass_SC_corr-medium-Et_20-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4tight-invMass_SC_corr-tight-Et_20-noPF-HggRunEtaR9.dat"

./script/compare_datFiles.sh $Nom_file $Syst_files

########################SMEARING

Nom_file_smear=test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4-invMass_SC_corr-loose-Et_20-noPF-HggRunEtaR9.dat

#NO MC effect on smearing
Syst_files_smear="test/dato/December2015_Rereco_C_D_withPho/loose/invMass_SC_pho_regrCorr/table/outFile-step4-invMass_SC_pho_regrCorr-loose-Et_20-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4Et_22-invMass_SC_corr-loose-Et_22-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4Et_25-invMass_SC_corr-loose-Et_25-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4medium-invMass_SC_corr-medium-Et_20-noPF-HggRunEtaR9.dat test/dato/December2015_Rereco_C_D/loose/invMass_SC_corr/table/outFile-step4tight-invMass_SC_corr-tight-Et_20-noPF-HggRunEtaR9.dat"

./script/compare_datFiles.sh $Nom_file_smear $Syst_files_smear