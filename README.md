ECALELF
=======

Code for electron calibration in CMS
Testing

======
Preliminary instructions: GitHub account
Be sure to have a GitHub account and have followed the instructions here:
`http://cms-sw.github.io/cmssw/faq.html#how_do_i_subscribe_to_github`


======
Download instructions.

`wget https://raw.github.com/ECALELFS/ECALELF/master/setup_git.sh`

`chmod +x setup_git.sh`

`./setup_git.sh CMSSW_5_3_14_patch2`

`cd CMSSW_5_3_14_patch2/src/`

`cmsenv`

`cd Calibration/ZFitter && make && cd -`

If you are using a tcsh shell:
`cd Calibration && source initCmsEnv.csh`

If you are using a bash shell:
`cd Calibration && source initCmsEnv.sh`

======
Code documentation:
Once downloaded the code, in Calibration/ you can run the command

`doxygen fulldoc`

to have the code documentation produced by doxygen opening the `doc/doxygen/fulldoc/html/index.html` with your browser 


======
Instructions for developments:
fork the repository in GIT to your own area (if you didn't it already)
`git@github.com:ECALELFS/ECALELF.git`

Add your git repository to the list of remotes to be used like:
`git remote add -f myfork git@github.com:hengne/ECALELF.git`
`git checkout devel-42X_44X_53X`


======
If you want to develop the code:
Create a new branch for your development (use a meaningful name)
`git branch myNewBranch`
Switch to the new branch: `git checkout myNewBranch`
Push it to your git repository (create a new branch with the same name also in your remote GIT repository)
`git push myfork`

Then start to develop, remember to do commits as much as possible

======
Remember to update regularly the code doing when you are in the branch devel-42X_44X_53X:
`git pull`

