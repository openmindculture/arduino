#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#define DELAY_STATE_CHANGE 500
#define PIN_MOISTURE A1
#define PIN_STATE_TRIGGER 3
#define MOISTURE_THRESHOLD 500
#define PIN_NEOPIXEL 2
#define NEOPIXEL_LED_COUNT 5

int valStateTrigger = 0;
int valMoisture = 0;
boolean stateMode = false;
boolean gotWater = false;
boolean saidHello = false;
boolean saidGoodbye = false;
boolean needsWater = false;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(NEOPIXEL_LED_COUNT, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void setup()
{
    // Pin setup
    pinMode(PIN_STATE_TRIGGER, INPUT_PULLUP);
    pinMode(PIN_MOISTURE, INPUT);
    pinMode(PIN_NEOPIXEL, OUTPUT);

    // Neopixel setup
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.clear();            // Turn OFF all pixels ASAP
    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

    // Serial setup
    Serial.begin(9600);
}

// ----------------------------------------------------------------------------
// NEOPIXEL SEQUENCES
//

void showGoodbye()
{
    strip.clear();

    // show static green
    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, 0, 100, 0);
    }

    strip.show();

    // fade from green to off
    for (uint32_t i = strip.numPixels(); i > 0; i--)
    {
        for (int j = 100; j > 0; j--)
        {
            strip.setPixelColor(i - 1, 0, j, 0);
            strip.show();
            delay(10);
        }
    }

    strip.clear();
}

void showIsAwake()
{
    // show static green
    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, 10, 50, 0);
    }

    strip.show();
}

void showNeedsWatering()
{
    // // fade from greenish to reddish color
    // // decrease green color
    // for (uint32_t i = strip.numPixels() - 1; i > -1; i--)
    // {
    //     for (int j = 100; j > 19; j--)
    //     {
    //         strip.setPixelColor(i, 25, j, 0);
    //         strip.show();
    //         delay(2 * i + i);
    //     }
    // }
    // // increase red color
    // for (uint32_t i = 0; i < strip.numPixels(); i++)
    // {
    //     for (int j = 25; j < 81; j++)
    //     {
    //         strip.setPixelColor(i, j, 20, 0);
    //         strip.show();
    //         delay(10);
    //     }
    // }

    // show static red
    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, 100, 0, 0);
    }

    strip.show();
}

void showWink()
{
    // "wink" by turning off the light for a short time twice
    // remember current color for each pixel
    uint32_t colors[strip.numPixels()];

    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        colors[i] = strip.getPixelColor(i);
    }

    // wink twice
    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, 0, 0, 0);
    }

    strip.show();

    delay(60);

    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, colors[i]);
    }

    strip.show();

    delay(120);

    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, 0, 0, 0);
    }

    strip.show();
    delay(60);

    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, colors[i]);
    }

    strip.show();
}

void showGreeting()
{
    strip.clear();

    // fade from off to green
    for (uint32_t i = 0; i < strip.numPixels(); i++)
    { // For each pixel in strip...
        for (int j = 0; j < 101; j++)
        {
            strip.setPixelColor(i, 25, j, 0);
            strip.show();
            delay(10);
        }
        for (int j = 0; j < 26; j++)
        {
            strip.setPixelColor(i, j, 100, 0);
            strip.show();
        }
    }

    // wink twice
    showWink();
}

// Some functions of our own for creating animated effects -----------------
// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait)
{
    strip.clear();

    // Hue of first pixel runs 5 complete loops through the color wheel.
    // Color wheel has a range of 65536 but it's OK if we roll over, so
    // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
    // means we'll make 5*65536/256 = 1280 passes through this outer loop:
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
    {
        for (uint32_t i = 0; i < strip.numPixels(); i++)
        { // For each pixel in strip...
            // Offset pixel hue by an amount to make one full revolution of the
            // color wheel (range of 65536) along the length of the strip
            // (strip.numPixels() steps):
            uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
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

void showThanksForWatering()
{
    strip.clear();

    // perform awesome animations...
    rainbow(10); // rainbow demo already looks smooth and elegant
    showWink();
    delay(1000);
    showWink();
    delay(1000);
    rainbow(5);

    // turn off
    for (uint32_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, 0, 0, 0);
    }

    strip.show();
}

// ----------------------------------------------------------------------------
// CONTROLLER
//

void signalHello()
{
    Serial.println("Signal hello");
    // 1. play hello sequence
    showGreeting();
    delay(2000);
    Serial.println("Finished Signal hello");
}

void signalGoodbye()
{
    Serial.println("Signal goodbye");
    // 1. play goodbye sequence
    showGoodbye();
    delay(2000);
    Serial.println("Finished Signal goodbye");
}

void signalForWatering()
{
    Serial.println("Signal for watering");
    // play LED sequence for waiting for water
    // should have no delay!
    showNeedsWatering();
}

void thanksForWatering()
{
    Serial.println("Signal got some water");
    // play LED sequence for gotWater
    // reset state for watering
    showThanksForWatering();
    delay(2000);
    Serial.println("Finished Signal got some water");
}

void checkForPause()
{
}

void checkForDrinking()
{
}

void checkForWatering()
{
    // moisture is low (= value above 500):
    // -> signal for watering
    if (valMoisture > MOISTURE_THRESHOLD)
    {
        signalForWatering();
        gotWater = false;
        needsWater = true;
    }

    // moisture is high (= value is below or even to 500):
    // -> plant got some water once
    if (valMoisture <= MOISTURE_THRESHOLD && gotWater == false)
    {
        thanksForWatering();
        gotWater = true;
        needsWater = false;
    }
}

void runWhenOn()
{
    checkForPause();
    checkForDrinking();

    if (needsWater == false) {
        showIsAwake();
    }
}

void runAlways()
{
    checkForWatering();
}

void loop()
{
    valStateTrigger = digitalRead(PIN_STATE_TRIGGER);
    valMoisture = analogRead(PIN_MOISTURE);
    // Serial.print("Moisture: ");
    // Serial.print(valMoisture);
    // Serial.print(", State: ");
    // Serial.println(valStateTrigger);

    if (stateMode == false && valStateTrigger == LOW)
    {
        Serial.println("Set mode to 'on'");
        stateMode = true;
        delay(DELAY_STATE_CHANGE);
    }
    else if (stateMode == true && valStateTrigger == LOW)
    {
        Serial.println("Set mode to 'off'");
        stateMode = false;
        delay(DELAY_STATE_CHANGE);
    }

    if (stateMode == true && saidHello == false)
    {
        saidGoodbye = false;
        saidHello = true;
        signalHello();
    }

    if (stateMode == false && saidGoodbye == false)
    {
        saidHello = false;
        saidGoodbye = true;
        signalGoodbye();
    }

    if (stateMode == true)
    {
        runWhenOn();
    }

    runAlways();
}
