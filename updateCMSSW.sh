### ECALELF side:
if [ ! -d "cmssw" ]; then
    mkdir /tmp/cmssw/
    git clone git@github.com:shervin86/cmssw.git
    ln -s cmssw /tmp/cmssw/
fi

git branch  splittingForCMSSW
git checkout splittingForCMSSW 
git filter-branch -f --prune-empty --subdirectory-filter EcalAlCaRecoProducers

git init --bare cmssw/tmpBareRepo/
git remote add cmsswMergeRepo cmssw/tmpBareRepo/  
git push cmsswMergeRepo splittingForCMSSW

cd cmssw/
git remote add -f ecalelf tmpBareRepo/
git branch mergingECALELF
git checkout mergingECALELF
git merge -s recursive -Xtheirs -Xsubtree=Calibration/EcalAlCaRecoProducers  ecalelf/splittingForCMSSW

git remote rm ecalelf 
cd ..
git checkout test2
git branch -D splittingForCMSSW
git remote rm cmsswMergeRepo

rm  cmssw/tmpBareRepo/ -Rf

exit 0
mv tmpBareRepo/ cmssw/

exit 0
exit 0
#git push origin splittingForCMSSW

#git subtree split --prefix=EcalAlCaRecoProducers -b split --squash


if [ "${CMSSW_VERSION}" != "" ];then 
git cms-init; 
git remote add -f ecalelf git@github.com:ECALELFS/ECALELF.git
git cms-addpkg Calibration
else
    git clone git@github.com:shervin86/cmssw.git
    cd cmssw
    git remote add -f ecalelf ../tmpBareRepo/
fi

git merge -s recursive -Xsubtree=Calibration/EcalAlCaRecoProducers --no-commit ecalelf/splittingForCMSSW


