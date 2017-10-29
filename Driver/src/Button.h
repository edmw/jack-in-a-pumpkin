#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <Arduino.h>

class Button {
    class Implementation;

public:
    enum class PullUp : bool { Disable, Enable };

    Button(uint8_t pin, PullUp enablePullUp, uint32_t debounceTime);
    ~Button();

    bool begin(void);
    bool read(void);

    bool isPressed(void);
    bool isReleased(void);
    bool wasPressed(void);
    bool wasReleased(void);
    bool pressedFor(uint32_t ms);
    bool releasedFor(uint32_t ms);
    uint32_t lastChange(void);

private:
    Button(const Button&);
    Button& operator=(const Button&);

    Implementation *impl;
};

#endif
