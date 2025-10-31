#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= PIN DEFINITIONS ================= //
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED Pins (I2C)
#define OLED_SDA 8
#define OLED_SCL 9

// Joystick Pins  
#define JOYSTICK_X A0    // GPIO 1
#define JOYSTICK_Y A1    // GPIO 2
#define JOYSTICK_BUTTON 4

// Other Components
#define TOUCH_PIN 5
#define SPEAKER_PIN 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= TETRIS GAME VARIABLES ================= //
#define GRID_WIDTH 10
#define GRID_HEIGHT 16
#define CELL_SIZE 5
#define GRID_X 2
#define GRID_Y 2

// Tetromino shapes
const uint8_t tetrominos[7][4][4] = {
  {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
  {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
  {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
  {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
  {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
  {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
  {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}
};

// Game variables
uint8_t grid[GRID_HEIGHT][GRID_WIDTH] = {0};

struct Tetromino {
  int x, y;
  int type;
  int rotation;
};

Tetromino current;
Tetromino next;

int score = 0;
int level = 1;
int linesCleared = 0;
bool gameRunning = false;
bool gameOver = false;

unsigned long lastDropTime = 0;
int dropSpeed = 800;

// ================= JOYSTICK CONTROLS ================= //
unsigned long lastLeftMove = 0;
unsigned long lastRightMove = 0;
unsigned long lastButtonTime = 0;

const int MOVE_DELAY = 200;
const int BUTTON_DELAY = 500;
const int DEAD_ZONE = 500;

// ================= SOUND FUNCTIONS ================= //
void playTone(int frequency, int duration) {
  tone(SPEAKER_PIN, frequency, duration);
}

void playMoveSound() {
  playTone(300, 20);
}

void playRotateSound() {
  playTone(400, 30);
}

void playDropSound() {
  playTone(200, 40);
}

void playLineClearSound() {
  playTone(600, 80);
  delay(40);
  playTone(800, 80);
}

void playLevelUpSound() {
  playTone(800, 100);
  delay(80);
  playTone(1000, 100);
  delay(80);
  playTone(1200, 150);
}

void playGameOverSound() {
  for (int i = 400; i > 200; i -= 30) {
    playTone(i, 50);
    delay(40);
  }
}

void playStartSound() {
  playTone(500, 80);
  delay(60);
  playTone(700, 80);
  delay(60);
  playTone(900, 120);
}

// ================= GAME FUNCTIONS ================= //
void resetGame() {
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      grid[y][x] = 0;
    }
  }
  
  score = 0;
  level = 1;
  linesCleared = 0;
  dropSpeed = 800;
  gameRunning = true;
  gameOver = false;
  
  spawnNewPiece();
  generateNextPiece();
  
  playStartSound();
}

void spawnNewPiece() {
  current.x = GRID_WIDTH / 2 - 2;
  current.y = 0;
  current.type = next.type;
  current.rotation = 0;
  
  generateNextPiece();
  
  if (checkCollision(current.x, current.y, current.rotation)) {
    gameOver = true;
    gameRunning = false;
    playGameOverSound();
  }
}

void generateNextPiece() {
  next.type = random(0, 7);
  next.rotation = 0;
}

bool checkCollision(int x, int y, int rotation) {
  for (int py = 0; py < 4; py++) {
    for (int px = 0; px < 4; px++) {
      if (tetrominos[current.type][py][px]) {
        int gridX = x + px;
        int gridY = y + py;
        
        if (gridX < 0 || gridX >= GRID_WIDTH || gridY >= GRID_HEIGHT) {
          return true;
        }
        
        if (gridY >= 0 && grid[gridY][gridX]) {
          return true;
        }
      }
    }
  }
  return false;
}

void lockPiece() {
  for (int py = 0; py < 4; py++) {
    for (int px = 0; px < 4; px++) {
      if (tetrominos[current.type][py][px]) {
        int gridX = current.x + px;
        int gridY = current.y + py;
        if (gridY >= 0) {
          grid[gridY][gridX] = current.type + 1;
        }
      }
    }
  }
  playDropSound();
  clearLines();
  spawnNewPiece();
}

void clearLines() {
  int linesClearedThisTurn = 0;
  
  for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
    bool lineComplete = true;
    for (int x = 0; x < GRID_WIDTH; x++) {
      if (!grid[y][x]) {
        lineComplete = false;
        break;
      }
    }
    
    if (lineComplete) {
      linesClearedThisTurn++;
      for (int ny = y; ny > 0; ny--) {
        for (int x = 0; x < GRID_WIDTH; x++) {
          grid[ny][x] = grid[ny - 1][x];
        }
      }
      for (int x = 0; x < GRID_WIDTH; x++) {
        grid[0][x] = 0;
      }
      y++;
    }
  }
  
  if (linesClearedThisTurn > 0) {
    linesCleared += linesClearedThisTurn;
    
    int points = 0;
    switch (linesClearedThisTurn) {
      case 1: points = 40 * level; break;
      case 2: points = 100 * level; break;
      case 3: points = 300 * level; break;
      case 4: points = 1200 * level; break;
    }
    score += points;
    
    int newLevel = 1 + (linesCleared / 5);
    if (newLevel > level) {
      level = newLevel;
      dropSpeed = max(100, 800 - (level - 1) * 80);
      playLevelUpSound();
    }
    
    playLineClearSound();
  }
}

void rotatePiece() {
  int newRotation = (current.rotation + 1) % 4;
  if (!checkCollision(current.x, current.y, newRotation)) {
    current.rotation = newRotation;
    playRotateSound();
  }
}

void moveLeft() {
  if (!checkCollision(current.x - 1, current.y, current.rotation)) {
    current.x--;
    playMoveSound();
  }
}

void moveRight() {
  if (!checkCollision(current.x + 1, current.y, current.rotation)) {
    current.x++;
    playMoveSound();
  }
}

void softDrop() {
  if (!checkCollision(current.x, current.y + 1, current.rotation)) {
    current.y++;
    score += 2;
  }
}

void hardDrop() {
  while (!checkCollision(current.x, current.y + 1, current.rotation)) {
    current.y++;
    score += 4;
  }
  lockPiece();
}

// ================= JOYSTICK CONTROLS ================= //
void readJoystick() {
  int xValue = analogRead(JOYSTICK_X);
  bool buttonPressed = digitalRead(JOYSTICK_BUTTON) == LOW;
  
  unsigned long currentTime = millis();
  
  if (xValue < (2048 - DEAD_ZONE)) {
    if (currentTime - lastLeftMove > MOVE_DELAY) {
      moveLeft();
      lastLeftMove = currentTime;
    }
  }
  else if (xValue > (2048 + DEAD_ZONE)) {
    if (currentTime - lastRightMove > MOVE_DELAY) {
      moveRight();
      lastRightMove = currentTime;
    }
  }
  
  if (buttonPressed && (currentTime - lastButtonTime > BUTTON_DELAY)) {
    hardDrop();
    lastButtonTime = currentTime;
  }
}

// ================= TOUCH CONTROLS ================= //
void checkTouch() {
  static bool lastTouchState = HIGH;
  bool currentTouchState = digitalRead(TOUCH_PIN);
  
  if (currentTouchState == LOW && lastTouchState == HIGH) {
    if (!gameRunning) {
      if (gameOver) {
        resetGame();
      } else {
        gameRunning = true;
        playStartSound();
      }
    } else {
      rotatePiece();
    }
    delay(200);
  }
  
  lastTouchState = currentTouchState;
}

void updateGame() {
  if (!gameRunning) return;
  
  if (millis() - lastDropTime > dropSpeed) {
    if (!checkCollision(current.x, current.y + 1, current.rotation)) {
      current.y++;
    } else {
      lockPiece();
    }
    lastDropTime = millis();
  }
}

void drawGame() {
  display.clearDisplay();
  
  // Draw main game area
  int gameWidth = GRID_WIDTH * CELL_SIZE;
  int gameHeight = GRID_HEIGHT * CELL_SIZE;
  
  display.drawRect(GRID_X, GRID_Y, gameWidth, gameHeight, SSD1306_WHITE);
  
  // Draw grid cells
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      if (grid[y][x]) {
        display.fillRect(GRID_X + x * CELL_SIZE + 1, 
                        GRID_Y + y * CELL_SIZE + 1,
                        CELL_SIZE - 1, CELL_SIZE - 1, 
                        SSD1306_WHITE);
      }
    }
  }
  
  // Draw current piece
  for (int py = 0; py < 4; py++) {
    for (int px = 0; px < 4; px++) {
      if (tetrominos[current.type][py][px]) {
        int drawX = GRID_X + (current.x + px) * CELL_SIZE;
        int drawY = GRID_Y + (current.y + py) * CELL_SIZE;
        if (drawY >= GRID_Y) {
          display.fillRect(drawX + 1, drawY + 1, 
                          CELL_SIZE - 1, CELL_SIZE - 1, 
                          SSD1306_WHITE);
        }
      }
    }
  }
  
  // ================= FINAL FIXED UI POSITIONING ================= //
  int panelX = GRID_X + gameWidth + 4;
  
  // NEXT piece (moved higher)
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(panelX, 0);  // Moved to very top
  display.print("NEXT");
  
  int nextX = panelX + 2;
  int nextY = 8;  // Reduced from 12
  for (int py = 0; py < 4; py++) {
    for (int px = 0; px < 4; px++) {
      if (tetrominos[next.type][py][px]) {
        display.fillRect(nextX + px * (CELL_SIZE - 1), 
                        nextY + py * (CELL_SIZE - 1),
                        CELL_SIZE - 2, CELL_SIZE - 2, 
                        SSD1306_WHITE);
      }
    }
  }
  
  // SCORE (moved much higher)
  display.setCursor(panelX, nextY + 12);  // Reduced from 18
  display.print("SCORE:");
  display.setCursor(panelX, nextY + 22);  // Reduced from 28
  display.print(score);
  
  // LEVEL (moved much higher - NOW VISIBLE!)
  display.setCursor(panelX, nextY + 32);  // Reduced from 40
  display.print("LEVEL:");
  display.setCursor(panelX, nextY + 42);  // Reduced from 50
  display.print(level);
  
  // LINES (moved much higher)
  display.setCursor(panelX, nextY + 52);  // Reduced from 62
  display.print("LINES:");
  display.setCursor(panelX, nextY + 62);  // Reduced from 72
  display.print(linesCleared);
  
  // Game over screen
  if (gameOver) {
    display.fillRect(20, 20, 88, 24, SSD1306_BLACK);
    display.drawRect(20, 20, 88, 24, SSD1306_WHITE);
    display.setCursor(32, 25);
    display.print("GAME OVER");
    display.setCursor(15, 40);
    display.print("Score: ");
    display.print(score);
    display.setCursor(15, 50);
    display.print("Level: ");
    display.print(level);
    display.setCursor(25, 60);
    display.print("Tap to restart");
  }
  
  // Start screen
  if (!gameRunning && !gameOver) {
    display.setTextSize(1);
    display.setCursor(40, 15);
    display.print("TETRIS");
    display.setCursor(15, 30);
    display.print("Joystick: Move L/R");
    display.setCursor(15, 40);
    display.print("Touch: Rotate");
    display.setCursor(15, 50);
    display.print("Button: Hard Drop");
    display.setCursor(35, 60);
    display.print("Tap to start");
  }
  
  display.display();
}

// ================= SETUP & LOOP ================= //
void setup() {
  Serial.begin(115200);
  
  Wire.begin(OLED_SDA, OLED_SCL);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED allocation failed!");
    while(1);
  }
  
  pinMode(JOYSTICK_BUTTON, INPUT_PULLUP);
  pinMode(TOUCH_PIN, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);
  
  Serial.println("TETRIS - LEVEL NOW VISIBLE!");
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40, 25);
  display.print("TETRIS");
  display.setCursor(20, 40);
  display.print("Level Now Visible!");
  display.display();
  
  playStartSound();
  delay(2000);
  
  randomSeed(analogRead(0));
  generateNextPiece();
}

void loop() {
  readJoystick();
  checkTouch();
  
  if (gameRunning) {
    updateGame();
  }
  
  drawGame();
  delay(16);
}
