#include "Activator.h"

#include <PWMServo.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

class Activator::Implementation {
public:
    Implementation(uint8_t pin, uint8_t released, uint8_t restrained)
        : pin(pin), released(released), restrained(restrained) {
    }

    bool begin() {
        servo.attach(pin);
        return true;
    }

    bool loop() {
        return true;
    }

    bool release() {
        servo.write(released);
        return true;
    }

    bool restrain() {
        servo.write(restrained);
        return true;
    }

private:
    uint8_t pin;

    uint8_t released;
    uint8_t restrained;

    PWMServo servo;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

Activator::Activator(uint8_t pin, uint8_t released, uint8_t restrained)
    : impl(new Implementation(pin, released, restrained)) {
}

Activator::~Activator() {
    delete impl;
}

bool Activator::begin() {
    return impl->begin();
}

bool Activator::loop() {
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool Activator::release() {
    return impl->release();
}

bool Activator::restrain() {
    return impl->restrain();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
