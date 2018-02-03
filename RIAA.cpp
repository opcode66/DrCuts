#include "RIAA.h"

void RIAA::setCoefficients(float sampleRate, bool inverseRIAA) {
	if(!inverseRIAA) {
		if(sampleRate == 44100.0) {
			a0= 1.0000000000;
			a1= 1.7007240000;
			a2= 0.7029381524;
			b0= 1.0000000000;
			b1= -0.7218922000;
			b2= -0.1860520545;
		} else {
		if(sampleRate == 48000.0) {
			a0= 1.00000000000000000;
			a1=-0.75555209999999995;
			a2=-0.16462571129709999;
			b0= 1.00000000000000000;
			b1=-1.73276550000000020;
			b2= 0.73455344362250008;
		} else {
		if(sampleRate == 82000.0) {
			a0= 1.0000000000;
			a1= -1.8554648000;
			a2= 0.8559721393;
			b0= 1.0000000000;
			b1= -0.8479577000;
			b2= -0.1127631993;
		} else {
		if(sampleRate == 96000.0) {
			a0= 1.0000000000;
			a1= -1.8666083000;
			a2= 0.8670382873;
			b0= 1.0000000000;
			b1= -0.8535331000;
			b2= -0.1104595113;		
		}}}}
	} else {
		if(sampleRate == 44100.0) {
			a0= 1.00000000000000;
			a1= -0.73845850035973;
			a2= -0.17951755477430;
			b0= 0.02675918611906;
			b1= -0.04592084787595;
			b2= 0.01921229297239;		
		} else {
		if(sampleRate == 48000.0) {
			a0= 1.00000000000000;
			a1= -0.73845850035973;
			a2= -0.17951755477430;
			b0= 0.02675918611906;
			b1= -0.04592084787595;
			b2= 0.01921229297239;		
		} else {
		if(sampleRate == 88200.0) {
			a0= 1.00000000000000;
			a1= -0.85197860443215;
			a2= -0.10921171201431;
			b0= 0.04872204977233;
			b1= -0.09076930609195;
			b2= 0.04202280710877;		
		} else {
		if(sampleRate == 96000.0) {
			a0= 1.00000000000000;
			a1= -0.85835597216218;
			a2= -0.10600020417219;
			b0= 0.05265477122714;
			b1= -0.09864197097385;
			b2= 0.04596474352090;
		}}}}
	}
}

void RIAA::processSample(float inL, float inR, float *outL, float *outR) {

	// Proces left channel
	*outL = a0*inL + a1*inL1 + a2*inL2 - b1*outL1 - b2*outL2;

	// Process right channel
	*outR = a0*inR + a1*inR1 + a2*inR2 - b1*outR1 - b2*outR2;

	// save previous I/Os
	outL2 = outL1;
	outL1 = *outL;
	inL2 = inL1;
	inL1 = inL;

	outR2 = outR1;
	outR1 = *outR;
	inR2 = inR1;
	inR1 = inR; 
}

void RIAA::clear() {
	outL2 = 0;
	outL1 = 0;
	inL2 = 0;
	inL1 = 0;

	outR2 = 0;
	outR1 = 0;
	inR2 = 0;
	inR1 = 0; 
}
