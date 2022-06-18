#include "MyFirstDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
    kThreshold = 0,
    kNumParams = 4
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kThresholdX = 100,
  kThresholdY = 100,
  kKnobFrames = 60
};

MyFirstDistortion::MyFirstDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mThreshold(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kThreshold)->InitDouble("Threshold", 100., 0.01, 100.0, 0.01, "%");
  GetParam(kThreshold)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_RED);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob));

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
                // Make sure positive values can't go above the threshold:
                *output = fmin(*input, mThreshold);
            }
            else {
                // Make sure negative values can't go below the threshold:
                *output = fmax(*input, -mThreshold);
            }
            *output /= mThreshold;
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
    case kThreshold:
      mThreshold = GetParam(kThreshold)->Value() / 100.;
      break;

    default:
      break;
  }
}

void MyFirstDistortion::CreatePresets() {
    MakePreset("clean", 100.0);
    MakePreset("buzz", 80.0);
    MakePreset("harsh", 40.0);
    MakePreset("destroyed", 0.01);
}