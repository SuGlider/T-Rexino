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

### LED Indicator
