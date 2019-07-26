#include <HBridge.h>
#include <FlySkyIBus.h>
#include <ReceiveOnlySoftwareSerial.h>

// Channel selection

const int TURN_CHANNEL = 2;
const int FORWARD_CHANNEL = 1;
const int STRAFE_CHANNEL = 0;


// Radio settings
const float DEAD_BAND_FRC = 0.1;
const long RADIO_TIMEOUT = 500;

 // Enum style constants
const int SLOW_LED = 1000;
const int MED_LED = 2000;
const int FAST_LED = 100;

// Pin setup
const int ledPin = LED_BUILTIN;

// Pins available for motors are 3 - 12 (10 pins) and 14-17 (4 pins)
HBridge fl_motor(14, 15, 11, DEAD_BAND_FRC);
HBridge fr_motor(3, 4, 10, DEAD_BAND_FRC);
HBridge rl_motor(7, 8, 5, DEAD_BAND_FRC);
HBridge rr_motor(9, 12, 6, DEAD_BAND_FRC);

ReceiveOnlySoftwareSerial recSerial (2);  // Rx pin

// Initial state
int armedState = DISARMED;
int ledState = LOW;
unsigned long previousMillis = 0;
long led_flash_interval = SLOW_LED;


void setup() {
    Serial.begin(115200);
    IBus.begin(recSerial);
    pinMode(ledPin, OUTPUT);
}

void loop() {
    IBus.loop();

    if ( IBus.millisSinceUpdate() > RADIO_TIMEOUT) {
        armedState = DISARMED;
        led_flash_interval = FAST_LED;
    } else {
        armedState = ARMED;
        led_flash_interval = SLOW_LED;
    }

    updateLed();
    
    int strafe = IBus.readChannel(STRAFE_CHANNEL);
    int steering = IBus.readChannel(STEERING_CHANNEL);
    int throttle = IBus.readChannel(THROTTLE_CHANNEL);

    if (armedState == DISARMED) {
        throttle = 1500;
        steering = 1500;
        strafe = 1500;
    }

    setControls(throttle, steering, strafe);
}

void updateLed() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= led_flash_interval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (ledState == LOW) {
            ledState = HIGH;
        } else {
            ledState = LOW;
        }

        // set the LED with the ledState of the variable:
        digitalWrite(ledPin, ledState);
    }
}

void setControls(int throttle, int steering, int strage) {
    // Firstly scale 1000 to 2000 => -1 to 1
    
    float _throttle = normalise(throttle);
    float _steering = normalise(steering);
    float _strafe = normalise(strafe);
    
    float fl = _throttle + _steering + _strafe;    
    float fr = _throttle - _steering - _strafe;
    float rl = _throttle + _steering - _strafe;
    float rr = _throttle - _steering + _strafe;
    
    // These are now scaled -3 to 3.
    // find the largest and scale to that
    float largest = max(max(abs(fl), abs(fr)), max(abs(rl), abs(rr)));
    if (largest > 1.0) {
        fl = fl / largest;
        fr = fr / largest;
        rl = rl / largest;
        rr = rr / largest;
    }
    //now they are scaled -1 to 1 again
    
    fl_motor.set_signal(fl);
    fr_motor.set_signal(fr);
    rl_motor.set_signal(rl);
    rr_motor.set_signal(rr);
}


float normalise(float signal) {
    return mapfloat(signal, 1000.0, 2000.0, -1.0, 1.0);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}