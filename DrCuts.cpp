#include "DrCuts.h"

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
  return new DrCuts(audioMaster);
}

TestProgram::TestProgram()
{
	fMonoMix = 0.5;
	fMonoFreq = 0.5;
	fInvert = 0.0;
	fSwap = 0.0;
	fRIAA = 0.0;
	fIRIAA = 0.0;
	fHFFreq = 0.9;
	fHFAttack = 0.00;
	fHFRelease = 0.01;
	fHFThresh = 0.9;
	fHFTrim = 0.5;
	fHFKnee = 1.0;
	strcpy(name, "Init");
}

//Initialization Code
DrCuts::DrCuts(audioMasterCallback audioMaster) :
AudioEffectX(audioMaster, 2, kNumParams) {
	setNumInputs (2);
	setNumOutputs (2);
	setUniqueID('Dgmv');
	canProcessReplacing(true);
	
	iVUCount = 0;
	iMonoFreq = float2Freq(fMonoFreq);
	iHFLimiterFreq = float2Freq(fHFFreq);
	dblAttack = 10;
	dblRelease = 75;
	dblCount = 0;

	initHFLimiter();
	initEQ();
	initRIAA();
	setupEQ();
	setupHFLimiter();

	editor = new Editor(this);
	programs = new TestProgram[numPrograms];
	if(programs) setProgram (0);
}

DrCuts::~DrCuts() {
	if (programs)
		delete[] programs;
}

void DrCuts::setProgram(VstInt32 program)
{
	TestProgram* tp = &programs[program];
	curProgram = program;
	setParameter(kMonoMix, tp->fMonoMix);	
	setParameter(kMonoFreq, tp->fMonoFreq);
	setParameter(kInvert, tp->fInvert);
	setParameter(kSwap, tp->fSwap);
	setParameter(kRIAA, tp->fRIAA);
	setParameter(kIRIAA, tp->fIRIAA);
	setParameter(kHFFreq, tp->fHFFreq);
	setParameter(kHFAttack, tp->fHFAttack);
	setParameter(kHFRelease, tp->fHFRelease);
	setParameter(kHFThresh, tp->fHFThresh);
	setParameter(kHFKnee, tp->fHFKnee);
	setParameter(kHFTrim, tp->fHFTrim);
}

void DrCuts::setProgramName(char *name)
{
	strcpy(programs[curProgram].name, name);
}

void DrCuts::getProgramName(char *name)
{
	if (!strcmp(programs[curProgram].name, "Init"))
		sprintf(name, "%s %d", programs[curProgram].name, curProgram + 1);
	else
		strcpy(name, programs[curProgram].name);
}

bool DrCuts::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text)
{
	if (index < kNumPrograms)
	{
		strcpy(text, programs[index].name);
		return true;
	}
	return false;
}

void DrCuts::resume()
{
	AudioEffectX::resume();
}

void DrCuts::setParameter(VstInt32 index, float value)
{
	TestProgram* tp = &programs[curProgram];

	switch(index)
	{
		case kMonoMix :
			fMonoMix = tp->fMonoMix = value;
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			break;
		case kMonoFreq : 
			tp->fMonoFreq = value;
			fMonoFreq = value;
			iMonoFreq = float2Freq(fMonoFreq);
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			if (eq2Low) setupEQ();
			break;
		case kInvert :
			tp->fInvert = value;
			fInvert = value;
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			break;

		case kSwap :
			tp->fSwap = value;
			fSwap = value;
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			break;

		case kRIAA :
			if(CBool(value) != CBool(fRIAA)) oRIAA->clear(); 
			tp->fRIAA = value;
			fRIAA = value;
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			break;

		case kIRIAA :
			if(CBool(value) != CBool(fIRIAA)) oiRIAA->clear(); 
			tp->fIRIAA = value;
			fIRIAA = value;
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			break;

		case kHFFreq :
			tp->fHFFreq = value;
			fHFFreq = value;
			iHFLimiterFreq = float2Freq(fHFFreq);
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			if (eqHFLHighRight) setupHFLimiter();
			break;

		case kHFAttack :
			tp->fHFAttack = value;
			fHFAttack = value;
			if (fHFAttack == 1) dblAttack = 100; 
			else if(fHFAttack == 0) dblAttack = 0; 
			else dblAttack = (double)(100 * fHFAttack + 0.5f);
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			if (eqHFLHighRight) setupHFLimiter();
			break;

		case kHFRelease :
			tp->fHFRelease = value;
			fHFRelease = value;
			if (fHFRelease == 1) dblRelease = 100; 
			else if(fHFRelease == 0) dblRelease = 0; 
			else dblRelease = (double)(100 * fHFRelease + 0.5f);
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			if (eqHFLHighRight) setupHFLimiter();
			break;

		case kHFThresh :
			tp->fHFThresh = value;
			fHFThresh = value;
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			if (eqHFLHighRight) setupHFLimiter();
			break;

		case kHFTrim :
			tp->fHFTrim = value;
			fHFTrim = value;
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			if (eqHFLHighRight) setupHFLimiter();
			break;

		case kHFKnee :
			tp->fHFKnee = value;
			fHFKnee = value;
			if(editor) ((AEffGUIEditor*)editor)->setParameter(index, value);
			if (eqHFLHighRight) setupHFLimiter();
			break;
	}
}

float DrCuts::getParameter(VstInt32 index)
{
	float v = 0;

	switch(index)
	{
		case kMonoMix : v = fMonoMix; break;
		case kMonoFreq : v = fMonoFreq; break;
		case kInvert : v = fInvert; break;
		case kSwap : v = fSwap; break;
		case kRIAA : v = fRIAA; break;
		case kIRIAA : v = fIRIAA; break;
		case kHFFreq : v = fHFFreq; break;
		case kHFAttack : v = fHFAttack; break;
		case kHFRelease : v = fHFRelease; break;
		case kHFThresh : v = fHFThresh; break;
		case kHFTrim : v = fHFTrim; break;
		case kHFKnee : v = fHFKnee; break;
	}
	return v;
}

void DrCuts::getParameterName(VstInt32 index, char *label)
{
	switch (index)
	{
		case kMonoMix : strcpy(label, "Mono Mix"); break;
		case kMonoFreq : strcpy(label, "Mono Frequency"); break;
		case kInvert : strcpy(label, "Invert Audio"); break;
		case kSwap : strcpy(label, "Swap Channels"); break;
		case kRIAA : strcpy(label, "RIAA EQ"); break;
		case kIRIAA : strcpy(label, "IRIAA EQ"); break;
		case kHFFreq : strcpy(label, "HF Frequency"); break;
		case kHFAttack : strcpy(label, "HF Attack"); break;
		case kHFRelease : strcpy(label, "HF Release"); break;
		case kHFThresh : strcpy(label, " HF Threshold"); break;
		case kHFTrim : strcpy(label, " HF Trim"); break;
		case kHFKnee : strcpy(label, " HF Knee"); break;
	}
}

void DrCuts::getParameterDisplay (VstInt32 index, char *text)
{
	switch (index)
	{
		case kMonoMix : 
			if (fMonoMix == 1) strcpy(text, "100"); 
			else if(fMonoMix == 0) strcpy(text, "0"); 
			else sprintf (text, "%d%", (int)(100 * fMonoMix + 0.5f));
			break;
		case kMonoFreq : 
			int2string(float2Freq(fMonoFreq), text, kVstMaxParamStrLen); 
			break;
		case kInvert : 
			if(fInvert > 0.0) strcpy(text, "On"); else strcpy(text, "Off");
			break;
		case kSwap : 
			if(fSwap > 0.0) strcpy(text, "On"); else strcpy(text, "Off");
			break;
		case kRIAA : 
			if(fRIAA > 0.0) strcpy(text, "On"); else strcpy(text, "Off");
			break;
		case kIRIAA : 
			if(fIRIAA > 0.0) strcpy(text, "On"); else strcpy(text, "Off");
			break;
		case kHFFreq : 
			if (fHFFreq == 1) strcpy(text, "20000"); 
			else if(fHFFreq == 0) strcpy(text, "0"); 
			else sprintf (text, "%d%", (int)(20000 * fHFFreq + 0.5f));
			break;
		case kHFAttack :
			int2strng((VstInt32)(-301030.1 / (getSampleRate() * log10(1.0 - att))),text);
			break;
		case kHFRelease : 
			int2strng((VstInt32)(-301.0301 / (getSampleRate() * log10(1.0 - rel))),text);
			break;
		case kHFThresh :
			int2strng((VstInt32)(40.0*fHFThresh - 40.0),text);
			break;
		case kHFTrim :
			int2strng((VstInt32)(120.0*fHFTrim - 60.0),text); 
			break;
		case kHFKnee : 
			if(fHFKnee < 0.5) strcpy(text, "HARD");
            else strcpy(text, "SOFT"); 
			break;
	}
}

void DrCuts::getParameterLabel(VstInt32 index, char *label)
{
	switch(index)
	{
		case kMonoMix  : strcpy(label, "%"); break;
		case kMonoFreq : strcpy(label, "Hz"); break;
		case kHFFreq : strcpy(label, "Hz"); break;
		case kHFAttack : strcpy(label, "ms"); break;
		case kHFRelease : strcpy(label, "ms"); break;
		case kHFThresh  : strcpy(label, "dB"); break;
		case kHFTrim  : strcpy(label, "dB"); break;
		strcpy(label, ""); break;
	}
}

bool DrCuts::getEffectName (char* name)
{
	strcpy(name, "Dr Cuts");
	return true;
}

bool DrCuts::getProductString (char* text)
{
	strcpy(text, "Dr Cuts");
	return true;
}

bool DrCuts::getVendorString (char* text)
{
	strcpy(text, "Deep Grooves Mastering");
	return true;
}

void DrCuts::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) {
	if(programs) {
		float *in1 = inputs[0];
		float *in2 = inputs[1];
		float *out1 = outputs[0];
		float *out2 = outputs[1];

		while (--sampleFrames >= 0)
		{
			outVal1 = (*in1++);
			outVal2 = (*in2++);
			inVal1 = outVal1;
			inVal2 = outVal2;
			
			processInternal(&outVal1, &outVal2);

			if(dblCount < sampleRate) {
				outVal1 = inVal1;
				outVal2 = inVal2;
				dblCount++;
			}

			//Send the modified values to the output stream
			(*out1++) = outVal1;
			(*out2++) = outVal2;
			
			if(editor) {
				iVUCount++;
				if (iVUCount >= 1024) {
					iVUCount = 0;
					((AEffGUIEditor*)editor)->setParameter(kVULeft, outVal1);
					((AEffGUIEditor*)editor)->setParameter(kVURight, outVal2);
				}
			}
		}
	}
}

void DrCuts::process(float **inputs, float **outputs, VstInt32 sampleFrames) {
	if(programs) {
		float *in1 = inputs[0];
		float *in2 = inputs[1];
		float *out1 = outputs[0];
		float *out2 = outputs[1];

		while (--sampleFrames >= 0)
		{
			outVal1 = (*in1++);
			outVal2 = (*in2++);
			inVal1 = outVal1;
			inVal2 = outVal2;
			
			processInternal(&outVal1, &outVal2);

			if(dblCount < sampleRate) {
				outVal1 = inVal1;
				outVal2 = inVal2;
				dblCount++;
			} else {
				outVal1 = inVal1 + outVal1;
				outVal2 = inVal2 + outVal2;
			}

			//Send the modified values to the output stream
			(*out1++) = outVal1;
			(*out2++) = outVal2;
			
			if(editor) {
				iVUCount++;
				if (iVUCount >= 60) {
					iVUCount = 0;
					((AEffGUIEditor*)editor)->setParameter(kVULeft, outVal1);
					((AEffGUIEditor*)editor)->setParameter(kVURight, outVal2);
				}
			}
		}
	}
}

void DrCuts::processInternal(float* outVal1, float* outVal2) {
	outVal1Low = do_3band(eq1Low, *outVal1);
	outVal2Low = do_3band(eq2Low, *outVal2);
	outVal1High = do_3band(eq1High, *outVal1);
	outVal2High = do_3band(eq2High, *outVal2);

	outValHFLHighLeft = do_3band(eqHFLHighLeft, *outVal1);
	outValHFLHighRight = do_3band(eqHFLHighRight, *outVal2);
	outValHFLLowLeft = do_3band(eqHFLLowLeft, *outVal1);
	outValHFLLowRight = do_3band(eqHFLLowRight, *outVal2);

	//Do Mono Summing
	*outVal1 = outVal1Low + (fMonoMix * outVal2Low) + outVal1High;
	*outVal2 = outVal2Low + (fMonoMix * outVal1Low) + outVal2High;

	oLimiter->Process(outValHFLHighLeft, outValHFLHighRight, &outValHFLeft, &outValHFRight);
	*outVal1 = *outVal1 + outValHFLLowLeft + outValHFLeft;
	*outVal2 = *outVal2 + outValHFLLowRight + outValHFRight; 

	//Swap channels if selected
	if(CBool(fSwap)) {
		outTemp = *outVal1;
		*outVal1 = *outVal2;
		*outVal2 = outTemp;
	}
	//Invert the audio if selected
	if(CBool(fInvert)) {
		*outVal1 = -1 * *outVal1;
		*outVal2 = -1 * *outVal2;
	}
	//RIAA EQing
	if(CBool(fRIAA)) oRIAA->processSample(*outVal1, *outVal2, outVal1, outVal2);
	//iRIAA EQing
	if(CBool(fIRIAA)) oiRIAA->processSample(*outVal1, *outVal2, outVal1 , outVal2); 
}

void DrCuts::initEQ()
{
	eq1Low = new EQSTATE;
	eq2Low = new EQSTATE;
	eq1High = new EQSTATE;
	eq2High = new EQSTATE;
}

void DrCuts::setupEQ()
{
	init_3band_state(eq1Low, iMonoFreq, 5000, 20000);
	eq1Low->lg = 1;
	eq1Low->mg = eq1Low->hg = 0;

	init_3band_state(eq2Low, iMonoFreq, 5000, 20000);
	eq2Low->lg = 1;
	eq2Low->mg = eq2Low->hg = 0;

	init_3band_state(eq1High, 0, iMonoFreq, 20000);
	eq1High->lg = eq1High->mg = 0;
	eq1High->hg = 1;

	init_3band_state(eq2High, 0, iMonoFreq, 20000);
	eq2High->lg = eq2High->mg = 0;
	eq2High->hg = 1;

	if(eqHFLLowRight) setupHFLimiter();
}

void DrCuts::initRIAA()
{	
	oRIAA = new RIAA;
	oRIAA->setCoefficients(sampleRate, false);
	oiRIAA = new RIAA;
	oiRIAA->setCoefficients(sampleRate, true);
}

void DrCuts::calcHFLimiter()
{
	//calcs here
	if(fHFKnee > 0.5) //soft knee
	{
		thresh = (float)pow(10.0, 1.0 - (2.0 * fHFThresh));
	}
	else //hard knee
	{
		thresh = (float)pow(10.0, (2.0 * fHFThresh) - 2.0);
	}

	trim = (float)(pow(10.0, (2.0 * fHFTrim) - 1.0));
	att = (float)pow(10.0, -2.0 * fHFAttack);
	rel = (float)pow(10.0, -2.0 - (3.0 * fHFRelease));

	oLimiter->knee = fHFKnee;
	oLimiter->thresh = thresh;
	oLimiter->trim = trim;
	oLimiter->att = att;
	oLimiter->rel = rel;
}

void DrCuts::initHFLimiter()
{
	eqHFLHighLeft = new EQSTATE;
	eqHFLHighRight = new EQSTATE;
	eqHFLLowLeft = new EQSTATE;
	eqHFLLowRight = new EQSTATE;
	oLimiter = new Limiter;
}

void DrCuts::setupHFLimiter()
{
	calcHFLimiter();

	init_3band_state(eqHFLHighLeft, iMonoFreq, iHFLimiterFreq, 20000);
	eqHFLHighLeft->lg = 0;
	eqHFLHighLeft->mg = 0;
	eqHFLHighLeft->hg = 1;

	init_3band_state(eqHFLHighRight, iMonoFreq, iHFLimiterFreq, 20000);
	eqHFLHighRight->lg = 0;
	eqHFLHighRight->mg = 0;
	eqHFLHighRight->hg = 1;

	init_3band_state(eqHFLLowLeft, iMonoFreq, iHFLimiterFreq, 20000);
	eqHFLLowLeft->lg = 0;
	eqHFLLowLeft->mg = 1;
	eqHFLLowLeft->hg = 0;

	init_3band_state(eqHFLLowRight, iMonoFreq, iHFLimiterFreq, 20000);
	eqHFLLowLeft->lg = 0;
	eqHFLLowLeft->mg = 1;
	eqHFLLowLeft->hg = 0;
}

int DrCuts::float2Freq(float fFreq) {
	return (int)(fFreq * 500) + 1;
}

bool DrCuts::CBool(float value) {
	if(value > 0.0) return true; else return false;
}

void DrCuts::int2strng(VstInt32 value, char *string) { sprintf(string, "%d", value); }