#ifndef __RIAA__
#define __RIAA__

class RIAA
{
public:
	RIAA () {};
	~RIAA () {}

	void setCoefficients(float sampleRate, bool inverseRIAA);
	void processSample(float inL, float inR, float *outL, float *outR);
	void clear();

private:
	float a0, a1, a2;
	float b0, b1, b2;
	float inL1, inL2, inR1, inR2;
	float outL1, outL2, outR1, outR2;
};

#endif // #ifndef __RIAA__
//---------------------------------------------------------------------------