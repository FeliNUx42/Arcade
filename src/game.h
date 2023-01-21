#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


class Game {
private:
  byte mode;

  Reflex reflex;
  Race race;
  Snake snake;
  Tetris tetris;
public:
  Game(byte);

  bool need_update();

  void draw();
  void update();
  void pressed(byte);
};

Game::Game(byte _mode) {
  mode = _mode;
}

bool Game::need_update() {
  unsigned long mill = millis();

  if (mode == 0 && reflex.shift + reflex.lShifted < mill) return true;
  if (mode == 1 && ((race.shift + race.lShifted) < mill)) return true;
  if (mode == 2 && snake.shift + snake.lShifted < mill) return true;
  if (mode == 3 && tetris.shift + tetris.lShifted < mill) return true;

  return false;
}

void Game::draw() {
  if (mode == 0) { reflex.drawMatrix(); reflex.drawLife(); }
  if (mode == 1) { race.drawMatrix(); race.drawLife(); }
  if (mode == 2) { snake.drawMatrix(); snake.drawLife(); }
  if (mode == 3) { tetris.drawMatrix(); tetris.drawLife(); }
}

void Game::update() {
  if (mode == 0) reflex.update();
  if (mode == 1) race.update();
  if (mode == 2) snake.update();
  if (mode == 3) tetris.update();
}

void Game::pressed(byte button) {
  if (mode == 0) reflex.pressed(button);
  if (mode == 1) race.pressed(button);
  if (mode == 2) snake.pressed(button);
  if (mode == 3) tetris.pressed(button);
}
