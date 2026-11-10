#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C  // Standard 0.96" OLED I2C address

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Hardware Pins
const int BUZZER_PIN = 8;

// Timing Variables
unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 500; // Check screen status every 500ms for faster response
int currentTuneStep = 1;                 // Tracks which of the 3 background tunes to play next

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  
  Wire.begin();
  Wire.setWireTimeout(3000, true); // Prevent permanent I2C freezing
  
  initDisplay("SYSTEM START");
}

void loop() {
  // 1. CHECK OLED CONNECTION
  if (millis() - lastCheckTime >= checkInterval) {
    lastCheckTime = millis();
    
    Wire.beginTransmission(OLED_ADDRESS);
    byte error = Wire.endTransmission();
    
    // IF THE SCREEN DISCONNECTS
    if (error != 0) {
      // Play a continuous deep warning tone until it is plugged back in
      while (error != 0) {
        tone(BUZZER_PIN, 150, 400); // Low, deep warning beep (150 Hz)
        delay(600);
        
        // Re-check if it's plugged back in yet
        Wire.beginTransmission(OLED_ADDRESS);
        error = Wire.endTransmission();
      }
      
      // IF WE GET HERE, IT MEANS THE SCREEN WAS JUST PLUGGED BACK IN!
      tone(BUZZER_PIN, 1500, 150); delay(150); // High-pitched success chirp
      tone(BUZZER_PIN, 2000, 200); delay(250); 
      noTone(BUZZER_PIN);
      
      initDisplay("RECOVERED!");
    }
  }

  // 2. REPEATING 3-TUNE SEQUENCE (Normal Operation)
  playBackgroundSequence();

  // 3. REFRESH DISPLAY STATUS
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("Status: Connected");
  display.setCursor(0, 30);
  display.print("Buzzer: Playing Loop");
  display.setCursor(0, 50);
  display.print("Disconnect to test");
  display.display();
}

// Function to handle full screen initialization
void initDisplay(String statusMessage) {
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.print(statusMessage);
  display.display();
  delay(800); 
}

// Rotates through Tune 1, Tune 2, and Tune 3 continuously
void playBackgroundSequence() {
  if (currentTuneStep == 1) {
    // Tune 1: Arpeggio Chime
    tone(BUZZER_PIN, 440, 100); delay(120); // A4
    tone(BUZZER_PIN, 554, 100); delay(120); // C#5
    tone(BUZZER_PIN, 659, 100); delay(200); // E5
    currentTuneStep = 2;                    // Move to next tune
  } 
  else if (currentTuneStep == 2) {
    // Tune 2: Alternating Beeps
    tone(BUZZER_PIN, 587, 150); delay(200); // D5
    tone(BUZZER_PIN, 494, 150); delay(200); // B4
    currentTuneStep = 3;
  } 
  else if (currentTuneStep == 3) {
    // Tune 3: Rapid Cyber Chirp
    tone(BUZZER_PIN, 880, 50); delay(70);   // A5
    tone(BUZZER_PIN, 988, 50); delay(70);   // B5
    tone(BUZZER_PIN, 1047, 50); delay(300); // C6
    currentTuneStep = 1;                    // Loop back to Tune 1
  }
  noTone(BUZZER_PIN);
  delay(400); // Brief pause between the distinct tunes
}
