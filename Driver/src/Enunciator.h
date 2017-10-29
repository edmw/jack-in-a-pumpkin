#ifndef __ENUNCIATOR_H__
#define __ENUNCIATOR_H__

#include <Arduino.h>
#include <SoftwareSerial.h>

class Enunciator {
    class Implementation;

public:
    enum class Playback : bool { Nonblocking, Blocking };

    Enunciator(SoftwareSerial& serial);
    ~Enunciator();

    bool begin(uint8_t volume);
    bool loop(void);

    void turnOff(void);

    void overture(Playback playback = Playback::Nonblocking);
    void announce_waiting_time(Playback playback = Playback::Nonblocking);
    void announce_readiness(Playback playback = Playback::Nonblocking);
    void announce_adjustment_phase(Playback playback = Playback::Nonblocking);
    void laughout(Playback playback = Playback::Nonblocking);
    void halloween_song(Playback playback = Playback::Nonblocking);

private:
    Enunciator(const Enunciator&);
    Enunciator& operator=(const Enunciator&);

    Implementation *impl;
};

#endif
