#include "Indicator.h"

#include <FastLED.h>

#include "millis.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

#define FPS 25

enum mode_t {
    off,
    fire,
    disco
};

struct flame_t {
    uint8_t heat;
    uint8_t increment;
    bool cooling;
};

#define PURPLE 0x6611FF
#define ORANGE 0xFF6600
#define GREEN  0x00FF11
#define WHITE  0xCCCCCC

const CRGBPalette16 HalloweenColorsPalette (
    PURPLE, PURPLE, PURPLE, PURPLE,
    ORANGE, ORANGE, ORANGE, ORANGE,
    PURPLE, PURPLE, PURPLE, PURPLE,
    GREEN,  GREEN,  GREEN,  WHITE
);

CRGBPalette16 strobePalette(HalloweenColorsPalette);

///////////////////////////////////////////////////////////////////////////////////////////////////

class Indicator::Implementation {
public:
    Implementation(uint8_t pin, uint8_t count)
        : pin(pin), count(count), leds(new CRGB[count]) {

        flames_count = count / 3;
        flames = new flame_t[flames_count];

        strobe = 0;
    }

    ~Implementation() {
        delete leds;
        delete flames;
    }

    bool begin(uint8_t brightness) {
        this->brightness = brightness;

        FastLED.addLeds<APA106, LEDS_PIN, GRB>(leds, count);
        FastLED.clear(true);

        return true;
    }

    bool loop() {
        if (loop_ms > 1000 / FPS) {

            switch (mode) {
            case fire:
                burn();
                break;
            case disco:
                pulse();
                break;
            case off:
                break;
            }
            FastLED.show(mode_brightness);

            loop_ms = 0;
        }
        return true;
    }

    void lightUp(void) {
        light_fire();
        mode = fire;
        mode_brightness = scale8(brightness, 100);
    }

    void danceIn(void) {
        mode = disco;
        mode_brightness = brightness;
    }

    void turnOff(void) {
        mode = off;
        FastLED.clear(true);
    }

private:
    uint8_t pin;
    uint8_t count;

    CRGB *leds;

    uint8_t brightness = 20;

    elapsed_millis loop_ms;

    uint8_t flames_count;
    flame_t *flames;

    uint8_t strobe;
    uint8_t strobeCounter;
    uint8_t strobeHue;
    int8_t strobePosition;

    mode_t mode = off;
    uint8_t mode_brightness;
 
    void light_flame(uint8_t index) {
        flames[index].heat = qadd8(flames[index].heat, random8(160, 255));
        flames[index].increment = random8(1, 70);
        flames[index].cooling = false;
    }

    void light_fire(void) {
        for (uint8_t index = 0; index < flames_count; index++) {
            light_flame(index);
        }
    }

    void burn(void) {
        // Fire flame effect inspired by
        // https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino

        for (uint8_t index = 0; index < flames_count; index++) {
            if (flames[index].cooling) {
                flames[index].heat = qsub8(flames[index].heat, flames[index].increment);
                if (flames[index].heat == 0x00) {
                    light_flame(index);
                }
            }
            else {
                flames[index].heat = qadd8(flames[index].heat, flames[index].increment);
                if (flames[index].heat == 0xFF) {
                    flames[index].cooling = true;
                }
            }
        }
        for (uint8_t index = 0; index < flames_count; index++) {
            CRGB color;
            uint8_t number = index * 3;
            color = HeatColor(flames[index].heat);
            leds[number + 1] = color;
            color = HeatColor(flames[index].heat * 0.66);
            leds[number + 0] = color;
            leds[number + 2] = color;
        }
    }

    void pulse(void) {
        // Disco strobe effect taken from
        // https://gist.github.com/kriegsman/626dca2f9d2189bd82ca

        FastLED.clear();

        strobe = (strobe + 1) % 4; if (strobe != 0) return;

        uint8_t dashperiod = beatsin8(8, 4, 10);
        uint8_t dashwidth = dashperiod / 4 + 1;
        int8_t  dashspeed = beatsin8(30, 1, dashperiod);

        if (dashspeed >= (dashperiod / 2)) {
            dashspeed = 0 - (dashperiod - dashspeed);
        }

        uint8_t huedelta = scale8(cubicwave8(ease8InOutCubic(ease8InOutCubic(beat8(2)))), 130);

        // work

        strobeHue += 1;

        strobeCounter += 1;
        if (strobeCounter >= 2) {
            strobeCounter = 0;

            strobePosition = strobePosition + dashspeed;
            if (strobePosition >= dashperiod) {
                while (strobePosition >= dashperiod) {
                    strobePosition -= dashperiod;
                }
                strobeHue -= huedelta;
            }
            else if (strobePosition < 0) {
                while (strobePosition < 0) {
                    strobePosition += dashperiod;
                }
                strobeHue += huedelta;
            }
        }

        // draw

        uint8_t hue = strobeHue;
        for(uint8_t i = strobePosition; i <= count - 1; i += dashperiod) {
            CRGB color = ColorFromPalette(strobePalette, hue, 255, NOBLEND);
            uint8_t p = i;
            for (uint8_t w = 0; w < dashwidth && p < count; w++, p++) {
                leds[p] = color;
            }
            hue += huedelta;
        }
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

Indicator::Indicator(uint8_t pin, uint8_t count)
    : impl(new Implementation(pin, count)) {
}

Indicator::~Indicator() {
    delete impl;
};

bool Indicator::begin(uint8_t brightness) {
    return impl->begin(brightness);
}

bool Indicator::loop() {
    return impl->loop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Indicator::lightUp() {
    impl->lightUp();
}

void Indicator::danceIn() {
    impl->danceIn();
}

void Indicator::turnOff() {
    impl->turnOff();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
