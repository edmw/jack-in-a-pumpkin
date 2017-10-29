#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Jack-In-The-Box driver for Arduino
//
// The Jack-In-The-Box consists of these parts:
// * Motion sensor for triggering
// * Servo for activation
// * MP3 module for audio effects
// * LED module for visual effects
// * Push-Button
//
// The driver implements this flow:
//  1. Adjusts the servo to release the Jack-In-The-Box activator.
//     Uses the MP3 module to play a notification sound.
//  2. (Now the user can setup the Jack-In-The-Box activator)
//  3. Waits until push button is pressed to indicate the user
//     finished setting up the Jack-In-The-Box activator.
//  4. Adjusts the servo to retain the Jack-In-The-Box activator.
//     Uses the MP3 module to play a notification sound.
//  5. (Now the user should leave the area of the Jack-In-The-Box motion sensor)
//  6. Waits until there is no motion detected (waits at least 5 seconds).
//  7. Plays a sound using the MP3 module to signal Jack-In-The-Box is set up.
//     Begins to visualize fire flames using the LED module.
//  8. Waits until there is motion detected.
//  9. Adjusts the servo to release the Jack-In-The-Box activator.
//     Plays a sound using the MP3 module to signal activator is triggered.
//     Begins to visualize disco strobe using the LED module.
// 10. Stopps all sounds and lights after Jack finished. Starts again at step 1.
//
// Copyright (c) 2017 Michael Baumgärtner
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <SoftwareSerial.h>

#include "Jack.h"

#include "Button.h"
#include "Enunciator.h"
#include "Receptor.h"
#include "Activator.h"
#include "Indicator.h"

#include "millis.h"

SoftwareSerial Serial0(RX0, TX0);

Enunciator enunciator(Serial0);

Indicator indicator(LEDS_PIN, LEDS_COUNT);

Activator activator(SERVO_PIN, SERVO_RELEASED, SERVO_REFRAINED);

Receptor receptor(SENSOR_PIN, Receptor::PullUp::Enable, RECEPTOR_DEBOUNCE);

Button button(BUTTON_PIN, Button::PullUp::Enable, BUTTON_DEBOUNCE);

#undef WARMUP

void warmup() {
    // Warmup Jack-In-The-Box:
    DSERIAL.println(F("Jack Warmup ..."));
    enunciator.overture(Enunciator::Playback::Blocking);
}

void setup() {
    DSERIAL_BEGIN;

    // Setup Jack-In-The-Box:
    DSERIAL.println(F("Jack Setup ..."));

    enunciator.begin(ENUNCIATOR_VOLUME);
    indicator.begin(INDICATOR_BRIGHTNESS);
    receptor.begin();
    activator.begin();
    button.begin();

    #ifdef WARMUP
    warmup();
    #endif

    DSERIAL.println(F("Jack In-The-Box running ..."));
}

// states
enum state_t {
    installed,

    // Jack is prepared to be mounted
    prepared,
    // Jack is mounted
    mounted,

    // Jack is equipped to be triggered
    equipped,
    // Jack is triggered
    triggered,

    // Jack stopped his actions
    stopped,

    // Jack crashed
    crashed,

    // intermediate states
    button_pressed,
    button_released
};

// active state
state_t state = installed;
// elapsed time since the state became active
elapsed_millis state_time;

state_t button_next_state;

void transition(state_t to);
void transition_with_button_pressed(state_t to);

void loop() {
    button.read();

    enunciator.loop();
    indicator.loop();
    receptor.loop();
    activator.loop();

    // handle states (not transitions)
    switch (state) {
    case installed: {
        // ready to go
        transition(prepared);
        break;
    }
    case prepared: {
        if (button.wasReleased()) {
            transition(mounted);
        }
        break;
    }
    case mounted: {
        if (state_time > 5000 && receptor.isClear()) {
            transition(equipped);
        }
        if (button.wasReleased()) {
            transition(prepared);
        }
        if (button.pressedFor(3000)) {
            transition_with_button_pressed(equipped);
        }
        break;
    }
    case equipped: {
        if (receptor.sensedMotion() || button.wasPressed()) {
            transition(triggered);
        }
        break;
    }
    case triggered: {
        if (state_time > 10000 || button.wasPressed()) {
            transition(stopped);
        }
        break;
    }
    case stopped: {
        if (button.isReleased()) {
            transition(prepared);
        }
        break;
    }
    case crashed: {
        // Jack is crashed, flash built-in led forever
        unsigned long ms = millis();
        digitalWrite(LED_BUILTIN, ((ms % 1000 < 100) ? HIGH : LOW));
        break;
    }
    case button_pressed: {
        if (button.wasReleased()) {
            transition(button_next_state);
        }
        break;
    }
    case button_released: {
        if (button.wasPressed()) {
            transition(button_next_state);
        }
        break;
    }
    default:
        transition(crashed);
        break;
    }
}

// handle transitions (not states)
void transition(state_t to) {
    DSERIAL.print(F("Jack goes from "));
    DSERIAL.print(state);
    DSERIAL.print(" to ");
    DSERIAL.println(to);

    switch (state) {
    case prepared:
        switch (to) {
        case mounted:
            activator.restrain();
            enunciator.announce_waiting_time();
            break;
        default: to = crashed; break;
        }
        break;
    case mounted:
        switch (to) {
        case equipped:
            enunciator.halloween_song();
            indicator.lightUp();
            break;
        case prepared:
            activator.release();
            break;
        case button_pressed:
            break;
        default: to = crashed; break;
        }
        break;
    case equipped:
        switch (to) {
        case triggered:
            activator.release();
            enunciator.laughout();
            indicator.danceIn();
            break;
        default: to = crashed; break;
        }
        break;
    case triggered:
        switch (to) {
        case stopped:
            indicator.turnOff();
            break;
        default: to = crashed; break;
        }
        break;
    default: break;
    }
    switch (to) {
    case prepared:
        activator.release();
        enunciator.announce_adjustment_phase();
        break;
    case crashed:
        DSERIAL.println(F("Jack crashed ..."));
        enunciator.turnOff();
        indicator.turnOff();
        break;
    default: break;
    }
    state = to;
    state_time = 0;
}
void transition_with_button_pressed(state_t to) {
    transition(to);

    button_next_state = to;

    state = button_pressed;
    state_time = 0;
}
