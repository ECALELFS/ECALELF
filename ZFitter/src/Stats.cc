#include "../interface/Stats.hh"

stats::stats(const std::vector<float>& v):
	_isSorted(false),
	_sum(0.),
	_sum2(0.),
	_n(0)
{
	for(auto & val : v) {
		add(val);
	}
	std::sort(_values.begin(), _values.end());
	_isSorted = true;
}

void stats::add(const double val)
{
	_values.push_back(val);
	++_n;
	_sum += val;
	_sum2 += val * val;
}


std::pair<size_t, size_t> stats::eff_sigma_interval(float q) const
{
	if (_n < 2) return std::make_pair(0, 0);
	size_t s = floor(q * _n);
	float d_min = _values[s] - _values[0];
	std::pair<size_t, size_t> interval(0, s);
	for (size_t i = s; i < _n; ++i) {
		float d = _values[i] - _values[i - s];
		if (d < d_min) {
			d_min = d;
			interval.first = i - s;
			interval.second = i;
		}
	}
	return interval;
}

double stats::mean(size_t imin, size_t imax) const
{
	double sum = 0;
	unsigned int n = 0;

	for(size_t i = imin; i < imax; ++i) {
		sum += _values[i];
		n++;
	}
	return sum / n;
}

float stats::recursive_effective_mode(size_t imin, size_t imax, float q, float e) const
{
	size_t n = imax - imin;
	if (imax == imin) return _values[imin]; // no element left
	else if (n == 1 || _values[imax] - _values[imin] < e) { // exit if 1 element left or distance < tolerance
		return 0.5 * (_values[imin] + _values[imax]);
	}
	size_t s = floor(q * n) - 1;
	float d_min = _values[imin + s] - _values[imin];
	size_t im = imin, iM = imin + s;
	for (size_t i = imin + s; i < imax; ++i) {
		float d = _values[i] - _values[i - s];
		if (d < d_min - e ) {
			d_min = d;
			im = i - s;
			iM = i;
		} else if (d - e < d_min) {
			iM = i;
		}
	}
	return recursive_effective_mode(im, iM, q, e);
}


void stats::fillHisto(TH1 * h)
{
	for (auto & v : _values) h->Fill(v);
}

/**
 * This operator defines how the stats are printed.
 *	\snippet Stats.cc STATS OUTPUT
 */
std::ostream& operator<<(std::ostream& os, const stats s)
{
	if(s.n() == 0) {
		os << s.name() << "\t" << s.n() << "\t" << "-" << "\t" << "-" << "\t" << "-" << "\t" << "-";
	} else {
		std::pair<size_t, size_t> interval = s.eff_sigma_interval();
		std::pair<size_t, size_t> interval03 = s.eff_sigma_interval(0.3);
/// [STATS OUTPUT]
		os << s.name()   << "\t"
		   << s.n()      << "\t"
		   << s.mean()   << "\t"
		   << s.stdDev() << "\t"
		   << s.median() << "\t"
		   << s.eff_sigma(interval) << "\t"
		   << s.eff_sigma(interval03) << "\t"
		   << s.mean(interval.first, interval.second) << "\t"
		   << s.mean(interval03.first, interval03.second) << "\t"
		   << s.eff_sigma(interval) / s.mean(interval.first, interval.second); // << "\t" << s.recursive_effective_mode() ;
/// [STATS OUTPUT]
	}
	return os;
}

std::string stats::printHeader(void)
{
	std::string s;
	s += "varName\t";
	s += "nEvents\t";
	s += "mean\t";
	s += "stdDev\t";
	s += "median\t";
	s += "effSigma\t";
	s += "effSigma30\t";
	s += "mean68\t";
	s += "mean30\t";
	s += "effSigmaScaled";
	return s;
}
