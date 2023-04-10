#include "esphome.h"

using namespace esphome;

class IFan02Output : public Component, public FloatOutput {
  public:
  void write_state(float state) override 
  {
      if(state < 0.2)
      {
        digitalWrite(21, LOW);
        digitalWrite(19, LOW);
        digitalWrite(18, LOW);
      }
      else if (state < 0.4) {
        // OFF
          digitalWrite(21, LOW);
          digitalWrite(19, LOW);
          digitalWrite(18, LOW);
          delay(1000);
          digitalWrite(21, HIGH);
          digitalWrite(19, LOW);
          digitalWrite(18, LOW);
      } else if (state < 0.6) {
        // low speed
          digitalWrite(21, LOW);
          digitalWrite(19, LOW);
          digitalWrite(18, LOW);
          delay(1000);
          digitalWrite(21, LOW);
          digitalWrite(19, HIGH);
          digitalWrite(18, LOW);
      } else if (state < 0.8) {
        // medium speed
          digitalWrite(21, LOW);
          digitalWrite(19, LOW);
          digitalWrite(18, LOW);
          delay(1000);
          digitalWrite(21, HIGH);
          digitalWrite(19, HIGH);
          digitalWrite(18, LOW);
      } else {
        // high speed
          digitalWrite(21, LOW);
          digitalWrite(19, LOW);
          digitalWrite(18, LOW);
          delay(1000);
          digitalWrite(21, LOW);
          digitalWrite(19, LOW);
          digitalWrite(18, HIGH);
      }
  }
};