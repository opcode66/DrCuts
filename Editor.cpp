#ifndef __editor__
#include "editor.h"
#endif

#ifndef __vstdgmtest__
#include "DrCuts.h"
#endif

#include <stdio.h>

void int2strng(VstInt32 value, char *string) {
	sprintf(string, "%d db", value);
}

void percentStringConvert(float value, char* string)
{
	 sprintf(string, "%d%%", (int)(100 * value + 0.5f));
}

void zerotohundredStringConvert(float value, char* string)
{
	 sprintf(string, "%d ms", (int)(100 * value + 0.5f));
}

void zerotofivehundredStringConvert(float value, char* string)
{
	 sprintf(string, "%d ms", (int)(500 * value + 0.5f));
}

void freqStringConvert(float value, char* string)
{
	 sprintf(string, "%d% Hz", (int)(500 * value + 0.5f));
}

void freqStringConvert2(float value, char* string)
{
	 sprintf(string, "%d% Hz", (int)(20000 * value + 0.5f));
}

void dbStringConvert(float value, char* string)
{
	int2strng((VstInt32)(120.0*value - 60.0),string);
}

void kneeStringConvert(float value, char* string)
{
	if(value < 0.5) strcpy(string, "HARD"); else strcpy(string, "SOFT");
}

Editor::Editor(AudioEffect *effect)
 : AEffGUIEditor(effect) 
{
	// load the background bitmap
	// we don't need to load all bitmaps, this could be done when open is called
	hBackground = new CBitmap(kBackgroundId);

	// init the size of the plugin
	rect.left = 0;
	rect.top = 0;
	rect.right = (short)hBackground->getWidth();
	rect.bottom = (short)hBackground->getHeight();
}

Editor::~Editor()
{
	// free the background bitmap
	if(hBackground) hBackground->forget ();
	hBackground = 0;
}

bool Editor::open(void *ptr)
{
	// !!! always call this !!!
	AEffGUIEditor::open(ptr);
	
	//--load some bitmaps
	CBitmap* hFaderBody = new CBitmap(kFaderBodyId);
	CBitmap* hFaderHandle = new CBitmap(kFaderHandleId);
	CBitmap* hOnOffButton = new CBitmap(kOnOffButtonId);
	CBitmap* hVUMeterOn = new CBitmap(kOnVUId);
	CBitmap* hVUMeterOff = new CBitmap(kOffVUId);

	//--init background frame-----------------------------------------------
	// We use a local CFrame object so that calls to setParameter won't call into objects which may not exist yet. 
	// If all GUI objects are created we assign our class member to this one. See bottom of this method.
	CRect size(0, 0, hBackground->getWidth(), hBackground->getHeight());
	CFrame* lFrame = new CFrame(size, ptr, this);
	lFrame->setBackground(hBackground);

	//--init the faders------------------------------------------------
	int minPos = kFaderY;
	int maxPos = kFaderY + hFaderBody->getHeight() - hFaderHandle->getHeight() - 1;
	CPoint point(0, 0);
	CPoint offset(1, 0);

	// Mono Mix
	size(kFaderX, kFaderY, kFaderX + hFaderBody->getWidth(), kFaderY + hFaderBody->getHeight());
	monoMixFader = new CVerticalSlider(size, this, kMonoMix, minPos, maxPos, hFaderHandle, hFaderBody, point);
	monoMixFader->setOffsetHandle(offset);
	monoMixFader->setValue(effect->getParameter(kMonoMix));
	lFrame->addView(monoMixFader);

	// Mono Freq
	size.offset(kFaderInc + hFaderBody->getWidth(), 0);
	monoFreqFader = new CVerticalSlider(size, this, kMonoFreq, minPos, maxPos, hFaderHandle, hFaderBody, point);
	monoFreqFader->setOffsetHandle(offset);
	monoFreqFader->setValue(effect->getParameter(kMonoFreq));
	lFrame->addView(monoFreqFader);

	// Invert
	size(kFaderX + 170, kFaderY, kFaderX + 170 + hOnOffButton->getWidth(), kFaderY + (hOnOffButton->getHeight() / 2));
	invertButton = new COnOffButton(size, this, kInvert, hOnOffButton, COnOffButton::kPreListenerUpdate);
	invertButton->setValue(effect->getParameter(kInvert));
	lFrame->addView(invertButton);

	// Swap
	size.offset(0, hOnOffButton->getHeight() + 10);
	swapButton = new COnOffButton(size, this, kSwap, hOnOffButton, COnOffButton::kPreListenerUpdate);
	swapButton->setValue(effect->getParameter(kSwap));
	lFrame->addView(swapButton);

	// RIAA
	size.offset(0, hOnOffButton->getHeight() + 10);
	riaaButton = new COnOffButton(size, this, kRIAA, hOnOffButton, COnOffButton::kPreListenerUpdate);
	riaaButton->setValue(effect->getParameter(kRIAA));
	lFrame->addView(riaaButton);

	// IRIAA
	size.offset(0, hOnOffButton->getHeight() + 10);
	iriaaButton = new COnOffButton(size, this, kIRIAA, hOnOffButton, COnOffButton::kPreListenerUpdate);
	iriaaButton->setValue(effect->getParameter(kIRIAA));
	lFrame->addView(iriaaButton);

	// HF Freq
	size(kFaderX + size.left, kFaderY, kFaderX + hFaderBody->getWidth() + size.left, kFaderY + hFaderBody->getHeight());
	size.offset (kFaderInc + hFaderBody->getWidth() - 2, 0);
	hfFreqFader = new CVerticalSlider(size, this, kHFFreq, minPos, maxPos, hFaderHandle, hFaderBody, point);
	hfFreqFader->setOffsetHandle(offset);
	hfFreqFader->setValue(effect->getParameter(kHFFreq));
	lFrame->addView(hfFreqFader);

	// HF Attack
	size.offset(kFaderInc + hFaderBody->getWidth() + 9, 0);
	hfAttackFader = new CVerticalSlider(size, this, kHFAttack, minPos, maxPos, hFaderHandle, hFaderBody, point);
	hfAttackFader->setOffsetHandle(offset);
	hfAttackFader->setValue(effect->getParameter(kHFAttack));
	lFrame->addView(hfAttackFader);

	// HF Release
	size.offset(kFaderInc + hFaderBody->getWidth() + 10, 0);
	hfReleaseFader = new CVerticalSlider(size, this, kHFRelease, minPos, maxPos, hFaderHandle, hFaderBody, point);
	hfReleaseFader->setOffsetHandle(offset);
	hfReleaseFader->setValue(effect->getParameter(kHFRelease));
	lFrame->addView(hfReleaseFader);

	// HF Thresh
	size.offset(kFaderInc + hFaderBody->getWidth() + 15, 0);
	hfThreshFader = new CVerticalSlider(size, this, kHFThresh, minPos, maxPos, hFaderHandle, hFaderBody, point);
	hfThreshFader->setOffsetHandle(offset);
	hfThreshFader->setValue(effect->getParameter(kHFRelease));
	lFrame->addView(hfThreshFader);

	// HF Trim
	size.offset(kFaderInc + hFaderBody->getWidth() + 15, 0);
	hfTrimFader = new CVerticalSlider(size, this, kHFTrim, minPos, maxPos, hFaderHandle, hFaderBody, point);
	hfTrimFader->setOffsetHandle(offset);
	hfTrimFader->setValue(effect->getParameter(kHFTrim));
	lFrame->addView(hfTrimFader);

	// HF Knee
	size.offset(kFaderInc + hFaderBody->getWidth() + 15, 0);
	hfKneeFader = new CVerticalSlider(size, this, kHFKnee, minPos, maxPos, hFaderHandle, hFaderBody, point);
	hfKneeFader->setOffsetHandle(offset);
	hfKneeFader->setValue(effect->getParameter(kHFRelease));
	lFrame->addView(hfKneeFader);


	//--init the display------------------------------------------------
	// Mono Mix
	size (kDisplayX, kDisplayY, kDisplayX + kDisplayXWidth, kDisplayY + kDisplayHeight);
	monoMixDisplay = new CParamDisplay(size, 0, kCenterText);
	monoMixDisplay->setFont(kNormalFontSmall);
	monoMixDisplay->setFontColor(kWhiteCColor);
	monoMixDisplay->setBackColor(kBlackCColor);
	monoMixDisplay->setFrameColor(kGreyCColor);
	monoMixDisplay->setStringConvert(percentStringConvert);
	monoMixDisplay->setValue(effect->getParameter (kMonoMix));
	lFrame->addView(monoMixDisplay);

	// Mono Freq
	size.offset (kFaderInc + hFaderBody->getWidth(), 0);
	monoFreqDisplay = new CParamDisplay(size, 0, kCenterText);
	monoFreqDisplay->setFont(kNormalFontSmall);
	monoFreqDisplay->setFontColor(kWhiteCColor);
	monoFreqDisplay->setBackColor(kBlackCColor);
	monoFreqDisplay->setFrameColor(kGreyCColor);
	monoFreqDisplay->setStringConvert(freqStringConvert);
	monoFreqDisplay->setValue(effect->getParameter(kMonoFreq));
	lFrame->addView(monoFreqDisplay);

	// HF Freq
	size (kDisplayX, kDisplayY, kDisplayX + 44, kDisplayY + kDisplayHeight);
	size.offset (((kFaderInc + hFaderBody->getWidth()) * 2) + 150, 0);
	hfFreqDisplay = new CParamDisplay(size, 0, kCenterText);
	hfFreqDisplay->setFont(kNormalFontSmall);
	hfFreqDisplay->setFontColor(kWhiteCColor);
	hfFreqDisplay->setBackColor(kBlackCColor);
	hfFreqDisplay->setFrameColor(kGreyCColor);
	hfFreqDisplay->setStringConvert(freqStringConvert2);
	hfFreqDisplay->setValue(effect->getParameter(kHFFreq));
	lFrame->addView(hfFreqDisplay);

	size (kDisplayX, kDisplayY, kDisplayX + kDisplayXWidth, kDisplayY + kDisplayHeight);
	size.offset (((kFaderInc + hFaderBody->getWidth()) * 3) + 160, 0);
	hfAttackDisplay = new CParamDisplay(size, 0, kCenterText);
	hfAttackDisplay->setFont(kNormalFontSmall);
	hfAttackDisplay->setFontColor(kWhiteCColor);
	hfAttackDisplay->setBackColor(kBlackCColor);
	hfAttackDisplay->setFrameColor(kGreyCColor);
	hfAttackDisplay->setStringConvert(zerotohundredStringConvert);
	hfAttackDisplay->setValue(effect->getParameter(kHFAttack));
	lFrame->addView(hfAttackDisplay);

	size.offset ((kFaderInc + hFaderBody->getWidth()) + 10, 0);
	hfReleaseDisplay = new CParamDisplay(size, 0, kCenterText);
	hfReleaseDisplay->setFont(kNormalFontSmall);
	hfReleaseDisplay->setFontColor(kWhiteCColor);
	hfReleaseDisplay->setBackColor(kBlackCColor);
	hfReleaseDisplay->setFrameColor(kGreyCColor);
	hfReleaseDisplay->setStringConvert(zerotofivehundredStringConvert);
	hfReleaseDisplay->setValue(effect->getParameter(kHFRelease));
	lFrame->addView(hfReleaseDisplay);

	size.offset ((kFaderInc + hFaderBody->getWidth()) + 15, 0);
	hfThreshDisplay = new CParamDisplay(size, 0, kCenterText);
	hfThreshDisplay->setFont(kNormalFontSmall);
	hfThreshDisplay->setFontColor(kWhiteCColor);
	hfThreshDisplay->setBackColor(kBlackCColor);
	hfThreshDisplay->setFrameColor(kGreyCColor);
	hfThreshDisplay->setStringConvert(percentStringConvert);
	hfThreshDisplay->setValue(effect->getParameter(kHFThresh));
	lFrame->addView(hfThreshDisplay);

	size.offset((kFaderInc + hFaderBody->getWidth()) + 15, 0);
	hfTrimDisplay = new CParamDisplay(size, 0, kCenterText);
	hfTrimDisplay->setFont(kNormalFontSmall);
	hfTrimDisplay->setFontColor(kWhiteCColor);
	hfTrimDisplay->setBackColor(kBlackCColor);
	hfTrimDisplay->setFrameColor(kGreyCColor);
	hfTrimDisplay->setStringConvert(dbStringConvert);
	hfTrimDisplay->setValue(effect->getParameter(kHFTrim));
	lFrame->addView(hfTrimDisplay);

	size.offset((kFaderInc + hFaderBody->getWidth()) + 15, 0);
	hfKneeDisplay = new CParamDisplay(size, 0, kCenterText);
	hfKneeDisplay->setFont(kNormalFontSmall);
	hfKneeDisplay->setFontColor(kWhiteCColor);
	hfKneeDisplay->setBackColor(kBlackCColor);
	hfKneeDisplay->setFrameColor(kGreyCColor);
	hfKneeDisplay->setStringConvert(kneeStringConvert);
	hfKneeDisplay->setValue(effect->getParameter(kHFKnee));
	lFrame->addView(hfKneeDisplay);

	//--init VU Meteres----------------------------------------------------
	// Left VU Meter
	size (565, 60, 593, 239);
	vuMeterLeft = new CVuMeter(size, hVUMeterOn, hVUMeterOff, 10, kVertical);
	vuMeterLeft->setValue(0.0);
	lFrame->addView(vuMeterLeft);

	// Right VU Meter
	size (609, 60, 637, 239);
	vuMeterRight = new CVuMeter(size, hVUMeterOn, hVUMeterOff, 10, kVertical);
	vuMeterRight->setValue(0.0);
	lFrame->addView(vuMeterRight);

	// Note : in the constructor of a CBitmap, the number of references is set to 1.
	// Then, each time the bitmap is used (for hinstance in a vertical slider), this
	// number is incremented.
	// As a consequence, everything happens as if the constructor by itself was adding
	// a reference. That's why we need til here a call to forget ().
	// You mustn't call delete here directly, because the bitmap is used by some CControls...
	// These "rules" apply to the other VSTGUI objects too.
	hFaderBody->forget();
	hFaderHandle->forget();
	hOnOffButton->forget();
	hVUMeterOn->forget();
	hVUMeterOff->forget();

	frame = lFrame;
	return true;
}

void Editor::close()
{
	delete frame;
	frame = 0;
}

void Editor::setParameter(VstInt32 index, float value)
{
	if(frame == 0)
		return;

	switch (index)
	{
		case kMonoMix:
			if(monoMixFader)
				monoMixFader->setValue(effect->getParameter(index));
			if(monoMixDisplay)
				monoMixDisplay->setValue(effect->getParameter(index));
			break;
		case kMonoFreq:
			if(monoFreqFader)
				monoFreqFader->setValue(effect->getParameter(index));
			if(monoFreqDisplay)
				monoFreqDisplay->setValue(effect->getParameter(index));
			break;
		case kInvert:
			if (invertButton)
				invertButton->setValue(effect->getParameter(index));
			break;
		case kSwap:
			if (swapButton)
				swapButton->setValue(effect->getParameter(index));
			break;
		case kRIAA:
			if (riaaButton)
				riaaButton->setValue(effect->getParameter(index));
			break;
		case kIRIAA:
			if (iriaaButton)
				iriaaButton->setValue(effect->getParameter(index));
			break;
		case kHFFreq:
			if(hfFreqFader)
				hfFreqFader->setValue(effect->getParameter(index));
			if(hfFreqDisplay)
				hfFreqDisplay->setValue(effect->getParameter(index));
			break;
		case kHFAttack:
			if(hfAttackFader)
				hfAttackFader->setValue(effect->getParameter(index));
			if(hfAttackFader)
				hfAttackDisplay->setValue(effect->getParameter(index));
			break;
		case kHFRelease:
			if(hfReleaseFader)
				hfReleaseFader->setValue(effect->getParameter(index));
			if(hfReleaseDisplay)
				hfReleaseDisplay->setValue(effect->getParameter(index));
			break;
		case kHFThresh:
			if(hfThreshFader)
				hfThreshFader->setValue(effect->getParameter(index));
			if(hfThreshDisplay)
				hfThreshDisplay->setValue(effect->getParameter(index));
			break;
		case kHFTrim:
			if(hfTrimFader)
				hfTrimFader->setValue(effect->getParameter(index));
			if(hfTrimDisplay)
				hfTrimDisplay->setValue(effect->getParameter(index));
			break;
		case kHFKnee:
			if(hfKneeFader)
				hfKneeFader->setValue(effect->getParameter(index));
			if(hfKneeDisplay)
				hfKneeDisplay->setValue(effect->getParameter(index));
			break;
		case kVULeft:
			if (value < 0) vuMeterLeft->setValue(-1 * value); else vuMeterLeft->setValue(value);
			break;
		case kVURight:
			if (value < 0) vuMeterRight->setValue(-1 * value); else vuMeterRight->setValue(value);
			break;
	}
}

void Editor::valueChanged(CDrawContext* context, CControl* control)
{
	long tag = control->getTag();
	switch (tag)
	{
		case kMonoMix:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kMonoFreq:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kInvert:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kSwap:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kRIAA:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kIRIAA:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kHFFreq:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kHFAttack:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kHFRelease:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kHFThresh:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kHFTrim:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;
		case kHFKnee:
			effect->setParameterAutomated(tag, control->getValue());
			control->setDirty();
			break;


	}
}