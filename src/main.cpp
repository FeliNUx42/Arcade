#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


#define MATRIX_PIN 6
#define LIFE_PIN 7
#define BRIGHT_PIN 8

byte bPins[4] = {2, 3, 4, 5};
unsigned long lState[4] = {0, 0, 0, 0}, lastUp[4];
int interval = 50;

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(
  64,
  MATRIX_PIN,
  NEO_GRB + NEO_KHZ800
);

Adafruit_NeoPixel life = Adafruit_NeoPixel(
  32,
  LIFE_PIN,
  NEO_GRB + NEO_KHZ800
);

uint32_t colors[9] = {
  matrix.Color(0,  0, 0),     // black
  matrix.Color(255, 255, 0),  // yellow
  matrix.Color(0, 255, 0),    // green
  matrix.Color(0, 0, 255),    // blue
  matrix.Color(255, 0, 0),    // red
  matrix.Color(255, 255, 255),// white
  matrix.Color(200, 0, 255),  // purple
  matrix.Color(255, 165, 0),  // orange
  matrix.Color(0, 255, 255)   // turquoise
};


#include <rainbow.h>
#include <reflex.h>
#include <race.h>
#include <snake.h>
#include <tetris.h>

#include <game.h>


bool btnPressed(byte btn) {
  bool state = !digitalRead(bPins[btn]);
  bool ret = 0;
  if (state && !lState[btn] && (lastUp[btn] + interval) < millis()) {
    ret = 1;
  } else if (!state && lState[btn]) {
    lastUp[btn] = millis();
  }
  lState[btn] = state;
  return ret;
}

void adjustBrightness(int i) {
  if (!(i % 2)) return;
  
  byte brightness = matrix.getBrightness(); // or life.getBrightness();
  int8_t mult = i == 1 ? 1 : -1;
  int8_t diff = 20 * mult;

  brightness = constrain(brightness+diff, 0, 255);

  matrix.setBrightness(brightness);
  life.setBrightness(brightness);
}

void setup() {
  matrix.begin();

  life.begin();

  pinMode(BRIGHT_PIN, INPUT_PULLUP);

  for (byte i = 0; i < 4; i++) {
    pinMode(bPins[i], INPUT_PULLUP);
  }

  Rainbow r;
  byte mode = 0;
  bool pressed = false;

  while (!pressed) {
    r.draw();

    //Serial.println(matrix.getBrightness());

    for (byte i = 0; i < 4; i++) {
      if (btnPressed(i)) {
        if (digitalRead(BRIGHT_PIN)) {
          pressed = true;
          mode = i;
        } else adjustBrightness(i);
      }
    }
  }
  randomSeed(millis());

  delay(100);

  Game game(mode);

  while (1) {
    for (byte i = 0; i < 4; i++) {
      if (btnPressed(i)) {
        if (digitalRead(BRIGHT_PIN)) game.pressed(i);
        else adjustBrightness(i);
      }
    }
    
    game.draw();
    
    if (game.need_update()) game.update();
  }
}

void loop() {}