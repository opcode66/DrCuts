#include "math.h"

class Limiter {
public:
  Limiter();
  ~Limiter();

  void Process(float inL, float inR, float* outL, float* outR);

  float gain;
  float lev;
  float thresh;
  float att;
  float rel;
  float trim;
  float knee;
};
