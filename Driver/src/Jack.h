#define RX0 12
#define TX0 11

#define SENSOR_PIN 7
#define BUTTON_PIN 4
#define BUTTON_DEBOUNCE 25
#define SERVO_PIN 9
#define SERVO_RELEASED 0
#define SERVO_REFRAINED 55
#define LEDS_PIN 8
#define LEDS_COUNT 12

#define ENUNCIATOR_VOLUME 30
#define INDICATOR_BRIGHTNESS 50
#define RECEPTOR_DEBOUNCE 25

#define DEBUG true
#define DSERIAL_BEGIN if (DEBUG) Serial.begin(9600)
#define DSERIAL if (DEBUG) Serial
