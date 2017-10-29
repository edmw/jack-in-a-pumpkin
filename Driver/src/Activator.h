#ifndef __ACTIVATOR_H__
#define __ACTIVATOR_H__

#include <Arduino.h>

class Activator {
    class Implementation;

public:
    Activator(uint8_t pin, uint8_t released, uint8_t refrained);
    ~Activator();

    bool begin(void);
    bool loop(void);

    bool release(void);
    bool restrain(void);

private:
    Activator(const Activator&);
    Activator& operator=(const Activator&);

    Implementation *impl;
};

#endif
