#ifndef __INDICATOR_H__
#define __INDICATOR_H__

#include <Arduino.h>

#include "Jack.h"

class Indicator {
    class Implementation;

public:
    Indicator(uint8_t pin, uint8_t count);
    ~Indicator();

    bool begin(uint8_t brightness);
    bool loop(void);

    void lightUp(void);
    void danceIn(void);
    void turnOff(void);

private:
    Indicator(const Indicator&);
    Indicator& operator=(const Indicator&);

    Implementation *impl;
};

#endif
