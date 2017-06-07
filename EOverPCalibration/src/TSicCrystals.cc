#include "Calibration/EOverPCalibration/interface/TSicCrystals.h"

// default constructor, reading the map from file
TSicCrystals::TSicCrystals()
{
	int ix, iy, iz;
	for (ix = 0; ix < 100; ix++) {
		for (iy = 0; iy < 100; iy++) {
			for (iz = -1; iz < 2; iz++) sicMap[ix][iy][iz] = 0;
		}
	}

	FILE *fSic;
	fSic = fopen("./interface/SIC_ixiyiz.list", "r");
	std::cout << "Reading SIC crystals map: SIC_ixiyiz.list" << std::endl;
	while(fscanf(fSic, "%d %d %d \n", &ix, &iy, &iz) != EOF ) {
		sicMap[ix][iy][iz] = 1;
	}
	return;

}
TSicCrystals::~TSicCrystals()
{
	return;
}

int TSicCrystals::isSic(Int_t ix, Int_t iy, Int_t iz)
{
	return sicMap[ix][iy][iz];
}
