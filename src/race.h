#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


class Race {
private:
  byte track[84] = {
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10011001,
    0b10011001,
    0b10011001,
    0b10000001,
    0b11000001,
    0b11000011,
    0b11000111,
    0b11100111,
    0b11100011,
    0b11110011,
    0b11110001,
    0b11110001,
    0b11100001,
    0b11000011,
    0b10000111,
    0b10001111,
    0b10001111,
    0b10000111,
    0b10000011,
    0b10100001,
    0b10110001,
    0b10110001,
    0b10010001,
    0b10000001,
    0b10000011,
    0b11000111,
    0b11000111,
    0b11000111,
    0b10000011,
    0b10000001,
    0b10011001,
    0b10011001,
    0b10011001,
    0b10011001,
    0b10000001,
    0b10000011,
    0b11000111,
    0b11100111,
    0b11100011,
    0b11000001,
    0b10000001,
    0b10000001,
    0b10011001,
    0b10011001,
    0b10011001,
    0b10011001,
    0b10000001,
    0b11000011,
    0b11100111,
    0b11100111,
    0b11000111,
    0b11000011,
    0b11100001,
    0b11110001,
    0b11110001,
    0b11110001,
    0b11110001,
    0b11100011,
    0b11100111,
    0b11000111,
    0b11000111,
    0b11000111,
    0b11000111,
    0b11100111,
    0b11100011,
    0b11000011,
    0b10000011,
    0b10000011,
    0b10011001,
    0b10011001,
    0b10000011,
    0b11000011,
    0b11000111,
    0b11000111,
    0b11100011,
    0b11100001,
    0b11000001,
    0b11000001
  };
  byte index = 0;
  byte pos = 3;
  byte length = sizeof(track);
  int score = 0;
  byte i;
  byte colorSucc[6] = {5, 6, 4, 7, 2, 3};

  bool collide();
  void over();
public:
  unsigned long lShifted = millis();
  int shift = 750;

  Race() {};
  void drawMatrix();
  void drawLife();
  void update();
  void pressed(byte);
};

void Race::drawMatrix() {
  for (byte y = 0; y < 8; y++) {
    for (byte x = 0; x < 8; x++) {
      if (y%2) i = 8*(7-y) + (7-x);
      else i = 8*(7-y) + x;

      bool isWall = track[(index+y) % length] & int(pow(2, (7-x))+0.5);
      if (isWall) {
        matrix.setPixelColor(i, colors[3]);
      } else {
        matrix.setPixelColor(i, colors[0]);
      }
    }
  }

  matrix.setPixelColor(56+pos, colors[5]);
  matrix.show();
}

void Race::drawLife() {
  life.clear();

  for (int j = 0; j < score/4; j++) {
    int col = j/32;
    life.setPixelColor(j%32, colors[colorSucc[col%sizeof(colorSucc)]]);
  }

  life.show();
}

void Race::update() {
  lShifted = millis();

  index = (index+1) % length;
  shift /= 1.009;
  score += 1;

  if (collide()) over();
}

void Race::pressed(byte button) {
  if (button == 1) pos += 1;
  if (button == 3) pos -= 1;

  if (collide()) over();
}

bool Race::collide() {
  return track[index] & int(pow(2, 7-pos)+0.5);
}

void Race::over() {
  drawMatrix();
  drawLife();
  delay(100);

  for (byte i = 0; i < 8; i++) {
    matrix.setPixelColor(56+pos, colors[0]);
    matrix.show();
    delay(125);
    matrix.setPixelColor(56+pos, colors[4]);
    matrix.show();
    delay(125);
  }

  matrix.clear();
  matrix.show();

  delay(500);

  index = 0;
  pos = 3;
  score = 0;
  shift = 750;
}
