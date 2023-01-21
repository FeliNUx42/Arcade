#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


class Reflex {
private:
  byte shapes[10][8] = {
    {
      0b00000000,
      0b01100000,
      0b01000000,
      0b01100000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00001010,
      0b00001110,
      0b00000000
    },
    {
      0b00000000,
      0b01100000,
      0b00100000,
      0b01100000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00001110,
      0b00001010,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00010000,
      0b00010000,
      0b00010000,
      0b00010000,
      0b00000000,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00001100,
      0b00001100,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b01100000,
      0b00100000,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00011000,
      0b00111100,
      0b00111100,
      0b00011000,
      0b00000000,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00111000,
      0b00111000,
      0b00111000,
      0b00000000,
      0b00000000,
      0b00000000
    },
    {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00001110,
      0b00001110,
      0b00000000,
      0b00000000
    }
  };
  
  byte active_shape = 0;
  byte last_shape = -1;
  byte points[4] = {0,0,0,0};
  byte i;
  byte frequency = 4;
  bool visible = false;
  bool active = false;

  void setShape();
  void won(byte player);
  void lost(byte player);
  void over(byte player);
public:
  unsigned long lShifted = millis();
  int shift = 800;

  Reflex() {};
  void drawMatrix(uint32_t);
  void drawLife();
  void update();
  void pressed(byte);
};

void Reflex::drawMatrix(uint32_t col = colors[5]) {
  if (!visible) {
    matrix.clear();
    matrix.show();
    return;
  }

  for (byte y = 0; y < 8; y++) {
    for (byte x = 0; x < 8; x++) {

      if (y%2) i = 8*(7-y) + (7-x);
      else i = 8*(7-y) + x;

      bool isShape = shapes[active_shape][y] & int(pow(2, (7-x))+0.5);

      if (isShape) {
        matrix.setPixelColor(i, col);
      } else {
        matrix.setPixelColor(i, colors[0]);
      }
    }
  }

  matrix.show();
}

void Reflex::drawLife () {
  life.clear();

  for (byte player = 0; player < 4; player++) {
    for (byte p = 1; p <= points[player]; p++) {
      i = player*8 + 8-p;

      life.setPixelColor(i, colors[5]);
    }
  }

  life.show();
}

void Reflex::update() {
  lShifted = millis();

  if (!visible) setShape();

  visible = !visible;
}

void Reflex::pressed(byte button) {
  if (!active) return;
  active = false;

  delay(100);

  if (active_shape == last_shape) won(button);
  else lost(button);

  lShifted = millis();

  byte _shape = random(sizeof(shapes)/8 - 1);
  last_shape = -1;
  active_shape = _shape < active_shape ? _shape : _shape + 1;
  active = true;
}

void Reflex::setShape() {
  active = true;
  last_shape = active_shape;

  if (!random(frequency)) return;

  byte _shape = random(sizeof(shapes)/8 - 1);
  active_shape = _shape < last_shape ? _shape : _shape + 1;
}

void Reflex::won(byte player) {
  points[player]++;

  drawMatrix(colors[player+1]);

  life.fill(colors[2]);
  life.show();

  if (points[player] > 8) over(player);

  delay(1500);
}

void Reflex::lost(byte player) {
  points[player] = constrain(points[player]-1, 0, 8);

  drawMatrix(colors[player+1]);

  life.fill(colors[4]);
  life.show();

  delay(1500);
}

void Reflex::over(byte player) {
  delay(500);

  matrix.clear();

  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      matrix.setPixelColor(8*i+j, colors[player+1]);
    }
    matrix.show();
    delay(50);
  }

  delay(200);
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      matrix.setPixelColor(8*i+j, colors[0]);
    }
    matrix.show();
    delay(50);
  }

  matrix.clear();
  matrix.show();
  life.clear();
  life.show();
  delay(200);

  points[0] = 0;
  points[1] = 0;
  points[2] = 0;
  points[3] = 0;

  lShifted = 0;
  visible = false;
}