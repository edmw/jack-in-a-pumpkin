#include "Receptor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class Receptor::Implementation {
public:
    Implementation(uint8_t pin, PullUp enablePullUp, uint32_t debounceTime)
        : pin(pin), pinPullUp(bool(enablePullUp)), debounceTime(debounceTime) {
        state = false;
        stateMillis = millis();
        stateChanged = false;
        stateChangedMillis = stateMillis;
        lastState = state;
    }

    bool begin(void) {
        if (pinPullUp != 0) {
            pinMode(pin, INPUT_PULLUP);
        }
        else {
            pinMode(pin, INPUT);
        }

        state = (digitalRead(pin) == HIGH);

        return true;
    }

    bool loop(void) {
        uint32_t ms = millis();
        if (stateMillis - stateChangedMillis >= debounceTime) {
            lastState = state;

            state = (digitalRead(pin) == HIGH);
            stateChanged = (state != lastState);
            if (stateChanged) {
                stateChangedMillis = ms;
            }
        }
        else {
            stateChanged = false;
        }
        stateMillis = ms;
        return state;
    }

    bool isTriggered() {
        return state;
    }

    bool isClear() {
        return !state;
    }

    bool sensedMotion(void) {
        return state && stateChanged;
    }

private:
    uint8_t pin;
    uint8_t pinPullUp;
    uint32_t debounceTime;

    bool state;
    bool stateChanged;
    uint32_t stateMillis;
    uint32_t stateChangedMillis;

    bool lastState;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

Receptor::Receptor(uint8_t pin, PullUp enablePullUp, uint32_t debounceTime)
    : impl(new Implementation(pin, enablePullUp, debounceTime)) {
}

Receptor::~Receptor() {
    delete impl;
}

bool Receptor::begin() {
    return impl->begin();
}

bool Receptor::loop() {
    return impl->loop();
}

bool Receptor::isTriggered() {
    return impl->isTriggered();
}

bool Receptor::isClear() {
    return impl->isClear();
}

bool Receptor::sensedMotion() {
    return impl->sensedMotion();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
