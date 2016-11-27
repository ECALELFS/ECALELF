#ifndef STATS_HH
#define STATS_HH

#include <array>
#include <vector>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <TH1.h>
#include <TStopwatch.h>

#define MAXSIZE 10000

/** \class stats stats stats
	\brief class that provides statistical tools
*/
class stats
{
public:

	stats(std::string name = "", size_t maxSize = MAXSIZE): _name(name), _isSorted(false), _sum(0.), _sum2(0.), _n(0)
	{
		_values.reserve(maxSize);
	}; ///< default constructor

	stats(const std::vector<float>& v); ///< constructor starting from vector of values

	~stats(void) {};

	inline void clear()
	{
		_values.clear();
		_n = 0;
		_sum = 0.;
		_sum2 = 0.;
		_isSorted = false;
	};


	inline std::string name(void) const
	{
		return _name;
	};


	inline size_t n(void) const
	{
		return _n;
	}; ///< returns the number of values


	inline double mean(void) const
	{
		if(_n == 0) return 0;
		return _sum / _n;
	}; ///< returns the mean

	double mean(size_t imin, size_t imax) const; ///< returns the mean in an interval

	inline double stdDev(void) const
	{
		if(_n == 0) return 0;
		double m = mean();
		return sqrt(_sum2 / _n - m * m);
	}; ///< returns the standard devation

	std::pair<size_t, size_t> eff_sigma_interval(float q = 0.68269) const;


	/// returns the half width of the smallest interval containing a fraction q of the events
	inline float eff_sigma(float q = 0.68269) const
	{
		std::pair<size_t, size_t> interval = eff_sigma_interval(q);
		return eff_sigma(interval);
	};

	inline float eff_sigma(std::pair<size_t, size_t> interval) const
	{
		if(interval.first == interval.second) return -1;
		return (_values[interval.second] - _values[interval.first]) / 2.;
	};

	/// returns the MPV of the distribution
	float recursive_effective_mode(size_t imin, size_t imax, float q = 0.25, float e = 1e-05) const;
	inline float recursive_effective_mode(float q = 0.25, float e = 1e-05) const
	{
		return recursive_effective_mode(0, _n - 1, q, e);
	};


	float median(void) const
	{
		if(_n == 0) return 0;
		size_t i = _n / 2;
		if(_n % 2 == 1) return _values[i];
		else return 0.5 * (_values[i - 1] + _values[i]);
	};

	float min()
	{
		if(_n == 0) return 0;
		return _values[0];
	}

	float max()
	{
		if(_n == 0) return 0;
		return _values[_n - 1];
	}


	void add(double); ///< add one entry to the list of values

	void fillHisto(TH1 * h);

	inline void sort(void)
	{
		if(_isSorted) return;
		//TStopwatch ts;
		//ts.Start();
		std::sort(_values.begin(), _values.end());
		_isSorted = true;
		//ts.Stop();
		//ts.Print();
	};


	float operator[](size_t i)
	{
		return _values[i];
	};


	friend std::ostream& operator<<(std::ostream& os, const stats s);
	std::string printHeader(void);

private:
	std::string _name;
	bool _isSorted;
	double _sum;
	double _sum2;
	size_t _n;

	std::vector<float> _values;

	float eff_sigma(std::vector<float> & v, float q = 0.68269);
};

class statsCollection : std::vector<stats>
{
	typedef std::vector<stats> stats_coll_t;
	stats_coll_t _stats_coll;
public:
	void push_back(stats& s)
	{
		_stats_coll.push_back(s);
	};


	stats_coll_t::iterator begin()
	{
		return _stats_coll.begin();
	};


	stats_coll_t::iterator end()
	{
		return _stats_coll.end();
	};


	size_t size()
	{
		return _stats_coll.size();
	};


	void reset()
	{
		for(auto& s : _stats_coll) {
			s.clear();
		}
	};


	size_t nEvents()
	{
		return _stats_coll[0].n();
	};


	stats& operator[](size_t i)
	{
		return _stats_coll[i];
	}


	void dump(std::string filename)
	{
		size_t nevts = nEvents();
		std::ofstream f(filename);

		// first line
		for(auto & s : _stats_coll) {
			f << s.name() << "\t";
		}
		f << "\b\n";

		// all values
		for(size_t i = 0; i < nevts; ++i) {
			for(auto & s : _stats_coll) {
				f << s[i] << "\t";
			}
			f << "\b\n";
		}
		f << std::flush;
	}
};
#endif
