#include "Button.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class Button::Implementation {
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

        state = !(digitalRead(pin) == HIGH);

        return true;
    }

    bool read(void) {
        uint32_t ms = millis();
        if (stateMillis - stateChangedMillis >= debounceTime) {
            lastState = state;

            state = !(digitalRead(pin) == HIGH);
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

    bool isPressed(void) {
        return state;
    }

    bool isReleased(void) {
        return !state;
    }

    bool wasPressed(void) {
        return state && stateChanged;
    }

    bool wasReleased(void) {
        return !state && stateChanged;
    }

    bool pressedFor(uint32_t ms) {
        return (state && stateMillis - stateChangedMillis >= ms);
    }

    bool releasedFor(uint32_t ms) {
        return (!state && stateMillis - stateChangedMillis >= ms);
    }

    uint32_t lastChange(void) {
        return stateChangedMillis;
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

Button::Button(uint8_t pin, PullUp enablePullUp, uint32_t debounceTime)
    : impl(new Implementation(pin, enablePullUp, debounceTime)) {
}

Button::~Button() {
    delete impl;
}

bool Button::begin() {
    return impl->begin();
}

bool Button::read() {
    return impl->read();
}

bool Button::isPressed(void) {
    return impl->isPressed();
}

bool Button::isReleased() {
    return impl->isReleased();
}

bool Button::wasPressed() {
    return impl->wasPressed();
}

bool Button::wasReleased() {
    return impl->wasReleased();
}

bool Button::pressedFor(uint32_t ms) {
    return impl->pressedFor(ms);
}

bool Button::releasedFor(uint32_t ms) {
    return impl->releasedFor(ms);
}

uint32_t Button::lastChange() {
    return impl->lastChange();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
