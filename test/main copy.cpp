#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

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

// ======================== Race ==========================

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

  bool collide();
  void over();
public:
  unsigned long lShifted = millis();
  int shift = 750;

  Race() {};
  void draw();
  void update();
  void pressed(byte);
};

void Race::draw() {
  for (byte y = 0; y < 8; y++) {
    for (byte x = 0; x < 8; x++) {
      i = 8*(7-y)+x;
      if (y%2) i = 8*(7-y)+7-x;


      if (track[(index+y) % length] & (int)(pow(2, (7-x))+0.5)) {
        matrix.setPixelColor(i, colors[3]);
      } else {
        matrix.setPixelColor(i, colors[0]);
      }
    }
  }

  matrix.setPixelColor(56+pos, colors[5]);
  matrix.show();
}

void Race::update() {
  index = (index+1) % length;
  if (collide()) over();
  if (!index) shift *= 0.9;
}

void Race::pressed(byte button) {
  if (button == 1) pos += 1;
  if (button == 3) pos -= 1;
  if (collide()) over();
}

bool Race::collide() {
  if (track[index] & int(pow(2, 7-pos)+0.5)) return 1;
  return 0;
}

void Race::over() {
  /* display text*/
  draw();
  delay(100);
  for (int i = 0; i < 8; i++) {
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
  /* end */
  index = 0;
  pos = 3;
  score = 0;
  shift = 750;
}

void race();

// ======================== Snake =========================

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
  void draw();
  void update();
  void pressed(byte);
};

Snake::Snake() {
  body[0][0] = 3;
  body[0][1] = 0;

  length = 1;

  food[0] = random(8);
  food[1] = random(8);
}

void Snake::draw() {
  matrix.clear();

  index = 8*food[1]+food[0];
  if (!(food[1]%2)) index = 8*food[1]+7-food[0];
  matrix.setPixelColor(index, colors[2]);

  for (int i = 0; i < length; i++) {
    index = 8*body[i][1]+body[i][0];
    if (!(body[i][1]%2)) index = 8*body[i][1]+7-body[i][0];
    matrix.setPixelColor(index, colors[4]);
  }

  matrix.show();
}

void Snake::update () {
  for (int i = length; i > 0; i--) {
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

  for (int i = 1; i < length; i++) {
    if (body[0][0] == body[i][0] && body[0][1] == body[i][1]) return 1;
  }

  return 0;
}

void Snake::over() {
  delay(500);
  matrix.clear();
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      matrix.setPixelColor(8*i+j, colors[3]);
    }
    matrix.show();
    delay(50);
  }
  delay(200);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
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

void snake();

// ======================== Tetris ========================

class Tetris {
private:
  byte grid[8][8];
  byte size = 8;
  byte shapes[7][4][4][2]={
  /*  square  */    {
    /* angle 0   */   {  {0,0}, {1,0}, {0,1}, {1,1} },
    /* angle 90  */   {  {0,0}, {1,0}, {0,1}, {1,1} },
    /* angle 180 */   {  {0,0}, {1,0}, {0,1}, {1,1} },
    /* angle 270 */   {  {0,0}, {1,0}, {0,1}, {1,1} }
    },
  /*  bar  */        {
    /* angle 0   */   {  {0,1}, {1,1}, {2,1}, {3,1} },
    /* angle 90  */   {  {1,0}, {1,1}, {1,2}, {1,3} },
    /* angle 180 */   {  {0,1}, {1,1}, {2,1}, {3,1} },
    /* angle 270 */   {  {1,0}, {1,1}, {1,2}, {1,3} }
    },  
  /*  Z  */       {
    /* angle 0   */   {  {0,0}, {1,0}, {1,1}, {2,1} },
    /* angle 90  */   {  {2,0}, {1,1}, {2,1}, {1,2} },
    /* angle 180 */   {  {0,0}, {1,0}, {1,1}, {2,1} },
    /* angle 270 */   {  {2,0}, {1,1}, {2,1}, {1,2} }
    },   
  /*  S  */       {
    /* angle 0   */   {  {1,0}, {2,0}, {0,1}, {1,1} },
    /* angle 90  */   {  {0,0}, {0,1}, {1,1}, {1,2} },
    /* angle 180 */   {  {1,0}, {2,0}, {0,1}, {1,1} },
    /* angle 270 */   {  {0,0}, {0,1}, {1,1}, {1,2} }
    },
  /*  L  */       {
    /* angle 0   */   {  {0,1}, {1,1}, {2,1}, {0,2} },
    /* angle 90  */   {  {1,0}, {1,1}, {1,2}, {2,2} },
    /* angle 180 */   {  {2,0}, {0,1}, {1,1}, {2,1} },
    /* angle 270 */   {  {0,0}, {1,0}, {1,1}, {1,2} }   
    },
  /*  J  */       {
    /* angle 0   */   {  {0,1}, {1,1}, {2,1}, {2,2} },
    /* angle 90  */   {  {1,0}, {2,0}, {1,1}, {1,2} },
    /* angle 180 */   {  {0,0}, {0,1}, {1,1}, {2,1} },
    /* angle 270 */   {  {1,0}, {1,1}, {1,2}, {0,2} }                       
    },
  /* T  */{
    /* angle 0   */   {  {0,1}, {1,1}, {2,1}, {1,2} },
    /* angle 90  */   {  {1,0}, {1,1}, {2,1}, {1,2} },
    /* angle 180 */   {  {1,0}, {0,1}, {1,1}, {2,1} },
    /* angle 270 */   {  {1,0}, {0,1}, {1,1}, {1,2} } 
    }         
  };
  byte colorIndex[8] = {0, 1, 8, 4, 2, 7, 3, 6};
  byte sx, sy, active, rot;
  int score = 0;
  byte i;

  void create();
  void merge();
  void sweep();
  void deleteRow(byte);
  bool collide(byte, byte, byte);
  void over();
public:
  unsigned long lShifted = millis();
  int shift = 500;

  Tetris();
  void draw();
  void update();
  void pressed(byte);
};

Tetris::Tetris() {
  for(int y = 0; y < size; y++) {
    for(int x = 0; x < size; x++) {
      grid[y][x] = 0;
    }
  }

  create();
}

void Tetris::create() {
  sx = 3;
  sy = 0;
  active = random(7);
  rot = 0;
  if (collide(sx, sy, rot)) {
    over();
  }
}

void Tetris::draw() {
  for (byte y = 0; y < size; y++) {
    for (byte x = 0; x < size; x++) {
      i = 8*y+x;
      if (!(y%2)) i = 8*y+7-x;

      matrix.setPixelColor(i, colors[grid[y][x]]);
    }
  }

  for (byte j = 0; j < 4; j++) {
    byte y = sy + shapes[active][rot][j][1];
    byte x = sx + shapes[active][rot][j][0];
    i = 8*y+x;
    if (!(y%2)) i = 8*y+7-x;

    matrix.setPixelColor(i, colors[colorIndex[active+1]]);
  }

  matrix.show();
}

void Tetris::update() {
  if (!collide(sx, sy+1, rot)) {
    sy++;
  } else {
    merge();
    sweep();
    create();
  }
}

void Tetris::pressed(byte button) {
  if (button == 0 && !collide(sx, sy, (rot+1) % 4)) {
    rot = (rot+1) % 4;
  } else if (button == 1 && !collide(sx+1, sy, rot)) {
    sx++;
  } else if (button == 2) {
    lShifted = 0;
  } else if (button == 3 && !collide(sx-1, sy, rot)) {
    sx--;
  }
}

void Tetris::merge() {
  for (byte j = 0; j < 4; j++) {
    byte y = sy + shapes[active][rot][j][1];
    byte x = sx + shapes[active][rot][j][0];

    grid[y][x] = colorIndex[active+1];
  }
}

void Tetris::sweep() {
  for (byte y = size-1; y > 0; y--) {
    bool full = 1;
    for (byte x = 0; x < size; x++) {
      if (!grid[y][x]) full = 0;
    }
    if (full) {
      deleteRow(y);
      y++;
      score += 1; // ??????
    }
  }
}

void Tetris::deleteRow(byte row) {
  for (byte y = row; y >= 1; y--) {
    for (byte x = 0; x < size; x++) {
      grid[y][x] = grid[y-1][x];
    }
  }
}

bool Tetris::collide(byte nx, byte ny, byte nrot) {
  for (byte j = 0; j < 4; j++) {
    byte y = ny + shapes[active][nrot][j][1];
    byte x = nx + shapes[active][nrot][j][0];

    if (y < 0 || y >= size) return 1;
    if (x < 0 || x >= size) return 1;

    if (grid[y][x]) return 1;
  }

  return 0;
}

void Tetris::over() {
  delay(500);
  matrix.clear();
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      matrix.setPixelColor(8*i+j, colors[3]);
    }
    matrix.show();
    delay(50);
  }
  delay(200);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      matrix.setPixelColor(8*i+j, colors[0]);
    }
    matrix.show();
    delay(50);
  }
  matrix.clear();
  matrix.show();
  delay(200);

  for(int y = 0; y < size; y++) {
    for(int x = 0; x < size; x++) {
      grid[y][x] = 0;
    }
  }

  create();
}

void tetris();

// ======================== Rainbow =====================

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

// ======================== END ===========================


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
  matrix.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
  matrix.begin();

  life.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
  life.begin();

  for (int i = 0; i < 4; i++) {
    pinMode(bPins[i], INPUT_PULLUP);
  }

  bool pressed = 1;
  Rainbow r;
  while (pressed) {
    r.draw();

    matrix.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
    life.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));


    for (byte i = 0; i < 4; i++) {
      if (btnPressed(i)) pressed = 0;
    }
  }
  randomSeed(millis());

  byte mode = choose();

  delay(100);

  if (mode == 0) race();
  if (mode == 1) snake();
  if (mode == 2) tetris();
}

void race() {
  Race game;

  while (1) {
    for (byte i = 0; i < 4; i++) {
      if (btnPressed(i)) {
        game.pressed(i);
      }
    }

    game.draw();
    
    if (game.lShifted+game.shift < millis()) {
      game.update();
      game.lShifted = millis();

      matrix.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
      life.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
    }
  }
}

void snake() {
  Snake game;

  while (1) {
    for (byte i = 0; i < 4; i++) {
      if (btnPressed(i)) {
        game.pressed(i);
      }
    }

    game.draw();
    
    if (game.lShifted+game.shift < millis()) {
      game.update();
      game.lShifted = millis();

      matrix.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
      life.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
    }
  }
}

void tetris() {
  Tetris game;

  while (1) {
    for (byte i = 0; i < 4; i++) {
      if (btnPressed(i)) {
        game.pressed(i);
      }
    }

    game.draw();
    
    if (game.lShifted+game.shift < millis()) {
      game.update();
      game.lShifted = millis();

      matrix.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
      life.setBrightness(map(analogRead(BRIGHT_PIN), 0, 1023, 5, 255));
    }
  }
}

void loop() {}