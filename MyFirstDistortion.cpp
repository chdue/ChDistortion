#include "MyFirstDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
    kDistortionLevel = 0,
    kGain = 1,
    kNumParams = 4
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kDistortionLevelX = (kWidth - 64)/2, // 64 is the width of MainKnob.png
  kDistortionLevelY = (kHeight - 64) / 2,
  kKnobFrames = 31,

  kGainX = kDistortionLevelX + 100,
  kGainY = kDistortionLevelY + 100,
  kGainFrames = 60
};

MyFirstDistortion::MyFirstDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mDistortionLevel(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDistortionLevel)->InitDouble("DistortionLevel", 0., 0., 99.99, 0.01, "%");
  GetParam(kDistortionLevel)->SetShape(1.);

  GetParam(kGain)->InitDouble("Makeup Gain", 0, -23.0, 10.0, 0.01, "dBs");
  GetParam(kGain)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  //pGraphics->AttachPanelBackground(&COLOR_RED);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID_NEW, KNOB_NEW, kKnobFrames);
  IBitmap gKnob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kGainFrames); //will be changed when new resources are made

  pGraphics->AttachControl(new IKnobMultiControl(this, kDistortionLevelX, kDistortionLevelY, kDistortionLevel, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &gKnob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  //MakeDefaultPreset((char *) "-", kNumPrograms);
  CreatePresets();
}

MyFirstDistortion::~MyFirstDistortion() {}

void MyFirstDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

    int const channelCount = 2;

    for (int i = 0; i < channelCount; i++) {
        double* input = inputs[i];
        double* output = outputs[i];

        for (int s = 0; s < nFrames; ++s, ++input, ++output) {
            if (*input >= 0) {
                // Make sure positive values can't go above the DistortionLevel:
                *output = fmin(*input, mDistortionLevel);
            }
            else {
                // Make sure negative values can't go below the DistortionLevel:
                *output = fmax(*input, -mDistortionLevel);
            }
            *output /= mDistortionLevel;
            *output *= mGain;
        }
    }
}

void MyFirstDistortion::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void MyFirstDistortion::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kDistortionLevel:
      mDistortionLevel = 1 - GetParam(kDistortionLevel)->Value() / 100.;
      break;

    case kGain:
      mGain = exp(GetParam(kGain)->Value() / 10); // 10 * log(ratio) = dB Change
      break;

    default:
      break;
  }
}

void MyFirstDistortion::CreatePresets() {
    MakePreset("clean", 100.0, 1.0);
    MakePreset("buzz", 80.0, 1.0);
    MakePreset("harsh", 40.0, 1.0);
    MakePreset("destroyed", 0.01, 1.0);
}