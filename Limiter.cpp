#include "Limiter.h"

Limiter::Limiter() {
}

Limiter::~Limiter() {}

void Limiter::Process(float inL, float inR, float* outL, float* outR) {
	//soft knee
	if(knee > 0.5) {
		lev = (float)(1.0 / (1.0 + thresh * fabs(inL + inR)));

		if(gain > lev) { gain = gain - att * (gain - lev); } else { gain = gain + rel * (lev - gain); }

		*outL = (inL * trim * gain);	
		*outR = (inR * trim * gain);	
	} else {
		lev = (float)(0.5 * gain * fabs(inL + inR));

		if (lev > thresh)
		{
			gain = gain - (att * (lev - thresh));
		}
		else //below threshold
		{
			gain = gain + (float)(rel * (1.0 - gain));
		}

		*outL = (inL * trim * gain);	 
		*outR = (inR * trim * gain);	
	}
}