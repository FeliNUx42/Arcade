#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


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
  byte colorSucc[6] = {5, 6, 4, 7, 2, 3};
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
  void drawMatrix();
  void drawLife();
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

void Tetris::drawMatrix() {
  for (byte y = 0; y < size; y++) {
    for (byte x = 0; x < size; x++) {
      if (y%2) i = 8*y + x;
      else i = 8*y + (7-x);

      matrix.setPixelColor(i, colors[grid[y][x]]);
    }
  }

  for (byte j = 0; j < 4; j++) {
    byte y = sy + shapes[active][rot][j][1];
    byte x = sx + shapes[active][rot][j][0];

    if (y%2) i = 8*y + x;
    else i = 8*y + (7-x);

    matrix.setPixelColor(i, colors[colorIndex[active+1]]);
  }

  matrix.show();
}

void Tetris::drawLife() {
  life.clear();

  for (int j = 0; j < score; j++) {
    int col = j/32;
    life.setPixelColor(j%32, colors[colorSucc[col%sizeof(colorSucc)]]);
  }

  life.show();
}

void Tetris::update() {
  lShifted = millis();
  
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
      score += 1;
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
  score = 0;

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

  for(byte y = 0; y < size; y++) {
    for(byte x = 0; x < size; x++) {
      grid[y][x] = 0;
    }
  }

  drawLife();

  create();
}
