#ifndef __MYFIRSTDISTORTION__
#define __MYFIRSTDISTORTION__

#include "IPlug_include_in_plug_hdr.h"

class MyFirstDistortion : public IPlug
{
public:
  MyFirstDistortion(IPlugInstanceInfo instanceInfo);
  ~MyFirstDistortion();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mThreshold;
  void CreatePresets();
};

#endif
