    xVar=runNumber
    if [ ! -d test/dato/miniAOD_November2015/loose/invMass_SC_corr/step1/img/stability/ ];then
	mkdir -p test/dato/miniAOD_November2015/loose/invMass_SC_corr/step1/img/stability/
    fi

    ./script/stability.sh -t  test/dato/miniAOD_November2015/loose/invMass_SC_corr/table/step1_stability-invMass_SC_corr-loose.tex --outDirImgData test/dato/miniAOD_November2015/loose/invMass_SC_corr/step1/img/stability// -x  -y peak || exit 1
    ./script/stability.sh -t  test/dato/miniAOD_November2015/loose/invMass_SC_corr/table/step1_stability-invMass_SC_corr-loose.tex --outDirImgData test/dato/miniAOD_November2015/loose/invMass_SC_corr/step1/img/stability// -x  -y scaledWidth || exit 1
    ./script/stability.sh -t  test/dato/miniAOD_November2015/loose/invMass_SC_corr/table/step1_stability-invMass_SC_corr-loose.tex --outDirImgData test/dato/miniAOD_November2015/loose/invMass_SC_corr/step1/img/stability// -x  -y peak --allRegions || exit 1: No such file or directory
