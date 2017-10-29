#include "Enunciator.h"

#include <DFMiniMp3.h>

#define MP3_OVERTURE 1
#define MP3_LAUGHOUT 2
#define MP3_IM_SO_READY 3
#define MP3_WAIT_A_MINUTE 4
#define MP3_COME_ON 5
#define MP3_HALLOWEEN_SONG 6

///////////////////////////////////////////////////////////////////////////////////////////////////

class DFMiniMp3Handler {
public:
    static void OnError(unsigned int errorCode) {
    }
    static void OnPlayFinished(unsigned int globalTrack) {
    }
    static void OnCardOnline(unsigned int code) {
    }
    static void OnCardInserted(unsigned int code) {
    }
    static void OnCardRemoved(unsigned int code) {
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class Enunciator::Implementation {
public:
    Implementation(SoftwareSerial& serial)
        : mp3(serial) {
    }

    bool begin(uint8_t volume) {
        this->volume = volume;
        mp3.begin();
        mp3.setVolume(volume);
        return true;
    }

    bool loop() {
        mp3.loop();
        return true;
    }

    void turnOff() {
        mp3.stop();
    }

    void play(uint8_t track, uint16_t wait_ms, uint8_t track_volume = 30) {
        uint16_t volume = (this->volume * track_volume) / 30;
        mp3.setVolume(volume);
        mp3.playMp3FolderTrack(track);
        if (wait_ms) wait_millis(wait_ms);
    }

    void wait_millis(uint16_t ms) {
        uint32_t start = millis();
        while ((millis() - start) < ms) {
            mp3.loop();
            delay(1);
        }
    }

private:
    DFMiniMp3<SoftwareSerial, DFMiniMp3Handler> mp3;

    uint8_t volume;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

Enunciator::Enunciator(SoftwareSerial& serial)
    : impl(new Implementation(serial)) {
}

Enunciator::~Enunciator() {
    delete impl;
}

bool Enunciator::begin(uint8_t volume) {
    return impl->begin(volume);
}

bool Enunciator::loop() {
    return impl->loop();
}

void Enunciator::turnOff() {
    impl->turnOff();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Enunciator::overture(Playback playback) {
    impl->play(MP3_OVERTURE, bool(playback) ? 40000 : 0);
}

void Enunciator::announce_waiting_time(Playback playback) {
    impl->play(MP3_WAIT_A_MINUTE, bool(playback) ? 567 : 0);
}

void Enunciator::announce_readiness(Playback playback) {
    impl->play(MP3_IM_SO_READY, bool(playback) ? 1568 : 0);
}

void Enunciator::announce_adjustment_phase(Playback playback) {
    impl->play(MP3_COME_ON, bool(playback) ? 433 : 0);
}

void Enunciator::laughout(Playback playback) {
    impl->play(MP3_LAUGHOUT, bool(playback) ? 3936 : 0);
}

void Enunciator::halloween_song(Playback playback) {
    impl->play(MP3_HALLOWEEN_SONG, bool(playback) ? 206968 : 0, 15);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
