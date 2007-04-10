// #include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "Calibration/EcalCalibAlgos/interface/miscalibExample.h"
#include "Calibration/EcalCalibAlgos/interface/ElectronCalibration.h"
#include "Calibration/EcalCalibAlgos/interface/PhiSymmetryCalibration.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(miscalibExample);
DEFINE_ANOTHER_FWK_MODULE(ElectronCalibration);
DEFINE_ANOTHER_FWK_MODULE(PhiSymmetryCalibration);
