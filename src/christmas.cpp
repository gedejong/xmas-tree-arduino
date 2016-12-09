#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
#define NUMPIXELS 46
#define FLICKERING true
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 20
int tick = 0;
uint8_t twinkle = 20;

template <typename T> int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

uint8_t oldColors[NUMPIXELS * 3];
uint8_t targetColors[NUMPIXELS * 3];

void setup() {
  Serial.begin(9600);
  pixels.begin();

  memset(oldColors, 0, NUMPIXELS * 3);
  memset(targetColors, 0, NUMPIXELS * 3);
}

float gettr(int pos) {
  return targetColors[pos * 3] * (.8 + .2 * sin(.03 * (tick + pos)));
}

float gettg(int pos) {
  return targetColors[pos * 3 + 1] * (.8 + .2 * sin(.05 * (tick + pos)));
}

float gettb(int pos) {
  return targetColors[pos * 3 + 2] * (.8 + .2 * sin(.04 * (tick + pos)));
}

void setGammaColor(int pixelNumber, uint8_t red, uint8_t green, uint8_t blue) {
  oldColors[pixelNumber * 3] = red;
  oldColors[pixelNumber * 3 + 1] = green;
  oldColors[pixelNumber * 3 + 2] = blue;

  pixels.setPixelColor(pixelNumber,
    pgm_read_byte(&gamma[red]),
    pgm_read_byte(&gamma[green]),
    pgm_read_byte(&gamma[blue]));
}

void loop() {
  tick = tick % (1 << 15) + 1;

  for (int i = 0; i < NUMPIXELS; i++) {
    uint8_t tr = gettr(i), tg = gettg(i), tb = gettb(i);
    uint8_t r = oldColors[i * 3], g = oldColors[i * 3 + 1], b = oldColors[i * 3 + 2];
    uint8_t nr = r + sgn(tr - r) + (tr - r) / 16, 
      ng = g + sgn(tg - g) + (tg - g) / 16, 
      nb = b + sgn(tb - b) + (tb - b) / 16;

    if (FLICKERING && (rand() % (twinkle*3) ) == 0) {
      nr = min(255, nr * 2);
      ng = min(255, ng * 2);
      nb = min(255, nb * 2);
    }

    setGammaColor(i, nr, ng, nb);
  }
  pixels.show(); 
  delay(DELAYVAL);

  while (Serial.available() > 0) {
      uint8_t command = Serial.read();
      if (command == 0) {
          // Led light
          uint8_t pixelNumber = Serial.read();
          uint8_t red = Serial.read();
          uint8_t green = Serial.read();
          uint8_t blue = Serial.read();
          targetColors[pixelNumber * 3] = red;
          targetColors[pixelNumber * 3 + 1] = green;
          targetColors[pixelNumber * 3 + 2] = blue;
      } else if (command == 1) {
        twinkle = Serial.read();
      }
  }
}
