#ifndef __RECEPTOR_H__
#define __RECEPTOR_H__

#include <Arduino.h>

class Receptor {
    class Implementation;

public:
    enum class PullUp : bool { Disable, Enable };

    Receptor(uint8_t pin, PullUp enablePullUp, uint32_t debounceTime);
    ~Receptor();

    bool begin(void);
    bool loop(void);

    bool isTriggered(void);
    bool isClear(void);

    bool sensedMotion(void);

private:
    Receptor(const Receptor&);
    Receptor& operator=(const Receptor&);

    Implementation *impl;
};

#endif
