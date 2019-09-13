#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN   2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 5

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
 // Lifecycle demo
 showGreeting();
 delay(100);

 showIsAwake();
 delay(3000);

 showNeedsWatering();
 delay(3000);

 showThanksForWatering();
 delay(2000);

 showGoodbye();
 delay(2000);
}

void showGreeting() {
  // fade from off to green
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    for (int j = 0; j <101; j++) {
      strip.setPixelColor(i, 25, j, 0);
      strip.show();
      delay(10);
    }
    for (int j = 0; j <26; j++) {
      strip.setPixelColor(i, j, 100, 0);
      strip.show();
    }
  }
  // wink twice
  showWink();
}
void showGoodbye() {
  // fade from green to off
  for (int i = strip.numPixels() -1; i > -1 ; i--) {
    for (int j = 100; j > -1; j--) {
      strip.setPixelColor(i, 0, j, 0);
      strip.show();
      delay(10);
    }
  }
  strip.clear();
}
void showIsAwake() {
  // show static green
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 25, 100, 0);
  }
  strip.show();
}
void showNeedsWatering() {
  // fade from greenish to reddish color
  // decrease green color
  for (int i = strip.numPixels() -1; i > -1 ; i--) {
    for (int j = 100; j > 19; j--) {
      strip.setPixelColor(i, 25, j, 0);
      strip.show();
      delay(2*i + i);
    }
  }
  // increase red color
  for (int i = 0; i < strip.numPixels(); i++) {
    for (int j = 25; j < 81; j++) {
      strip.setPixelColor(i, j, 20, 0);
      strip.show();
      delay(10);
    }
  }
}
void showThanksForWatering() {
  // perform awesome animations...
  rainbow(10); // rainbow demo already looks smooth and elegant
  showWink();
  delay(2000);
  showWink();
  delay(3000);
  rainbow(5);
  // turn off
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
  // return to green awake display
  showIsAwake();
}

void showWink() {
  // "wink" by turning off the light for a short time twice
  // remember current color for each pixel
  uint32_t colors[strip.numPixels()];
  for (int i = 0; i < strip.numPixels(); i++) {
    colors[i] = strip.getPixelColor(i);
  }
  // wink twice
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
  delay(60);

  for (int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, colors[i]);
  }
  strip.show();
  delay(120);

  for (int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
  delay(60);

  for (int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, colors[i]);
  }
  strip.show();
}

// Some functions of our own for creating animated effects -----------------
// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
