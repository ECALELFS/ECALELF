
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

#./copyTree.exe ntuple.root ntuple_out.root -1 "etaSCEle[1]>2.3"
date

#./copytree.exe ntuple_DoubleElectron-HighEtaSkim-RUN2012A-22Jan-v1-Extra.root copyTreeExtra_out_A${tag}.root -1 "$sele"
date

#./copytree.exe ntuple_DoubleElectron-HighEtaSkim-RUN2012B-22Jan-v1-Extra.root copyTreeExtra_out_B${tag}.root -1 "$sele"
date
#./copytree.exe ntuple_DoubleElectron-HighEtaSkim-RUN2012C-22Jan-v1-Extra.root copyTreeExtra_out_C${tag}.root -1 "$sele"
date
#./copytree.exe ntuple_DoubleElectron-HighEtaSkim-RUN2012D-22Jan-v1-Extra.root copyTreeExtra_out_D${tag}.root -1 "$sele"
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


