#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


class Snake {
private:
  byte body[64][2];
  byte length;
  byte dir = 2;
  byte food[2];
  byte index;

  bool collide();
  void over();
public:
  unsigned long lShifted = millis();
  int shift = 500;

  Snake();
  void drawMatrix();
  void drawLife();
  void update();
  void pressed(byte);
};

Snake::Snake() {
  body[0][0] = 3;
  body[0][1] = 0;

  length = 1;

  food[0] = random(8);
  food[1] = random(8);

  life.clear();
  life.show();
}

void Snake::drawMatrix() {
  matrix.clear();

  if (food[1] % 2) index = 8*food[1] + food[0];
  else index = 8*food[1] + (7-food[0]);

  matrix.setPixelColor(index, colors[2]);

  for (byte i = 0; i < length; i++) {
    if (body[i][1]%2) index = 8*body[i][1] + body[i][0];
    else index = 8*body[i][1] + (7-body[i][0]);
    matrix.setPixelColor(index, colors[4]);
  }

  matrix.show();
}

void Snake::drawLife() {
  life.clear();

  for (byte i = 0; i < length; i++) {
    if (i < 32) life.setPixelColor(i, colors[5]);
    else life.setPixelColor(i-32, colors[6]);
  }

  life.show();
}

void Snake::update () {
  lShifted = millis();

  for (byte i = length; i > 0; i--) {
    body[i][0] = body[i-1][0];
    body[i][1] = body[i-1][1];
  }

  if (dir == 0) {
    body[0][0] = body[1][0];
    body[0][1] = body[1][1]-1;
  } else if (dir == 1) {
    body[0][0] = body[1][0]+1;
    body[0][1] = body[1][1];
  } else if (dir == 2) {
    body[0][0] = body[1][0];
    body[0][1] = body[1][1]+1;
  } else if (dir == 3) {
    body[0][0] = body[1][0]-1;
    body[0][1] = body[1][1];
  }

  if (body[0][0] == food[0] && body[0][1] == food[1]) {
    length += 1;

    food[0] = random(8);
    food[1] = random(8);
  }

  if (collide()) over();
}

void Snake::pressed(byte button) {
  dir = button;
}

bool Snake::collide() {
  if (0 > body[0][0] || body[0][0] > 7) return 1;
  if (0 > body[0][1] || body[0][1] > 7) return 1;

  for (byte i = 1; i < length; i++) {
    if (body[0][0] == body[i][0] && body[0][1] == body[i][1]) return 1;
  }

  return 0;
}

void Snake::over() {
  delay(500);
  matrix.clear();
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      matrix.setPixelColor(8*i+j, colors[3]);
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
  delay(200);

  body[0][0] = 3;
  body[0][1] = 0;

  length = 1;
  dir = 2;

  food[0] = random(8);
  food[1] = random(8);
}
