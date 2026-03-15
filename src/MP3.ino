#define VOLUME 25
#include "kp_dfplayer_mini.h"
kp_dfplayer_mini dfplayer(Serial1);

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600);
  dfplayer.set_volume(VOLUME);
  delay(1000);
}

void loop()
{
  if (Serial.available()) {
    int value = Serial.parseInt();
  if (value > 0 && value <= 30) {
    dfplayer.set_volume(VOLUME);
    dfplayer.play_mp3(1, value);
  }
  }
}
