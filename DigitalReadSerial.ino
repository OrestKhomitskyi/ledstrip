#include <SoftwareSerial.h>
#include <avr/wdt.h>


#define NUM_LEDS 94
#include <FastLED.h>
#define PIN 7
CRGB leds[NUM_LEDS];
byte counter = 0;
int ledsX[NUM_LEDS][3];
SoftwareSerial mySerial(10, 11); // RX, TX
bool spd_st = false, g = false, pwr = true;
int spd = 10, colour[3], br = 255;
char cur_mode = 'a';
String bmInput = "";


void setup() {
  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  pinMode(PIN, OUTPUT);

  colour[0] = 209;
  colour[1] = 125;
  colour[2] = 44;
  Serial.begin(9600);
//  mySerial.begin(9600);

}

void setpower() {
  pwr = !pwr;
  g = !g;
  if (!pwr) {
    for (int i = 0; i < NUM_LEDS; i++ ) {
      leds[i] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }
}



void setcolour() {
  colour[0] = bmInput.substring(2, 5).toInt();
  colour[1] = bmInput.substring(10, 13).toInt();
  colour[2] =  bmInput.substring(6, 9).toInt();
}


void setBrightness(int x) {
  br = x;
  FastLED.setBrightness(br);
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  // Bug in FASTLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}


void executeCommand() {
  switch (bmInput[0]) {
    case '3': {
        setpower(); break;
      }
    case '4': {
        setcolour(); break;
    }
    case '7': {
        setBrightness(bmInput.substring(2, 5).toInt()); break;
    }
  }

  if (bmInput[0] == '8') cur_mode = bmInput[2];

}

void loop() {
  //CylonBounce(colour[0], colour[1], colour[2], 4, 10, 50);
  //colorWipeCombined(colour[0], colour[1], colour[2], 15);
  //meteorRain(colour[0], colour[1], colour[2], 5, 48, true, 3);
  rainbowCycle(15);
}

void runMode() {
  setAll(0, 0, 0);
  switch (cur_mode) {
    case '0': CylonBounce(colour[0], colour[1], colour[2], 4, 10, 50); break;
    case '1': colorWipeCombined(colour[0], colour[1], colour[2], 15); break;
    case '2': rainbowCycle(15); break;
    case '3': meteorRain(colour[0], colour[1], colour[2], 5, 48, true, 3); break;
    case '4': FadeInOutCombined(); break;
    case '5': singleColour(); break;
    case '6': basicMode(); break;
    default: basicMode();
  }
}

void basicMode() {

  while (true) {
    static uint8_t hue = 0;
    FastLED.showColor(CHSV(hue++, 255, 255));

    delay(25);
    if (checkInput()) break;
  }
}

void singleColour() {
  FastLED.showColor(CRGB(colour[0], colour[1], colour[2]));
  while (true) {
    if (checkInput()) break;
    delay(10);
  }
}

bool checkInput() {
  delay(10);
  if (mySerial.available() > 0) {
    bmInput = mySerial.readStringUntil('\n');
    Serial.println("Bluetooth input: " + bmInput);
    executeCommand();
    return true;
  }
  return false;

}


void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void showStrip() {
  FastLED.show();
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  while (true) {
    for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
      setAll(0, 0, 0);
      setPixel(i, red / 10, green / 10, blue / 10);
      for (int j = 1; j <= EyeSize; j++) {
        setPixel(i + j, red, green, blue);
      }
      setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
      showStrip();
      delay(SpeedDelay);

      if (checkInput()) return;
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
      if (checkInput()) return;
    }

    delay(ReturnDelay);

  }
}


void colorWipeCombined(byte red, byte green, byte blue, int SpeedDelay) {
  while (true) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      setPixel(i, red, green, blue);
      showStrip();
      delay(SpeedDelay);
      if (checkInput()) return;
    }

    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      setPixel(i, 0x00, 0x00, 0x00);
      showStrip();
      delay(SpeedDelay);
      if (checkInput()) return;
    }
  }
}

void rainbowCycle(int SpeedDelay) {
while (true) {
    byte *c;
    uint16_t i, j;

    for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
      for (i = 0; i < NUM_LEDS; i++) {
        c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
        setPixel(i, *c, *(c + 1), *(c + 2));
      }
      if (checkInput()) return;
      showStrip();
      delay(SpeedDelay);
    }
  }
}

byte * Wheel(byte WheelPos) {
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

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  while (true) {
    setAll(0, 0, 0);

    for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {

      // fade brightness all LEDs one step
      for (int j = 0; j < NUM_LEDS; j++) {
        if ( (!meteorRandomDecay) || (random(10) > 5) ) {
          fadeToBlack(j, meteorTrailDecay );
        }
      }
      if (checkInput()) return;
      // draw meteor
      for (int j = 0; j < meteorSize; j++) {
        if ( ( i - j < NUM_LEDS) && (i - j >= 0) ) {
          setPixel(i - j, red, green, blue);
        }

      }

      if (checkInput()) return;

      showStrip();
      //      delay(SpeedDelay/2);
    }
    for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {

      // fade brightness all LEDs one step
      for (int j = 0; j < NUM_LEDS; j++) {
        if ( (!meteorRandomDecay) || (random(10) > 5) ) {
          fadeToBlack(j, meteorTrailDecay );
        }
      }
      if (checkInput()) return;
      // draw meteor
      for (int j = 0; j < meteorSize; j++) {
        if ( ( i - j < NUM_LEDS) && (i - j >= 0) ) {
          setPixel(i - j, red, green, blue);
        }

      }

      if (checkInput()) return;

      showStrip();
      //      delay(SpeedDelay/2);
    }
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
  leds[ledNo].fadeToBlackBy( fadeValue );
}

void FadeInOutCombined() {
  while (true) {
    if (checkInput()) return;
    FadeInOut(255, 0, 0);
    if (checkInput()) return;
    FadeInOut(255, 255, 0);
    if (checkInput()) return;
    FadeInOut(255, 0, 255);
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
