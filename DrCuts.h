#include "audioeffectx.h"
#include "EQ3BAND.h"
#include "RIAA.h"
#include <stdio.h>
#include <string.h>
#include <Editor.h>
#include "Limiter.h"

enum
{
	// Global
	kNumPrograms = 16,

	// Parameters Tags
	kMonoMix = 0,
	kMonoFreq,
	kInvert,
	kSwap,
	kRIAA,
	kIRIAA,
	kHFFreq,
	kHFAttack,
	kHFRelease,
	kHFThresh,
	kHFTrim,
	kHFKnee,

	kNumParams
};

class TestProgram
{
friend class DrCuts;
public:
	TestProgram();
	~TestProgram() {}

private:	
	float fMonoMix;
	float fMonoFreq;
	float fInvert;
	float fSwap;
	float fRIAA;
	float fIRIAA;
	float fHFFreq;
	float fHFAttack;
	float fHFRelease;
	float fHFThresh;
	float fHFTrim;
	float fHFKnee;
	char name[24];
};

class DrCuts : public AudioEffectX {
public:
  DrCuts(audioMasterCallback audioMaster);
  ~DrCuts();

  void setupEQ();
  void initEQ();
  void initRIAA();
  void calcHFLimiter();
  void initHFLimiter();
  void setupHFLimiter();
  int float2Freq(float fFreq);
  bool CBool(float value);
  void int2strng(VstInt32 value, char *string);

  virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);
  virtual void process(float **inputs, float **outputs, VstInt32 sampleFrames);
  virtual void processInternal(float *outVal1, float *outVal2);
  virtual void setProgram(VstInt32 program);
  virtual void setProgramName(char* name);
  virtual void getProgramName(char* name);
  virtual bool getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text);
  virtual void setParameter(VstInt32 index, float value);
  virtual float getParameter(VstInt32 index);
  virtual void getParameterLabel(VstInt32 index, char* label);
  virtual void getParameterDisplay(VstInt32 index, char* text);
  virtual void getParameterName(VstInt32 index, char* text);
  virtual void resume();
  virtual bool getEffectName(char* name);
  virtual bool getVendorString(char* text);
  virtual bool getProductString(char* text);
  virtual VstInt32 getVendorVersion() { return 1000; }
  virtual VstPlugCategory getPlugCategory() { return kPlugCategEffect; }

  //Public variables for Limiter
  float thresh;
  float att;
  float rel;
  float trim;

protected:
  TestProgram* programs;

  float fMonoMix;
  float fMonoFreq;
  float fInvert;
  float fSwap;
  float fRIAA;
  float fIRIAA;
  float fHFFreq;
  float fHFAttack;
  float fHFRelease;
  float fHFThresh;
  float fHFTrim;
  float fHFKnee;
  
  float outVal1Low;
  float outVal2Low;
  float outVal1High;
  float outVal2High;
  float outValHFLLowLeft;
  float outValHFLLowRight; 
  float outValHFLHighLeft;
  float outValHFLHighRight;
  float outVal1;
  float outVal2;
  float inVal1;
  float inVal2;
  float outValHFLeft;
  float outValHFRight;
  float outTemp;

  int iMonoFreq;
  int iHFLimiterFreq;
  int iVUCount;
  double dblAttack;
  double dblRelease;

  double dblCount;
  
  EQSTATE* eq1Low;
  EQSTATE* eq2Low;
  EQSTATE* eq1High;
  EQSTATE* eq2High;
  EQSTATE* eqHFLHighLeft;
  EQSTATE* eqHFLHighRight;
  EQSTATE* eqHFLLowLeft;
  EQSTATE* eqHFLLowRight;

  RIAA* oRIAA;
  RIAA* oiRIAA;

  Limiter* oLimiter;
};
