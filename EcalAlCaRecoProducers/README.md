# Producing `ECALELF NTuples` for Calibration

*The following instructions work using `MiniAOD` dataset*

* To start the n-tuple production, movee to :
```bash 
cd ${CMSSW_BASE}/Calibration/EcalAlCaRecoProducers
```

* Setup `crab3` environment
```bash
source ../initCmsEnvCRAB.sh
```

* Select a JSON that contains the runs that you are willing to run. A new JSON is published every Friday and can be found on `lxplus` machines on `/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/`  or online on this [link](https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/)

* On the configuration file `alcareco_datasets.dat` you have to add mainly the location of the dataset on the `DAS` and the run range. For example:
```org
281613-282037 /DoubleEG/Run2016H-PromptReco-v2/MINIAOD DoubleEG-Run2016H-noSkim-Prompt_v2-miniAOD caf database VALID RUN2016H
```

* The most important columns here are the run-range, the dataset bath on the `DAS` and the name you want to give to you dataset on the `eos`, `DoubleEG-Run2016H-noSkim-Prompt_v2-miniAOD` in the example. Be sure that the run-range that you are running on is in the JSON file. 

* The next step will be to run the `prod_ntuples.py`. The script will create a directory `crab-submit-{ERA}-{RUN-RANGE}` containing a skimmed JOSN file with run in the selected `RUN-RANGE` and a `crab3` configuration to be submitted by `crab submit -c crab-submit-{ERA}-{RUN-RANGE}/crab_crab-{RUN-RANGE}-{ERA}.py`. Example of command :
```bash 
python prod_ntuples.py -j Cert_271036-284044_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt -r "281613-282037" -q False --name PromtReco -g config/reRecoTags/80X_dataRun2_Prompt_v12.py

crab sumbit -c crab-submit-Run2016H-281613-282037/crab_crab-281613-282037-Run2016H.py
```

* The full list of `prod_ntuples.py` options can be displayed by typing `python prod_ntuples.py --help` :
```bash 
[user@lxplus] python prod_ntuples.py --help

Usage: prod_ntuples.py [options]

Options:
  -h, --help            show this help message and exit
  -j FILE, --json-file=FILE
                        specify the JSON file
  -r RUN_RANGE, --run-range=RUN_RANGE
                                               Specify the run range. This
                        must be present in the input
                        JSON file, and in the dataset file. The range shoud be
                        specified for example as :
                        --run-range "281613-282037"
  -n NAME, --name=NAME                         name given to the job
  -q RUN_ON_CAF, --run-on-caf=RUN_ON_CAF
                                               Option allowing CRAB jobs to
                        run on CAF queues:                       If you want
                        to choose a specific site you can edit
                        the generated crab config file
  -d FILE, --dataset-file=FILE
                                               dataset file containing the
                        information about the run range:
                        example :                       | run-range | dataset
                        path | store path radical in eos | storing unit |
                        ......                       281613-282037
                        /DoubleEG/Run2016H-PromptReco-v2/MINIAOD DoubleEG-
                        Run2016H-noSkim-Prompt_v2-miniAOD caf database VALID
                        RUN2016H
  -g FILE, --global-tag=FILE
                                               Global Tag to the corresponding
                        dataset :                       example :
                        config/reRecoTags/80X_dataRun2_Prompt_v12.py
  -t TYPE, --type=TYPE                         Type of the ntuple :
                        MINIAODNTUPLE
  --create-only=CREATE_ONLY
                        This will crate JSON file and crab3 config file
                        without submission
  --submit=SUBMIT       This will crate JSON file and crab3 config files and
                        proceseed to submission:                       This
                        command is equivalent of running : crab python/

```

# Depricated instructions
* Girare in locale (funziona)
```
##DATI
file="root://eoscms//eos/cms/store/data/Run2015D/DoubleEG/MINIAOD/PromptReco-v4/000/259/862/00000/389D079E-EA7B-E511-9506-02163E0141D8.root"
cmsRun python/alcaSkimming.py type=MINIAODNTUPLE files=$file maxEvents=1000 isCrab=0 tagFile=config/reRecoTags/phiSym_materialCorrection_ref.py
#Ti trovi una ntupla in locale:
ls ntuple_numEvent1000.root
##MC
file="/afs/cern.ch/work/g/gfasanel/CMSSW_7_4_15/src/Calibration/EcalAlCaRecoProducers/ZToEE_13TeV_powheg_M_120_200_miniAOD.root"
cmsRun python/alcaSkimming.py type=MINIAODNTUPLE files=file:$file maxEvents=1000 isCrab=0 tagFile=config/reRecoTags/phiSym_materialCorrection_ref.py
#Ti trovi una ntupla in locale:
ls ntuple_numEvent1000.root
```
 
* Per sottomettere su crab2 (NON crab3), ho modificato scripts/prodNtuples.sh in modo che:
* 1) use_dbs3 =1
* 2) lumis_per_job=12000 -> era troppo (job killed due to cpu limit usage reached)
* 3) non riesce a copiare l'output su eos. Sono riuscito a farlo scrivere su una mia cartella su /afs
* 4) il config file di crab scritto da scripts/prodNtuples appare cosi': 

https://github.com/GiuseppeFasanella/ECALELF/blob/miniAOD/EcalAlCaRecoProducers/tmp/ntuple_data.cfg
```
##DATI
cd /afs/cern.ch/work/g/gfasanel/CMSSW_7_4_15/src/Calibration/EcalAlCaRecoProducers
source ../initCmsEnvCRAB2.sh
# --skim ZSkim
./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep miniAOD| grep DoubleEG` --type MINIAOD -t config/reRecoTags/phiSym_materialCorrection_ref.py --scheduler=lsf --createOnly
crab -c prod_ntuples/MINIAODNTUPLE/DoubleEG-ZSkim-Run2015B-PromptReco-v1-miniAOD_test/251022-251883/ -submit 1
```

* Tecnicamente funziona ma non riesce a copiare l'output su eos.
* Quando creo il job mi dice
```
crab:  output files will be staged at caf.cern.ch
       with LFN's starting with = group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/MINIAODNTUPLE/DoubleEG-ZSkim-Run2015B-PromptReco-v1-miniAOD/251022-251883/unmerged/
endpoint =  root://eoscms//eos/cms/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/MINIAODNTUPLE/DoubleEG-ZSkim-Run2015B-PromptReco-v1-miniAOD/251022-251883/unmerged/
crab:  Checking remote location
crab:  WARNING: Problems trying remote dir check: 
	Failure while checking remote dir: list assignment index out of range
WARNING: Stage out of output files may fail
crab:  Creating 29 jobs, please wait...
crab:  Total of 29 jobs created.
```

* dirottando l'output su una mia cartella locale, funziona tutto


```
##MC
./scripts/prodNtuples.sh `parseDatasetFile.sh alcareco_datasets.dat | grep miniAODSIM_test` --isMC --type MINIAOD -t config/reRecoTags/phiSym_materialCorrection_ref.py --scheduler=remoteGlidein --createOnly
```

* Tranne che non riesco a scrivere nemmeno su una cartella locale (penso che quel remoteGlidein lo confonda)

```
crab:  output files will be staged at caf.cern.ch
       with LFN's starting with = /afs/cern.ch/work/g/gfasanel/CMSSW_7_4_15/src/Calibration/EcalAlCaRecoProducers/Test_miniAOD_SIM/
crab:  Checking remote location
crab:  WARNING: Problems trying remote dir check: 
	Failure while checking remote dir: 'Error checking []'
WARNING: Stage out of output files may fail
crab:  Creating 3 jobs, please wait...
crab:  Total of 3 jobs created.
```


