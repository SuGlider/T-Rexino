/*
   Libraries:
   Adafruit APDS-9960
   https://github.com/adafruit/Adafruit_APDS9960
   BLE Mouse modified to work with any Arduino Core
   https://github.com/SuGlider/ESP32-BLE-Mouse

*/

/*
   This application is a DEMO for Maker Faire May, 2025 / Prague
   It will use a ESP32|ESP32-S3|ESP32-C3|ESP32-C6|ESP32-H2 and 
   a proximity sensor APDS-9960/9930

   The proximity will trigger a BLE Mouse event to a connected Android Tablet
   that is running a Dino T-Rex Android game found at
   https://play.google.com/store/apps/details?id=com.deerslab.dinoTREX&hl=en-US

   In order to make it work, after uploading the ESP32 with the current sketch,
   The Dino T-Rex game shall be installed into the Android Tablet.
   Turn on the Tablet BLE and search for "ESP32 Bluetooth Mouse" device
   Select it, connect to it and pair it.
   Wait a few seconds until seeing a message saying that the ESP32 is connected to the Tablet.
   This message shall shown up in the Tablet screen in the BLE scanning window.

   After pairing, make sure that the ESP32 can send Mouse events to the Tablet:
   Move an object in front of the APDS-9960 and a mouse pointer shall show up in the Tablet screen.
   It no mouse pointer is visible, click the "ESP32 Bluetooth Mouse" and pair it again.
   Repeat the procedure until it works by showing a mouse pointer in the Tablet screen.

   Once the ESP32 BLE Mouse is connected and working, start the Dino T-Rex application.
   Tap the proximity sensor to start the game and tap it to make the Dino jump the cactus.
   Tapping the proximity sensor can be done by getting an object close to the APDS-9960 sensor, followed by moving it distant.
   If the object is kept close to the APDS-9960 sensor, it will send a single click.
   It will only click the Mouse again if the object gets far and close again, like "virtual" button press in front of the APDS-9960 sensor.

   LED Indicator:
   ==============
   The sketch will try to connect to the APDS-9960 using I2C pins.
   If it fails, the LED will blink 2 times per second. If the LED is RGB, it will blink RED
   
   Latter the sketch will wait for the Android Tablet to connect using BLE.
   The LED will blink faster (about 5 times per second while waiting the BLE Connection.
   If the LED is RGB, it will blink in BLUE.
   It will stop blinking as soon as the board connect to the Ardroid Tablet.

   While the application is running, the LED will turn on and off whenever it detects that an object is close to he APDS-9960 sensor.
   When the object is detected, the ESP32 boards will send a BLE HID Mouse Click to the Android Tablet.
*/

#include <BleMouse.h>
#include "Adafruit_APDS9960.h"

// If necessary, define different I2C pins here
//#define SDA 25
//#define SCL 26

// the ESP32 SoC pin that the APDS-9960 interrupt is attached to
#define INT_PIN 21

// set your board LED INdicator pin here
// this example uses the built-in LED for easy visualization
#ifdef LED_BUILTIN
const uint8_t LED_PIN = LED_BUILTIN;
#else
const uint8_t LED_PIN = 2;  // Set your LED indicator pin here
#warning "Do not forget to set the LED indicator pin"
#endif

//create the APDS9960 object
Adafruit_APDS9960 apds;
BleMouse bleMouse;

void setup() {
  Serial.begin(115200);

#if defined(SDA) && defined(SCL)
  // set I2C pins
  Wire.begin(SDA, SCL);
#else
  Wire.begin(); // using default pins
#endif

  // configure LED_PIN
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

#ifdef RGB_BUILTIN
  //turn OFF RGB LED
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);  // OFF
#endif

  byte error = 0;
  do {
    Wire.beginTransmission(APDS9960_ADDRESS);
    error = Wire.endTransmission();
    if (error != 0) {
      Serial.printf("APDS-9960 can't be found in I2C bus. Using I2C Address %02x\r\n", APDS9960_ADDRESS);
      Serial.println("Check cables, I2C pins and the APDS-9960 pins");
      for (uint8_t i = 0; i < 4; i++) {
#ifdef RGB_BUILTIN
        // blink RGB LED in RED in case of I2C failure
        rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS * (i & 1), 0, 0);
#else
        digitalWrite(LED_PIN, (i & 1));
#endif
        delay(500);
      }
    }
  } while (error != 0);

  // configure APDS-9960 INTERRUPT PIN
  pinMode(INT_PIN, INPUT_PULLUP);

  if (!apds.begin()) {
    Serial.println("failed to initialize device! Please check your wiring.");
  } else {
    Serial.println("APDS-9960 is initialized!");
  }

  //enable proximity mode
  apds.enableProximity(true);

  //set the interrupt threshold to fire when proximity reading goes above 175
  apds.setProximityInterruptThreshold(0, 10);

  //enable the proximity interrupt
  apds.enableProximityInterrupt();

  // starting BLE HID Mouse
  Serial.println("Starting BLE work!");
  bleMouse.begin();
  while (!bleMouse.isConnected()) {
    Serial.println("Waiting for BLE Mouse to be paired with the Android Tablet");
    for (uint8_t i = 0; i < 10; i++) {
#ifdef RGB_BUILTIN
      // fast blink RGB LED in Blue
      rgbLedWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS * (i & 1));
#else
      digitalWrite(LED_PIN, (i & 1));
#endif
      delay(200);
    }
  }
  Serial.println("BLE Connected.");
  digitalWrite(LED_PIN, LOW);
#ifdef RGB_BUILTIN
  //turn OFF RGB LED
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);  // OFF
#endif
}

bool buttonWasReleased = false;
const uint32_t debounceTime = 50; // milliseconds
uint32_t lastMousePressed = 0;
void loop() {

  // There is a something too close to the sensor
  if (!digitalRead(INT_PIN)) {
    int proximity = apds.readProximity();
    Serial.println(proximity);
    //clear the interrupt
    apds.clearInterrupt();
    // Mouse Click press
    if (buttonWasReleased) {
      // Mouse Click BLE event
      bleMouse.press(MOUSE_LEFT);
      lastMousePressed = millis();
      Serial.println("Mouse Click Pressed...");
      // Blink LED while Mouse is pressed
      digitalWrite(LED_PIN, HIGH);
      //buttonPress = true;
      buttonWasReleased = false;
    }
  } else {
    // Nothing in front of the sensor - turn off LED
    if (buttonWasReleased) {
      digitalWrite(LED_PIN, LOW);
    }
  }
  // Timed Mouse BLE event release
  // Keeping the senor activated will cause Mouse Click repeating in <debounceTime> ms
  if (!buttonWasReleased && millis() > lastMousePressed + debounceTime) {
    bleMouse.release(MOUSE_LEFT);
    Serial.println("Mouse Click Released...");
    buttonWasReleased = true;
  }
  delay(10);
}
