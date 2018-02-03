#ifndef __Editor__
#define __Editor__

#include "vstgui.h"

enum {
	// bitmaps
	kBackgroundId = 128,
	kFaderBodyId,
	kFaderHandleId,
	kOnOffButtonId,
	kOnVUId,
	kOffVUId,

	kVULeft,
	kVURight,
	
	// positions
	kFaderX = 28,
	kFaderY = 60,

	kFaderInc = 25,

	kDisplayX = 22,
	kDisplayY = 224,
	kDisplayXWidth = 36,
	kDisplayHeight = 14
};

void percentStringConvert(float value, char* string);
void freqStringConvert(float value, char* string);
void freqStringConvert2(float value, char* string);
void zerotofivehundredStringConvert(float value, char* string);
void zerotohundredStringConvert(float value, char* string);
void dbStringConvert(float value, char* string);
void kneeStringConvert(float value, char* string);
void int2strng(VstInt32 value, char *string);

class Editor : public AEffGUIEditor, public CControlListener
{
public:
	Editor (AudioEffect* effect);
	virtual ~Editor ();

public:
	virtual bool open(void* ptr);
	virtual void close();

	virtual void setParameter(VstInt32 index, float value);
	virtual void valueChanged(CDrawContext* context, CControl* control);
	
private:
	// Controls
	CVerticalSlider* monoMixFader;
	CVerticalSlider* monoFreqFader;
	CVerticalSlider* hfFreqFader;
	CVerticalSlider* hfAttackFader;
	CVerticalSlider* hfReleaseFader;
	CVerticalSlider* hfThreshFader;
	CVerticalSlider* hfTrimFader;
	CVerticalSlider* hfKneeFader;

	COnOffButton* invertButton;
	COnOffButton* swapButton;
	COnOffButton* riaaButton;
	COnOffButton* iriaaButton;

	CParamDisplay* monoMixDisplay;
	CParamDisplay* monoFreqDisplay;
	CParamDisplay* hfFreqDisplay;
	CParamDisplay* hfAttackDisplay;
	CParamDisplay* hfReleaseDisplay;
	CParamDisplay* hfThreshDisplay;
	CParamDisplay* hfTrimDisplay;
	CParamDisplay* hfKneeDisplay;

	// Bitmap
	CBitmap* hBackground;

	// VU Meters
	CVuMeter* vuMeterLeft;
	CVuMeter* vuMeterRight;
};

#endif