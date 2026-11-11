

// ==========================================
// 1. Pin Configuration (Based on slides)
// ==========================================
const int buttonPin = 2;
const int RLedPin = 9;
const int GLedPin = 10;
const int BLedPin = 11;

// ==========================================
// 2. Button Variables (Short Click / Long Press)
// ==========================================
int buttonState = 0;
bool buttonPressed = false;
unsigned long pressingTime = 0;
const int longPressInterval = 2000; // Hold for 2 seconds to switch modes

// ==========================================
// 3. Color & Mode Variables
// ==========================================
// Selected colors: 0=White, 1=Red, 2=Green, 3=Blue, 4=Yellow[cite: 4]
int lightNum = 0; 
int RLightColor = 0;
int GLightColor = 0;
int BLightColor = 0;

// Current output colors sent to the RGB LED
int RCurrentColor = 0;
int GCurrentColor = 0;
int BCurrentColor = 0;

// Operating modes: 0=Constant, 1=Blink, 2=Breathing, 3=Rainbow (Advanced Bonus)[cite: 6, 8]
int currentMode = 0; 

// ==========================================
// 4. Effect Variables (Using millis() to prevent delay lag)
// ==========================================
// For Blink Mode
unsigned long blinkTimer = 0;
const int blinkInterval = 500;
bool blinkOn = true;

// For Breathing (Fading) Mode
unsigned long fadeTimer = 0;
const int fadeInterval = 10;
const int fadeAmount = 2;
int fadeDirection = 1;

// For Advanced Rainbow Mode
unsigned long rainbowTimer = 0;
int rainbowSpeed = 15; // Rainbow transition speed
int rainbowStep = 0;   // Color position (0 - 767)

// ==========================================
// SETUP
// ==========================================
void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(RLedPin, OUTPUT);
  pinMode(GLedPin, OUTPUT);
  pinMode(BLedPin, OUTPUT);
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  checkButton();      // Continuously check button inputs
  updateLEDColor();   // Calculate colors & effects based on current mode
  setRGBLEDColor(RCurrentColor, GCurrentColor, BCurrentColor); // Output to LED
}

// ==========================================
// Button Detection Function (Short Click vs Long Press)
// ==========================================
void checkButton() {
  buttonState = digitalRead(buttonPin);

  // Button pressed down
  if (buttonState == HIGH && !buttonPressed) {
    pressingTime = millis();
    buttonPressed = true;
  }

  // Button released
  if (buttonState == LOW && buttonPressed) {
    unsigned long currentTime = millis();
    
    if (currentTime - pressingTime < longPressInterval) {
      // --- SHORT CLICK ---
      if (currentMode == 3) {
        changeRainbowSpeed(); // Bonus feature: Short click changes rainbow speed
      } else {
        changeLEDColor();     // Regular mode: Short click changes color
      }
    } else {
      // --- LONG PRESS ---
      changeMode(); // Switch mode
    }
    buttonPressed = false;
  }
}

// ==========================================
// Mode Switching Function (Cycles 0 -> 1 -> 2 -> 3)
// ==========================================
void changeMode() {
  currentMode = currentMode + 1;
  if (currentMode >= 4) { // Total of 4 modes
    currentMode = 0;
  }

  // Reset timers/variables when entering a new mode
  if (currentMode == 1) {
    blinkTimer = millis();
    blinkOn = true;
  } else if (currentMode == 2) {
    RCurrentColor = RLightColor;
    GCurrentColor = GLightColor;
    BCurrentColor = BLightColor;
    fadeDirection = 1;
  } else if (currentMode == 3) {
    rainbowStep = 0;
  }
}

// ==========================================
// Color Changing Function (For regular modes 0, 1, 2)
// ==========================================
void changeLEDColor() {
  lightNum = lightNum + 1;
  if (lightNum >= 5) {
    lightNum = 0;
  }

  // Common Anode values: 0 = Max brightness, 255 = Off[cite: 1, 4]
  if (lightNum == 0) { // White[cite: 4]
    RLightColor = 0; GLightColor = 0; BLightColor = 0;
  } else if (lightNum == 1) { // Red[cite: 4]
    RLightColor = 0; GLightColor = 255; BLightColor = 255;
  } else if (lightNum == 2) { // Green[cite: 4]
    RLightColor = 255; GLightColor = 0; BLightColor = 255;
  } else if (lightNum == 3) { // Blue[cite: 4]
    RLightColor = 255; GLightColor = 255; BLightColor = 0;
  } else if (lightNum == 4) { // Yellow[cite: 4]
    RLightColor = 0; GLightColor = 0; BLightColor = 255;
  }

  RCurrentColor = RLightColor;
  GCurrentColor = GLightColor;
  BCurrentColor = BLightColor;
}

// ==========================================
// Change Rainbow Speed (Bonus feature in Mode 3)
// ==========================================
void changeRainbowSpeed() {
  if (rainbowSpeed == 15) {
    rainbowSpeed = 5;  // Faster
  } else if (rainbowSpeed == 5) {
    rainbowSpeed = 1;  // Very fast (Party mode)
  } else {
    rainbowSpeed = 15; // Smooth & slow back
  }
}

// ==========================================
// Update Colors & Effects based on Mode
// ==========================================
void updateLEDColor() {
  unsigned long currentTime = millis();

  // Mode 0: Constant Light Mode[cite: 5, 6]
  if (currentMode == 0) {
    RCurrentColor = RLightColor;
    GCurrentColor = GLightColor;
    BCurrentColor = BLightColor;
  }
  // Mode 1: Blink Mode[cite: 5, 6]
  else if (currentMode == 1) {
    if (currentTime - blinkTimer > blinkInterval) {
      blinkOn = !blinkOn;
      blinkTimer = currentTime;
    }
    if (blinkOn) {
      RCurrentColor = RLightColor;
      GCurrentColor = GLightColor;
      BCurrentColor = BLightColor;
    } else {
      RCurrentColor = 255; GLightColor = 255; BLightColor = 255; // Turn off
    }
  }
  // Mode 2: Breathing Light Mode[cite: 5, 6]
  else if (currentMode == 2) {
    if (currentTime - fadeTimer > fadeInterval) {
      fadeTimer = currentTime;
      bool negativeDir = false;

      if (RLightColor == 0) {
        RCurrentColor = RCurrentColor + fadeDirection * fadeAmount;
        if (RCurrentColor > 255 || RCurrentColor < 0) negativeDir = true;
        RCurrentColor = constrain(RCurrentColor, 0, 255);
      }
      if (GLightColor == 0) {
        GCurrentColor = GCurrentColor + fadeDirection * fadeAmount;
        if (GCurrentColor > 255 || GCurrentColor < 0) negativeDir = true;
        GCurrentColor = constrain(GCurrentColor, 0, 255);
      }
      if (BLightColor == 0) {
        BCurrentColor = BCurrentColor + fadeDirection * fadeAmount;
        if (BCurrentColor > 255 || BCurrentColor < 0) negativeDir = true;
        BCurrentColor = constrain(BCurrentColor, 0, 255);
      }

      if (negativeDir) {
        fadeDirection = -fadeDirection;
      }
    }
  }
  // Mode 3: Advanced Rainbow Mode[cite: 7, 8]
  else if (currentMode == 3) {
    if (currentTime - rainbowTimer > rainbowSpeed) {
      rainbowTimer = currentTime;
      
      int r, g, b;
      if (rainbowStep < 256) {
        r = 255 - rainbowStep; g = rainbowStep; b = 0;
      } else if (rainbowStep < 512) {
        r = 0; g = 255 - (rainbowStep - 256); b = rainbowStep - 256;
      } else {
        r = rainbowStep - 512; g = 0; b = 255 - (rainbowStep - 512);
      }

      RCurrentColor = 255 - r;
      GCurrentColor = 255 - g;
      BCurrentColor = 255 - b;

      rainbowStep++;
      if (rainbowStep >= 768) rainbowStep = 0;
    }
  }
}

// ==========================================
// Write PWM values to LED pins
// ==========================================
void setRGBLEDColor(int r, int g, int b) {
  analogWrite(RLedPin, r);
  analogWrite(GLedPin, g);
  analogWrite(BLedPin, b);
}

