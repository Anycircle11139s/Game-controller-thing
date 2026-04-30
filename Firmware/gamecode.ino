#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>

// Hardware Definitions from Schematic
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define BUZZER_PIN 1        // GPIO1 / D0 [cite: 20]
#define JUMP_BUTTON 9       // GPIO9 / D10 (Rotary Switch) 

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
Adafruit_ADS1115 ads; 

// Physics Constants
float playerX = 10, playerY = 30;
float velocityY = 0;
const float gravity = 0.8;
bool isGrounded = false;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(JUMP_BUTTON, INPUT_PULLUP); // Active low per schematic 

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while(1);
  ads.begin();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // 1. Read Inputs via ADS1115 
  int16_t joyX = ads.readADC_SingleEnded(0); // Joystick X 
  int16_t sliderVal = ads.readADC_SingleEnded(2); // Slider [cite: 118]
  
  // 2. Horizontal Movement
  if (joyX > 20000) playerX += 2;
  if (joyX < 10000) playerX -= 2;

  // 3. Jump Logic (Using Rotary Switch) [cite: 12]
  if (digitalRead(JUMP_BUTTON) == LOW && isGrounded) {
    velocityY = -6.0; 
    isGrounded = false;
    tone(BUZZER_PIN, 440, 50); // Jump sound 
  }

  // 4. Physics
  velocityY += gravity;
  playerY += velocityY;

  // Simple Floor Collision
  if (playerY >= 50) {
    playerY = 50;
    velocityY = 0;
    isGrounded = true;
  }

  // 5. Draw Game
  display.clearDisplay();
  
  // Draw Player
  display.fillRect(playerX, playerY, 8, 8, SSD1306_WHITE);
  
  // Draw "Ground"
  display.drawFastHLine(0, 58, 128, SSD1306_WHITE);
  
  // UI based on Slider [cite: 71]
  display.setCursor(0,0);
  display.print("Power: "); 
  display.print(map(sliderVal, 0, 26000, 0, 100));

  display.display();
  delay(20);
}
