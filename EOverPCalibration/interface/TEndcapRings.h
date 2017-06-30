#ifndef TEndcapRings_h
#define TEndcapRings_h


class TEndcapRings
{
private:
	int iEndcapRing[101][101][2];

public:

	// ctor
	TEndcapRings();

	// dtor
	~TEndcapRings();

	int GetEndcapRing(int, int, int);
	int GetEndcapIeta(int, int, int);
	int GetEndcapIphi(int, int, int);
	// ClassDef(TEndcapRings,1); //ring class
};

#endif
