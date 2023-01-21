#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


class Rainbow {
private:
  byte j = 0;

  uint32_t wheel(byte);
  void rainbowMatrix();
  void rainbowLife();
public:
  void draw();
};

uint32_t Rainbow::wheel(byte pos) {
  pos = 255 - pos;
  if(pos < 85) {
    return life.Color(255 - pos * 3, 0, pos * 3);
  }
  if(pos < 170) {
    pos -= 85;
    return life.Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return life.Color(pos * 3, 255 - pos * 3, 0);
}

void Rainbow::rainbowMatrix() {
  matrix.fill(wheel(j));
  matrix.show();
}

void Rainbow::rainbowLife() {
  //for (int i = 0; i< 32; i++) {
  //  life.setPixelColor(i, wheel(((i*256/32)+j) & 255));
  //}
  life.fill(wheel(j));
  life.show();
}

void Rainbow::draw() {
  j = (j+1) % 256;

  rainbowMatrix();
  rainbowLife();

  delay(20);
}
