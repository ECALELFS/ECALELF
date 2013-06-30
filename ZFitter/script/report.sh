#!/bin/bash

imgFormat=png
#recursively copy the result plots to public html area and then produces the report html file
#need to incorporate this in energyScale.sh so that the paths are not hard-coded
rm -r ~/public/html/calibplots/
mkdir ~/public/html/calibplots/
chmod a+x ~/public/html/calibplots/
mkdir ~/public/html/calibplots/mc/
chmod a+x ~/public/html/calibplots/mc/
mkdir ~/public/html/calibplots/data/
chmod a+x ~/public/html/calibplots/data/

cp test/data/HCP2012/WP90_PU/invMass_SC_regrCorr_pho/step3/img/*.$imgFormat ~/public/html/calibplots/data/
cp test/MC/DYJets-Summer12-START53-ZSkim-allRange/190456-203002-13Jul_PromptTS/WP90_PU/invMass_SC_regrCorr_pho/img/*.$imgFormat ~/public/html/calibplots/mc/

python ./script/makehtml.py -s ~/public/html/calibplots/data/ -d ~/public/html/calibplots/mc/ --imgFormat $imgFormat

mv Report.html ~/public/html/
chmod a+x ~/public/html/Report.html


