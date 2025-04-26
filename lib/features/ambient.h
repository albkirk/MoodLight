/* -- Please, add the following variables to your code:
// **** Libraries to include here ...
#include <telemetry.h>
// -- DHT Definition --
#define DHTTYPE          2          // use 1 for "DHT11", 2 for "DHT22", or 3 for "AM2320" to select the DHT Model
#define DHT_PIN         -1          // GPIO connected to DHT Data PIN. -1 means NO DHT used!
// -- ADC PIN Definition --
#define NTC_ADC_PIN     -1          // IO pin for NTC ADC measurement.
#define LUX_ADC_PIN     -1          // IO pin for LUX ADC measurement.
#define GAS_ADC_PIN     -1          // IO pin for GAS ADC measurement.
#define GAS_VCC_PIN     -1          // IO pin for GAS VCC control [High / Low] = [1.5v / 5v].
#define Gas_Res_Div   true          // Do you have a Resistor divider (ence needs to calculate the proportion)?
#define Gas_Res_High   100          // High Resistor value (in KOhms)
#define Gas_Res_Lower  220          // Lower Resistor value (in KOhms)
*/

/*  -- How to use this library?
In your sketch, you can use the following get functions for individual values:
    getTemperature()
    getHumidity()
    getLux()
    getGas()
or get all at once with:
    ambient_get_data()
to get and send all ambient values via MQTT to HASSIO use the telemetry function:

*/

#include "Adafruit_Sensor.h"
#include <DHT.h>
#include <Wire.h>
#include <AM2320.h>

#define Ambient_Enabled                     // Flag to enable Ambient feature on WEB Service, telemetry, etc.
// -- ADC GPIO (ESP32 Only)
#ifdef ESP32
    #ifndef NTC_ADC_PIN
        #define NTC_ADC_PIN     -1          // IO pin for NTC ADC measurement. Default->36,  TFT->36, EPaper->35
    #endif
    #ifndef LUX_ADC_PIN
        #define LUX_ADC_PIN     -1          // IO pin for LUX ADC measurement.
    #endif
    #ifndef GAS_ADC_PIN
        #define GAS_ADC_PIN     -1          // IO pin for GAS ADC measurement.
    #endif
    #ifndef GAS_VCC_PIN
        #define GAS_VCC_PIN     -1          // IO pin for GAS VCC control [High / Low] = [1.5v / 5v].
    #endif
#else
    #define NTC_ADC_PIN         A0
    #define LUX_ADC_PIN         A0    
    #define GAS_ADC_PIN         A0    
#endif

#ifndef DHTTYPE
    #define DHTTYPE              2          // use 1 for "DHT11", 2 for "DHT22", or 3 for "AM2320" to select the DHT Model
#endif
#ifndef DHT_PIN
    #define DHT_PIN             -1          // IO pin for NTC ADC measurement. Default->36,  TFT->36, EPaper->35
#endif


// Initialize DHT/AM sensor.
#define DHT_11 1
#define DHT_22 2
#define AM_2320 3

#if DHTTYPE == 1
    #define DHTMODEL DHT11                 // using the DHT11 Model
#else
    #define DHTMODEL DHT22                 // using the DHT22 Model either for DHTTYPE == 2 or 3
#endif
DHT dht_val(DHT_PIN, DHTMODEL);
AM2320 am_val(&Wire);

//NTC temperature
#define Rs 47000                            // 47KOhm Voltage Dividor Resistor

int ADC_Min_Read = 0;                       // ADC minimum value
int ADC_Max_Read = 4095;                    // ADC maximum value 4095 -> 12 bits
int Min_PPM = 100;                          // MQ-7 Carbon Monoxide sensor Min PPM
int Max_PPM = 1000;                         // MQ-7 Carbon Monoxide sensor Max PPM
float Temperature = 0.0;                    // Main Variable Temperature
float Humidity = 0.0;                       // Main Variable Humidity
float HumVelocity = 0.0;                    // Main Variable Humidity Velocity
float Lux = 0.0;                            // Main Variable Illuminance
int Gas = 0;                                // Main Variable Carbon Monoxide
hw_timer_t *GAS_timer = NULL;               // Timer for GAS_VCC_CONTROL toggle
volatile bool GAS_VCC_CONTROL = false;      // Tracks current GAS_VCC_CONTROL state [High / Low] = [1.5v / 5v].


void I2C_scan() {
    byte error, address;
    int nDevices;

    telnet_println("Scanning...");

    nDevices = 0;
    for(address = 1; address < 127; address++ ) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            telnet_print("I2C device found at address 0x");
            if (address<16) telnet_print("0");
            telnet_print(String(address,HEX));
            telnet_println(" !");

            nDevices++;
        }
        else if (error==4) {
            telnet_print("Unknow error at address 0x");
            if (address<16) telnet_print("0");
            telnet_println(String(address,HEX));
        }
    }

    if (nDevices == 0) telnet_println("No I2C devices found\n");
    else telnet_println("done\n");
}

#if NTC_ADC_PIN >= 0
float getNTCThermister(byte adc_pin = NTC_ADC_PIN) {
  // Return temperature as Celsius
  int val = 0;
  for(int i = 0; i < Number_of_measures; i++) {         // ADC value is read N times
      val += analogRead(adc_pin);
      delay(10);
  }
  val = val / Number_of_measures;

  double V_NTC = (double)val / 1024;
  double R_NTC = (Rs * V_NTC) / (Vcc - V_NTC);
  R_NTC = log(R_NTC);
  double Tmp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * R_NTC * R_NTC ))* R_NTC );
  float Temp = (float)Tmp - 273.15 + config.Temp_Corr;
  return Temp;
}
#else
float getNTCThermister() {
    return -100;
}
#endif

float getTemperature() {
    // Return temperature as Celsius (DHT22 range -40 to +125 degrees Celsius) or -100 if error
    if (DHT_PIN>=0 || SDA_PIN>=0) {
        float t;
        int n = 0;
        int chk = 0;

        while (n < 10) {
            if (DHTTYPE == DHT_11 || DHTTYPE == DHT_22) {
                //telnet_println("CHK value: " + String(chk));
                t = dht_val.readTemperature();
                chk = int(isnan(t));
            }
            if (DHTTYPE == AM_2320) {
                chk = int(am_val.Read());
                t = am_val.temperature;
            }
            //Serial.print("Temperature: " + String(t));
            // Check if any reads failed and exit.
            if (chk != 0) {
                telnet_println("Failed to read temperature from DHT sensor!");
                delay(100);
                //t = NULL;
                n ++;
            }
            else {
                //telnet_print("Temperature: " + String(t));
                return  t + config.Temp_Corr;
            }
        };
    }
    else {
        return getNTCThermister();
    }
    return -100;            // This line will never run?
}


float getHumidity() {
    // Read Humidity as Percentage (0-100%) or -1 if error
    if (DHT_PIN>=0 || SDA_PIN>=0) {
        float h;
        int n = 0;
        int chk = 0;
        while (n < 10 ) {
          if (DHTTYPE == DHT_11 || DHTTYPE == DHT_22) {
              h = dht_val.readHumidity();
              chk = int(isnan(h));
          }
          if (DHTTYPE == AM_2320) {
              chk = int(am_val.Read());
              h = am_val.humidity;
          }
          //Serial.print("Humidity: " + String(h));
          // Check if any reads failed and exit.
          if (chk != 0) {
            Serial.println("Failed to read humidity from DHT sensor!");
            delay(100);
            //h = NULL;
            n ++;
          }
          else {
            //Serial.print("Humidity: " + String(h));
            return h;
          }
        }
        return -1;
    }
    else return -1;
}

#if LUX_ADC_PIN >= 0
float getLux (byte adc_pin = LUX_ADC_PIN, int Nmeasures = Number_of_measures, float Max_Read = 3050, float Min_Read = 0) {
    // adc_pin A0 on ESP8266 and 35 or 36 on ESP32
	// MAx and Min reads of 910 and 55 are empiric values extract while testing the circut
    float lux = 0.0;
    for(int i = 0; i < Nmeasures; i++) {
        lux += (Max_Read - (float)analogRead(adc_pin)) / (Max_Read - Min_Read) * 100;
        //telnet_println("Sample-LUX: " + String(lux));
        delay(25);
    }
	  lux = lux / Nmeasures;
    if ( lux < 0 )   lux = 0.0;
    if ( lux > 100 ) lux = 100.0;
    //telnet_println("LUX: " + String(lux));
    return lux;
}
#else
float getLux() {
    return -1;
}
#endif


//void Gas_read(){
void Gas_read (byte adc_pin = GAS_ADC_PIN, int Nmeasures = Number_of_measures, float Max_Read = ADC_Max_Read, float Min_Read = ADC_Min_Read) {
    // adc_pin A0 on ESP8266 and 35 or 36 on ESP32
    int reading = 0;
    int gas = 0;
    for(int i = 0; i < Nmeasures; i++) {
        reading += analogRead(adc_pin);  
        #if GAS_VCC_PIN == -1
            delay(10);                    // This breaks the onTimer() interrupt
        #else
            yield();
        #endif
    }
    reading = reading / Nmeasures;
    
    if(Gas_Res_Div) {
        reading =  reading * Gas_Res_High / Gas_Res_Lower + reading;
        Max_Read = Max_Read * Gas_Res_High / Gas_Res_Lower + Max_Read;
    }
    gas = exp(reading/1000) * reading/10;    // Empiric formula to aprox value. See excel sheet.
    if ( gas < 0 )   gas = 0;
    telnet_println("GAS: " + String(gas));
    Gas = int(gas);
}

#if GAS_VCC_PIN >= 0
// Function to control the voltage of the GAS sensor resistor from 5v to 1.5v
void IRAM_ATTR onTimer() {
    if (GAS_VCC_CONTROL) {
      // Switch to LOW state for 60 seconds
      digitalWrite(GAS_VCC_PIN, LOW);
      timerAlarmWrite(GAS_timer, 60000000, false);  // 60 seconds in microseconds
    } else {
      // Switch to HIGH state for 89 seconds and call get_gas()
      digitalWrite(GAS_VCC_PIN, HIGH);
      timerAlarmWrite(GAS_timer, 90000000, false);  // 90 seconds in microseconds
      Gas_read();
    }
    GAS_VCC_CONTROL = !GAS_VCC_CONTROL;             // Toggle state
    timerWrite(GAS_timer, 0);                       // Reset GAS_timer counter
}
#endif

#if GAS_ADC_PIN >= 0
    int getGas () {
        return Gas;
    }
#else
    int getGas() {
        return -1;
    }
#endif

void ambient_get_data() {
    Temperature = getTemperature();
    Humidity = getHumidity();

    #ifdef Batt_Lux_Switch
        digitalWrite(FET_Control_Pin, HIGH);   // Set ADC Switch to HIGH to sample Lux (BC547 Saturated)
        delay(100);
        telnet_println("ADC Switch in Lux position");
    #endif
    Lux = getLux();
    #ifdef Batt_Lux_Switch
        digitalWrite(FET_Control_Pin, LOW);   // Set ADC Switch to LOW to sample Battery (BC547 Cut)
        telnet_println("ADC Switch in VOLTAGE position");
    #endif
   
    Gas = getGas();
}


void ambient_send_data() {
    if ( Temperature == -100 ) {
            telnet_print("Temperatura: -- ERRO! -- \t");
            mqtt_publish(mqtt_pathtele, "Status", "ERRO-Temperatura");
    } else {
            telnet_print("Temperatura: " + String(Temperature) + " C \t");
            mqtt_publish(mqtt_pathtele, "Temperature", String(Temperature));
    };
    if ( Humidity == -1 ) {
            telnet_print("Humidade: -- ERRO! -- \t");
            mqtt_publish(mqtt_pathtele, "Status", "ERRO-Humidade");
    } else {
            telnet_print("Humidade: " + String(Humidity) + " % \t");
            mqtt_publish(mqtt_pathtele, "Humidity", String(Humidity));
    };
    if ( Lux == -1 ) {
            telnet_print("Lux: -- ERRO! -- \t");
            mqtt_publish(mqtt_pathtele, "Status", "ERRO-Lux");
    } else {
            telnet_print("Lux: " + String(Lux) + " % \t");
            mqtt_publish(mqtt_pathtele, "Lux", String(Lux));
    };
    if ( Gas == -1 ) {
            telnet_print("Gas: -- ERRO! -- \t");
            mqtt_publish(mqtt_pathtele, "Status", "ERRO-Gas");
    } else {
            telnet_print("Gas: " + String(Gas) + " ppm \t");
            mqtt_publish(mqtt_pathtele, "Gas", String(Gas));
    };
    telnet_println("");
}


void ambient_data() {
    ambient_get_data();
    ambient_send_data();
}


void ambient_setup() {
    if (DHT_PIN>=0 || SDA_PIN>=0) {
        // Start Ambient Sensor
        if (DHTTYPE == DHT_11 || DHTTYPE == DHT_22) dht_val.begin();       // required if using Adafruit Library
        if (DHTTYPE == AM_2320) {
            //I2C_scan();
            Wire.begin(SDA_PIN, SCK_PIN);
        }
    }

#ifdef ESP32
    #if NTC_ADC_PIN>=0
        analogSetPinAttenuation(NTC_ADC_PIN,ADC_11db);  // ADC_11db provides an attenuation so that IN/OUT = 1 / 3.6.
        adcAttachPin(NTC_ADC_PIN);                      // An input of 3 volts is reduced to 0.833 volts before ADC measurement
    #endif
    #if LUX_ADC_PIN>=0
        analogSetPinAttenuation(LUX_ADC_PIN,ADC_11db);  // ADC_11db provides an attenuation so that IN/OUT = 1 / 3.6.
        adcAttachPin(LUX_ADC_PIN);                      // An input of 3 volts is reduced to 0.833 volts before ADC measurement
    #endif
    #if GAS_ADC_PIN>=0
        analogSetPinAttenuation(GAS_ADC_PIN,ADC_11db);  // ADC_11db provides an attenuation so that IN/OUT = 1 / 3.6.
        adcAttachPin(GAS_ADC_PIN);                      // An input of 3 volts is reduced to 0.833 volts before ADC measurement
    #endif
    #if GAS_VCC_PIN>=0
    pinMode(GAS_VCC_PIN, OUTPUT);
    digitalWrite(GAS_VCC_PIN, LOW);     // Initial state LOW
    
    // Initialize timer 0 with 1 MHz frequency (80MHz clock with 80 prescaler)
    GAS_timer = timerBegin(0, 80, true);
    
    // Attach interrupt to timer
    timerAttachInterrupt(GAS_timer, &onTimer, true);
    
    // Set initial alarm for 60 seconds
    timerAlarmWrite(GAS_timer, 60000000, false);
    
    // Start timer
    timerAlarmEnable(GAS_timer);
    #endif
#endif
}
