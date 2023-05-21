#include "lamp.h"

#define FASTLED_INTERNAL  // disable pragma messages
#include "FastLED.h"

int hue1 = 60;
int hue2 = 120;
float brightness = 1.0;

union color {
  struct {
    float r, g, b;
  };
  float arr[3];
};

const int LED_PIN = 14;
const int SECTIONS = 7;
const int SECTION_SIZE = 16;
const int LED_NUM = SECTION_SIZE * SECTIONS;
const float LAMP_LEN = 0.5f + 0.5f * (SECTIONS / 2);

CRGB leds[LED_NUM];

long crgb(color c) {
  return ((long)(c.r * 255) << 16) + ((long)(c.g * 255) << 8) + (long)(c.b * 255);
}
color hsv(float h, float s, float v) {
  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60, 2) - 1));
  float m = v - c;
  c += m;
  x += m;
  switch ((int)floor(h / 60)) {
    case 0:
      return { c, x, m };
    case 1:
      return { x, c, m };
    case 2:
      return { m, c, x };
    case 3:
      return { m, x, c };
    case 4:
      return { x, m, c };
    case 5:
      return { c, m, x };
    default:
      return { 0, 0, 0 };
  }
}
float percent(float num, float of) {
  return num / of;
}
float lerp(float a, float b, float frac) {
  return a * (1 - frac) + b * frac;
}
float to_srgb(float l) {
  if (l <= 0.0031308f)
    return 12.92f * l;
  else
    return sq((l + 0.055f) / 1.055f);
}
float to_linear(float s) {
  if (s <= 0.0031308f)
    return s / 12.92f;
  else
    return (1.055f * sqrt(s)) - 0.055f;
}
color filter_color(const color& c, float (*func)(float)) {
  color r;
  for (int i = 0; i < 3; i++)
    r.arr[i] = func(c.arr[i]);
  return r;
}
color interpolate_color(const color& col1, const color& col2, float frac) {
  color lin1 = filter_color(col1, to_linear);
  color lin2 = filter_color(col2, to_linear);

  color res = {
    lerp(lin1.r, lin2.r, frac),
    lerp(lin1.g, lin2.g, frac),
    lerp(lin1.b, lin2.b, frac),
  };

  return filter_color(res, to_srgb);
}
color interpolate_simple(color col1, color col2, float frac) {
  return {
    lerp(col1.r, col2.r, frac),
    lerp(col1.g, col2.g, frac),
    lerp(col1.b, col2.b, frac)
  };
}

float section_delta(int section) {
  if (section % 2 == 0)
    return -0.5f;
  else
    return 1.0f;
}
float section_start(int section) {
  float group_start = section / 4 * 1.0f;
  float local_start;
  switch (section % 4) {
    case 0:
    case 3:
      local_start = 0.5f;
      break;
    case 1:
      local_start = 0;
      break;
    case 2:
      local_start = 1.0f;
      break;
  }
  return group_start + local_start;
}

void setup_lamp() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.setBrightness(255);
}

void loop_lamp() {
  float s = 1;
  float v = sq(brightness);
  color col1 = hsv(hue1 % 360, s, v);
  color col2 = hsv(hue2 % 360, s, v);

  const int samples = 112;
  long gradient[samples];
  for (int i = 0; i < samples; i++) {
    float frac = i / (float)samples;
    gradient[i] = crgb(interpolate_color(col1, col2, frac));
  }
  for (int led = 0; led <= LED_NUM; led++) {
    int section = led / SECTION_SIZE;
    int local_led = led % SECTION_SIZE;
    float local_pos = percent(local_led, SECTION_SIZE);
    float pos = section_start(section) + section_delta(section) * local_pos;
    float frac = pos / LAMP_LEN;
    int i = round(frac * (samples - 1));


    leds[led] = gradient[i];
  }
  FastLED.show();
}