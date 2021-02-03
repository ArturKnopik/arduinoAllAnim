#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/atomic.h>

#define WIDTH 16
#define HEIGHT 16
#define NUM_LEDS 256

#define PIN 13
Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

#define BUTTON 2
byte selectedEffect = 1;

// The following addresses a problem in version 1.0.5 and earlier of the Arduino
// IDE that prevents randomSeed from working properly.
//        https://github.com/arduino/Arduino/issues/575
#define randomSeed(s) srandom(s)

volatile uint32_t
    seed; // These two variables can be reused in your program after the
volatile int8_t nrot; // function CreateTrulyRandomSeed()executes in the setup()
                      // function.
void CreateTrulyRandomSeed() {
  seed = 0;
  nrot = 32; // Must be at least 4, but more increased the uniformity of the
             // produced seeds entropy.

  // The following five lines of code turn on the watch dog timer interrupt to
  // create the seed value
  cli();
  MCUSR = 0;
  _WD_CONTROL_REG |= (1 << _WD_CHANGE_BIT) | (1 << WDE);
  _WD_CONTROL_REG = (1 << WDIE);
  sei();

  while (nrot > 0)
    ; // wait here until seed is created

  // The following five lines turn off the watch dog timer interrupt
  cli();
  MCUSR = 0;
  _WD_CONTROL_REG |= (1 << _WD_CHANGE_BIT) | (0 << WDE);
  _WD_CONTROL_REG = (0 << WDIE);
  sei();
}

ISR(WDT_vect) {
  nrot--;
  seed = seed << 8;
  seed = seed ^ TCNT1L;
}

void setup() {
  CreateTrulyRandomSeed();
  randomSeed(seed);
   changeEffect();
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
                // digitalWrite (BUTTON, HIGH);  // internal pull-up resistor
  // attachInterrupt (digitalPinToInterrupt (BUTTON), changeEffect, CHANGE); //
  // pressed
}

// *** REPLACE FROM HERE ***
void loop() {
  switch (selectedEffect) {

  case 0: {
    RGBLoop();
    break;
  }

  case 1: {
    // FadeInOut - Color (red, green. blue)
    FadeInOut(0xff, 0x00, 0x00); // red
    FadeInOut(0xff, 0xff, 0xff); // white
    FadeInOut(0x00, 0x00, 0xff); // blue
    break;
  }

  case 2: {
    // Strobe - Color (red, green, blue), number of flashes, flash speed, end
    // pause
    Strobe(0xff, 0xff, 0xff, 10, 50, 1000);
    break;
  }

  case 3: {
    // HalloweenEyes - Color (red, green, blue), Size of eye, space between
    // eyes, fade (true/false), steps, fade delay, end pause
    HalloweenEyes(255, 0x00, 0x00, 1, 4, true, random(5, 50), random(50, 150),
                  random(1000, 10000));
    HalloweenEyes(0, 255, 0x00, 1, 4, true, random(5, 50), random(50, 150),
                  random(1000, 10000));
    HalloweenEyes(0, 0x00, 255, 1, 4, true, random(5, 50), random(50, 150),
                  random(1000, 10000));
    HalloweenEyes(random(255, 255), random(255, 255), random(255, 255), 1, 4, true, random(5, 50), random(50, 150),
                  random(1000, 10000));
    break;
  }

  case 4: {
    // CylonBounce - Color (red, green, blue), eye size, speed delay, end pause
    CylonBounce(0xff, 0x00, 0x00, 10, 1, 50);
    break;
  }

  case 5: {
    // NewKITT - Color (red, green, blue), eye size, speed delay, end pause
    NewKITT(0xff, 0x00, 0x00, 10, 1, 50);
    break;
  }

  case 6: {
    // Twinkle - Color (red, green, blue), count, speed delay, only one twinkle
    // (true/false)
    Twinkle(0xff, 0x00, 0x00, 50, 300, false);
    break;
  }

  case 7: {
    // TwinkleRandom - twinkle count, speed delay, only one (true/false)
    TwinkleRandom(100, 200, false);
    break;
  }

  case 8: {
    // Sparkle - Color (red, green, blue), speed delay
    Sparkle(0xff, 0xff, 0xff, 0);
    break;
  }

  case 9: {
    // SnowSparkle - Color (red, green, blue), sparkle delay, speed delay
    SnowSparkle(0x10, 0x10, 0x10, 20, random(500, 3000));
    break;
  }

  case 10: {
    // Running Lights - Color (red, green, blue), wave dealy
    RunningLights(0xff, 0x00, 0x00, 50); // red
    RunningLights(0xff, 0xff, 0xff, 50); // white
    RunningLights(0x00, 0x00, 0xff, 50); // blue
    break;
  }

  case 11: {
    // colorWipe - Color (red, green, blue), speed delay
    colorWipe(random(0, 255), random(0, 255), random(0, 255), 5);
    colorWipe(random(0, 255), random(0, 255), random(0, 255), 5);
    colorWipe(random(0, 255), random(0, 255), random(0, 255), 5);
    colorWipe(0x00, 0x00, 0x00, 5);
    
    break;
  }

  case 12: {
    // rainbowCycle - speed delay
    rainbowCycle(10);
    break;
  }

  case 13: {
    // theatherChase - Color (red, green, blue), speed delay
    theaterChase(random(0, 255), random(0, 255), random(0, 255), 50);
    theaterChase(random(0, 255), random(0, 255), random(0, 255), 50);
    theaterChase(random(0, 255), random(0, 255), random(0, 255), 50);
    theaterChase(random(0, 255), random(0, 255), random(0, 255), 50);
    theaterChase(random(0, 255), random(0, 255), random(0, 255), 50);
    theaterChase(random(0, 255), random(0, 255), random(0, 255), 50);
    break;
  }

  case 14: {
    // theaterChaseRainbow - Speed delay
    theaterChaseRainbow(40);
    break;
  }

  case 15: {
    // Fire - Cooling rate, Sparking rate, speed delay
    for(int i=0;i<600;i++){
    FireLine(40, 200, 10);
    }
    break;
  }

    // simple bouncingBalls not included, since BouncingColoredBalls can perform
    // this as well as shown below BouncingColoredBalls - Number of balls, color
    // (red, green, blue) array, continuous CAUTION: If set to continuous then
    // this effect will never stop!!!

  case 16: {
    // mimic BouncingBalls
    byte onecolor[1][3] = {{random(0, 255), random(0, 255), random(0, 255)}};
    BouncingColoredBalls(15, onecolor, false);
    break;
  }

  case 17: {
    // multiple colored balls
   byte colors[3][3] = {
        {0xff, 0x00, 0x00}, {0xff, 0xff, 0xff}, {0x00, 0x00, 0xff}};
    BouncingColoredBalls(3, colors, false);
    break;
  }

  case 18: {
    // meteorRain - Color (red, green, blue), meteor size, trail decay, random
    // trail decay (true/false), speed delay
    meteorRain(0xff, 0xff, 0xff, 10, 64, true, 30);
    break;
  }
  case 19: {
    // meteorRain - Color (red, green, blue), meteor size, trail decay, random
    // trail decay (true/false), speed delay
    RandomColorFlicker();
    break;
  }
  case 20: {
    // meteorRain - Color (red, green, blue), meteor size, trail decay, random
    // trail decay (true/false), speed delay
    //for(int i=0;i<600;i++){
   // FireLine(65, 100, 10);
   // }
    break;
  }
  }
  changeEffect();
}

void changeEffect() {

/*
  if(selectedEffect==19)
  {
    selectedEffect=1;
  }
  else
  {
    selectedEffect++;
  }
  */
  selectedEffect = random(1, 19);
  // selectedEffect=19;
}

// *************************
// ** LEDEffect Functions **
// *************************

void RGBLoop() {
  for (int j = 0; j < 3; j++) {
    // Fade IN
    for (int k = 0; k < 256; k++) {
      switch (j) {
      case 0:
        setAll(k, 0, 0);
        break;
      case 1:
        setAll(0, k, 0);
        break;
      case 2:
        setAll(0, 0, k);
        break;
      }
      showStrip();
      delay(3);
    }
    // Fade OUT
    for (int k = 255; k >= 0; k--) {
      switch (j) {
      case 0:
        setAll(k, 0, 0);
        break;
      case 1:
        setAll(0, k, 0);
        break;
      case 2:
        setAll(0, 0, k);
        break;
      }
      showStrip();
      delay(3);
    }
  }
}

void FadeInOut(byte red, byte green, byte blue) {
  float r, g, b;

  for (int k = 0; k < 256; k = k + 1) {
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }

  for (int k = 255; k >= 0; k = k - 2) {
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay,
            int EndPause) {
  for (int j = 0; j < StrobeCount; j++) {
    setAll(red, green, blue);
    showStrip();
    delay(FlashDelay);
    setAll(0, 0, 0);
    showStrip();
    delay(FlashDelay);
  }

  delay(EndPause);
}

void HalloweenEyes(byte red, byte green, byte blue, int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay, int EndPause) {
  randomSeed(analogRead(0));

  int i;
  int StartPoint = random(0, NUM_LEDS - (2 * EyeWidth) - EyeSpace);
  int Start2ndEye = StartPoint + EyeWidth + EyeSpace;

  for (i = 0; i < EyeWidth; i++) {
    setPixel(StartPoint + i, red, green, blue);
    setPixel(Start2ndEye + i, red, green, blue);
  }

  showStrip();

  if (Fade == true) {
    float r, g, b;

    for (int j = Steps; j >= 0; j--) {
      r = j * (red / Steps);
      g = j * (green / Steps);
      b = j * (blue / Steps);

      for (i = 0; i < EyeWidth; i++) {
        setPixel(StartPoint + i, r, g, b);
        setPixel(Start2ndEye + i, r, g, b);
      }

      showStrip();
      delay(FadeDelay);
    }
  }

  setAll(0, 0, 0); // Set all black

  delay(EndPause);
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay,
                 int ReturnDelay) {

  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);
}

void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay,
             int ReturnDelay) {
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

// used by NewKITT
void CenterToOutside(byte red, byte green, byte blue, int EyeSize,
                     int SpeedDelay, int ReturnDelay) {
  for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS - i - j, red, green, blue);
    }
    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void OutsideToCenter(byte red, byte green, byte blue, int EyeSize,
                     int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS - i - j, red, green, blue);
    }
    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay,
                 int ReturnDelay) {
  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay,
                 int ReturnDelay) {
  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay,
             boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(NUM_LEDS), red, green, blue);
    showStrip();
    delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(SpeedDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
    showStrip();
    delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(SpeedDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel, 0, 0, 0);
}

void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay,
                 int SpeedDelay) {
  setAll(red, green, blue);

  int Pixel = random(NUM_LEDS);
  setPixel(Pixel, 0xff, 0xff, 0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position = 0;
  WaveDelay=0;
  for (int i = 0; i < NUM_LEDS/3; i++) {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < NUM_LEDS; i++) {
      // sine wave, 3 offset waves make a rainbow!
      // float level = sin(i+Position) * 127 + 128;
      // setPixel(i,level,0,0);
      // float level = sin(i+Position) * 127 + 128;
      setPixel(i, ((sin(i + Position) * 127 + 128) / 255) * red,
               ((sin(i + Position) * 127 + 128) / 255) * green,
               ((sin(i + Position) * 127 + 128) / 255) * blue);
    }

    showStrip();
    delay(WaveDelay);
  }
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
    showStrip();
    delay(SpeedDelay);
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < NUM_LEDS; i++) {
      c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c + 1), *(c + 2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

// used by rainbowCycle and theaterChaseRainbow
byte *Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j = 0; j < 10; j++) { // do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        setPixel(i + q, red, green, blue); // turn every third pixel on
      }
      showStrip();

      delay(SpeedDelay);

      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        setPixel(i + q, 0, 0, 0); // turn every third pixel off
      }
    }
  }
}

void theaterChaseRainbow(int SpeedDelay) {
  byte *c;

  for (int j = 0; j < 256; j++) { // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        c = Wheel((i + j) % 255);
        setPixel(i + q, *c, *(c + 1), *(c + 2)); // turn every third pixel on
      }
      showStrip();

      delay(SpeedDelay);

      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        setPixel(i + q, 0, 0, 0); // turn every third pixel off
      }
    }
  }
}

void FireLine(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if (random(255) < Sparking) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    // heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for (int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j]);
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor(int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2;              // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if (t192 > 0x80) { // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if (t192 > 0x40) { // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else { // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous) {
  float Gravity = -3.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt(-2 * Gravity * StartHeight);
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int Position[BallCount];
  long ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  boolean ballBouncing[BallCount];
  boolean ballsStillBouncing = true;

  for (int i = 0; i < BallCount; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
    ballBouncing[i] = true;
  }

  while (ballsStillBouncing) {
    for (int i = 0; i < BallCount; i++) {
      TimeSinceLastBounce[i] = millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce[i] / 1000, 2.0) +
                  ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if (Height[i] < 0) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if (ImpactVelocity[i] < 0.01) {
          if (continuous) {
            ImpactVelocity[i] = ImpactVelocityStart;
          } else {
            ballBouncing[i] = false;
          }
        }
      }
      Position[i] = round(Height[i] * (NUM_LEDS - 1) / StartHeight);
    }

    ballsStillBouncing = false; // assume no balls bouncing
    for (int i = 0; i < BallCount; i++) {
      setPixel(Position[i], colors[i][0], colors[i][1], colors[i][2]);
      if (ballBouncing[i]) {
        ballsStillBouncing = true;
      }
    }

    showStrip();
    setAll(0, 0, 0);
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize,
                byte meteorTrailDecay, boolean meteorRandomDecay,
                int SpeedDelay) {
  setAll(0, 0, 0);

  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {

    // fade brightness all LEDs one step
    for (int j = 0; j < NUM_LEDS; j++) {
      if ((!meteorRandomDecay) || (random(10) > 5)) {
        fadeToBlack(j, meteorTrailDecay);
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ((i - j < NUM_LEDS) && (i - j >= 0)) {
        setPixel(i - j, red, green, blue);
      }
    }

    showStrip();
    delay(SpeedDelay);
  }
}

// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[ledNo].fadeToBlackBy(fadeValue);
#endif
}

void RandomColorFlicker() {
  unsigned short randNumR = random(0, 200);
  unsigned short randNumG = random(0, 200);
  unsigned short randNumB = random(0, 200);
  for (unsigned short i = 0; i < 150; i++) {

    for (unsigned short i = 0; i < 256; i++) {
      short randNumRDecay = random(-50, 50);
      short randNumGDecay = random(-50, 50);
      short randNumBDecay = random(-50, 50);
      if (randNumRDecay + randNumR > 255) {
        randNumR = 255;
      }
      if (randNumGDecay + randNumG > 255) {
        randNumG = 255;
      }
      if (randNumBDecay + randNumB > 255) {
        randNumB = 255;
      }

      if (randNumRDecay + randNumR < 0) {
        randNumR = 0;
      }
      if (randNumGDecay + randNumG < 0) {
        randNumG = 0;
      }
      if (randNumBDecay + randNumB < 0) {
        randNumB = 0;
      }

      strip.setPixelColor(i, randNumR + randNumRDecay, randNumG + randNumGDecay,
                          randNumB + randNumBDecay);
    }
    showStrip();
  }
}

void FireGen(int Cooling, int Sparking, int SpeedDelay) {
    static byte heat[NUM_LEDS];
  int cooldown;
for(int rep=0; rep<255;rep++)
{
  // Step 1.  Cool down every cell a little
  for (int i = 0+rep; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if (random(255) < Sparking) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    // heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for (int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j]);
  }

  showStrip();
  delay(SpeedDelay);
}
}

// *** REPLACE TO HERE ***

// ***************************************
// ** FastLed/NeoPixel Common Functions **
// ***************************************

// Apply LED color changes
void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
