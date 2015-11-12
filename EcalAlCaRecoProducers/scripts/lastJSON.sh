#!/bin/bash
echo "Last Prompt JSON: "
ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly | grep -v ZeroTesla| grep 50ns | grep -v LOWPU | tail -1
ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly | grep -v ZeroTesla| grep 50ns | grep LOWPU | tail -1

ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly | grep -v ZeroTesla| grep 25ns | grep -v LOWPU | grep -v Silver | grep -v Totem |tail -1
ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly | grep -v ZeroTesla| grep 25ns | grep -v LOWPU | grep -v Silver | grep  Totem |tail -1

ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly |grep -v ZeroTesla| grep 25ns | grep LOWPU | grep -v Silver | tail -1

ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly | grep -v ZeroTesla| grep 25ns | grep -v LOWPU | grep  Silver |tail -1



ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly |grep  ZeroTesla| grep 50ns | grep -v LOWPU| tail -1
ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly |grep  ZeroTesla| grep 50ns | grep LOWPU |  tail -1
ls -tr /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/*13TeV_PromptReco_Collisions15*_JSON*.txt | grep -v Muon |grep -v CaloOnly |grep  ZeroTesla| grep 25ns |  tail -1

