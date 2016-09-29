#ifndef STATS_HH
#define STATS_HH

class stats
{

	float mean(void);
	float stdDev(void);
	float n(void);

private:
	double sum;
	double sum2;
	unsigned long long int n;


};
#endif STATS_HH
