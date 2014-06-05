
g++ -o copyTree.exe copyTree.cc \
      -pthread -m64 \
      -I${ROOTSYS}/include \
      -I./  -L${ROOTSYS}/lib \
      -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d \
      -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics \
      -lMathCore -lThread -lMinuit -lMinuit2 -lTreePlayer -lpthread \
      -Wl,-rpath,${ROOTSYS}/lib -lm -ldl

tag="_EEn"
sele="etaSCEle[1]<-2.2&&ZStat==2"

#./copyTree.exe "root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/heli/ntuple/ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012AB.root" \
# ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012AB-EEN.root \
#      -1 "etaSCEle[1]<-2.2&&ZStat==2" &

./copyTree.exe root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/heli/ntuple/ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012AB.root  ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012AB-EEP.root \
      -1 "etaSCEle[1]>2.2&&ZStat==2" &

./copyTree.exe root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/heli/ntuple/ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012C.root  ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012C-EEN.root \
      -1 "etaSCEle[1]<-2.2&&ZStat==2" &

./copyTree.exe root://eoscms//eos/cms/store/group/alca_ecalcalib/ecalelf/heli/ntuple/ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012C.root  ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012C-EEP.root \
      -1 "etaSCEle[1]>2.2&&ZStat==2" &


#./copyTree.exe ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4.root \
#   ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012ABC-EEN.root \
#      -1 "(runNumber==194533||runNumber==200519)&&etaSCEle[1]<-2.2&&ZStat==2" &

#./copyTree.exe ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4.root \
#   ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012ABC-EEP.root  \
#      -1 "(runNumber==194533||runNumber==200519)&&etaSCEle[1]>2.2&&ZStat==2" &

#./copyTree.exe ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4.root \
#   ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012D-EEN.root  \
#     -1 "(runNumber==206859)&&etaSCEle[1]<-2.2&&ZStat==2" &

#./copyTree.exe ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4.root \
#   ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012D-EEP.root \
#     -1 "(runNumber==206859)&&etaSCEle[1]>2.2&&ZStat==2" &

#./copyTree.exe ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4.root ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012AB.root  -1 "runNumber==194533" &

#./copyTree.exe ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4.root ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012C.root  -1 "runNumber==200519" &

#./copyTree.exe ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4.root ntuple_DYJets-Summer12-START53-HighEtaSkim-runDependent-Extra-V4-2012D.root  -1 "runNumber==206859"&

date

#hadd copyTreeExtra_out_ABCD${tag}.root  \
#     copyTreeExtra_out_A${tag}.root \
#     copyTreeExtra_out_B${tag}.root \
#     copyTreeExtra_out_C${tag}.root \
#     copyTreeExtra_out_D${tag}.root 

#hadd copyTreeExtra_out_ABC${tag}.root  \
#     copyTreeExtra_out_A${tag}.root \
#     copyTreeExtra_out_B${tag}.root \
#     copyTreeExtra_out_C${tag}.root \


date


