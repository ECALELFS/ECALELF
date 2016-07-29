# https://hypernews.cern.ch/HyperNews/CMS/get/ecal-calibration/919.html
# The sqlite file contains 3 IOVs:

# IOV 4:
#          run range analysed: 274958 (11 June 2016) to 275659 (23 June)
#          lone bunch: no (high E thresholds)
#          IOV runs: same as analysis range
#          Notes: 200 ps shift
#          File:  /afs/cern.ch/work/e/emanuele/public/ecal/pulseshapes_db/template_histograms_ECAL_Run2016_runs_274958_275659.txt

# Validation plots: 
#          https://emanuele.web.cern.ch/emanuele/ECAL/reco/PSTag/IOVs/2016/IOV4/?match=sample1.

# IOV 5:
#          run range analysed: 275757 (25 June) to 275931 (29 June)
#          lone bunch: no (high E thresholds)
#          IOV runs: same as analysis range
#          Notes: 100 ps shift, but before Off
#          File:  /afs/cern.ch/work/e/emanuele/public/ecal/pulseshapes_db/template_histograms_ECAL_Run2016_runs_275757_275931.txt

# Validation plots: 
#          https://emanuele.web.cern.ch/emanuele/ECAL/reco/PSTag/IOVs/2016/IOV5/?match=sample1.

# IOV 6
#          run range analysed: 276315 (4 July) to 276587 (10 July)
#          lone bunch: no (high E thresholds)
#          IOV runs: 276315 to infinity
#          Notes: stable timing after Boff
#          File:  /afs/cern.ch/work/e/emanuele/public/ecal/pulseshapes_db/template_histograms_ECAL_Run2016_runs_276315_276587.txt

# Validation plots: 
#          https://emanuele.web.cern.ch/emanuele/ECAL/reco/PSTag/IOVs/2016/IOV6/?match=sample1.

# [3] DB File:
#          sqlite file:   /afs/cern.ch/work/e/emanuele/public/ecal/pulseshapes_db/ecaltemplates_popcon_data_Run2016BC_since_274958.db

# Updated payload:
# conddb --db /afs/cern.ch/work/e/emanuele/public/ecal/pulseshapes_db/ecaltemplates_popcon_data_Run2016BC_since_274958.db list EcalPulseShapes_data

# Since: Run   Insertion Time              Payload                                   Object Type
# -----------  --------------------------  ----------------------------------------  ---------------------------------------
# 274958       2016-07-21 09:43:01.336759  8d2e3964c3e5d01a26796ebf0ffc385d556fbc51  EcalCondObjectContainer<EcalPulseShape>
# 275757       2016-07-21 09:43:02.253582  3bc574c9a7eddce037b5ac33b75e51254139a7b2  EcalCondObjectContainer<EcalPulseShape>
# 276315       2016-07-21 09:43:03.106900  5c2a3a69454cdf3df32d84782af23d1b65917764  EcalCondObjectContainer<EcalPulseShape>

import FWCore.ParameterSet.Config as cms

from CondCore.DBCommon.CondDBSetup_cfi import *

RerecoGlobalTag = cms.ESSource("PoolDBESSource",
                               CondDBSetup,
                               connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
                               globaltag = cms.string('80X_dataRun2_Prompt_v10'),
                               toGet = cms.VPSet(
                                             cms.PSet(record = cms.string("EcalPulseShapesRcd"),
                                                      tag = cms.string("EcalPulseShapes_data"),
                                                      connect = cms.string("sqlite_file:/afs/cern.ch/cms/CAF/CMSALCA/ALCA_ECALCALIB/pulseShapes/Prompt_2016/tags/sqlite/ecaltemplates_popcon_data_Run2016BC_since_274958.db"),
                                                      ),
                                             )
                               )
