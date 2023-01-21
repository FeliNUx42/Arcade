#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <memory.h>


#define MATRIX_PIN 6
#define LIFE_PIN 7
#define MODE_PIN A0
#define BRIGHT_PIN A1

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

class Game {
public:
  Game() {}
  virtual ~Game() = 0;
  /*
  unsigned long lShifted;
  int shift;
  void pressed(byte) {};
  void draw() {};
  void update() {};*/
};
Game::~Game() {}

#include <rainbow.h>
#include <race.h>
#include <snake.h>
#include <tetris.h>


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

byte choose() {
  int val = analogRead(MODE_PIN);
  return map(val, 0, 1024, 0, 3);
}


void setup() {
  Serial.begin(9600);

  // matrix.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
  matrix.begin();

  // life.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
  life.begin();

  for (int i = 0; i < 4; i++) {
    pinMode(bPins[i], INPUT_PULLUP);
  }

  Serial.print("one\n");

  Rainbow r;
  bool pressed = false;

  while (!pressed) {
    r.draw();

    // matrix.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
    // life.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));

    for (byte i = 0; i < 4; i++) {
      if (btnPressed(i)) pressed = true;
    }
  }
  randomSeed(millis());

  byte mode = choose();

  delay(100);

  std::unique_ptr<Game> game;

  if (mode == 0) game = std::unique_ptr<Game>(new Race());
  else if (mode == 1) game = new Snake();
  else if (mode == 2) game = new Tetris();

  while (true) {

    for (byte i = 0; i < 4; i++) {
      if (btnPressed(i)) {
        game.pressed(i);
      }
    }

    game.draw();
    
    if ((game.lShifted+game.shift) < millis()) {
      game.update();
      game.lShifted = millis();

      matrix.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
      life.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
    }
  }
}

void loop() {}