#include "KickFiltersRT.h"

template<typename T>
struct bandpass_filter
{
	KickFiltersRT<T> filter;

	void init(const T input0,
	          const float low_cut_off_freq,
	          const float high_cut_off_freq,
	          const float sample_freq)
	{
		filter.initlowpass(input0, low_cut_off_freq, sample_freq);
		filter.inithighpass(input0, high_cut_off_freq, sample_freq);
	}

	T bandpass(const T input) { return filter.lowpass(filter.highpass(input)); }
};
