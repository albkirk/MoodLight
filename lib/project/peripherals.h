// Declare and define here all Peripheral (sensors and actuators) used in this project.
// **** Periphericals definition here ...
// -- LED Lights GPIO & Configuration
//#define LED_RGB                                         // Uncoment to be used by color.h library
#define LED_NEO                                         // Uncoment to be used by color.h library
#define PIN_RED             -1                            // PWM Output PIN for RED  -1 means NOT used!
#define PIN_GREEN           -1                            // PWM Output PIN for GREEN  -1 means NOT used!
#define PIN_BLUE            -1                            // PWM Output PIN for BLUE  -1 means NOT used!
#define NEOPixelsPIN        16                            // NeoPixels DATA GPIO pin.
#define NEOPixelsNUM        64                            // Number of NeoPixels LEDs attached
uint8_t NEO_BRIGHTNESS    = 20;                           // NEO brightness [0-255]

// **** Libraries to include here ...
#include <color.h>
#include <buttons.h>


// **** Peripherals and critical functions here ...
void on_wakeup() {

}

void before_GoingToSleep() {
    yield();
    color_set(BLACK);
}


// One of the 1st functions to be called on main.cpp in order to initialize critical components,
// such as: GPIO config, sensors, actuators, wake-up validation...  
void peripherals_setup() {
// Output GPIOs
 
// Input GPIOs

// Setup functions
    color_setup();
// Wake-up validation
    on_wakeup();

// Setup buttons
    buttons_setup();
}

