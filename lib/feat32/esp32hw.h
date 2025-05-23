#ifndef _WIFI_WRAPPER_H
    #define _WIFI_WRAPPER_H 
    /* inner content only filled with stuff if we're on esp32 or esp8266 platforms.. */
    #if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
        #include <WiFi.h> 
        /* other stuff */
    #endif
#endif
//#include <BLEDevice.h>
#include <rom/rtc.h>
#include <Preferences.h>
#include <WebServer.h>
#ifdef IP5306
    #include <ip5306.h>
#endif
#ifdef ULP_Support
   #include <myulp.h>
#endif

uint32_t getChipId();
String HEXtoUpperString(uint32_t hexval, uint hexlen);

//System Parameters
String ChipID=HEXtoUpperString(getChipId(), 6);
#define ESP_SSID String("ESP-" + ChipID)    // SSID to use as Access Point
#define Number_of_measures 5                // Number of value samples (measurements) to calculate average
unsigned long SLEEPTime = 0;                // Variable to allow temporary change the sleeptime (ex.: = 0)
bool Celular_Connected = false;             // Modem Connection state


// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
// so the RTC data structure should be padded to a 4-byte multiple.
RTC_DATA_ATTR struct {
  uint32_t crc32 = 0U;                      // 4 bytes   
  unsigned long lastUTCTime = 0UL;          // 4 bytes
  uint8_t bssid[6];                         // 32 bytes
  uint8_t LastWiFiChannel = 0;              // 1 byte,   1 in total
  //uint8_t padding[3];                       // 2 bytes,  4 in total
  uint8_t ByteValue = 0;                    // 1 byte,   2 in total
  //uint8_t padding1[3];                      // 2 bytes,  4 in total
  float FloatValue = 0.0f;                  // 4 bytes
} rtcData;

Preferences preferences;                    // Preferences library is wrapper around Non-volatile storage on ESP32 processor.

/*
// ADC to internal voltage
#if Using_ADC == false
    ADC_MODE(ADC_VCC)                       // Get voltage from Internal ADC
#endif
*/

// Initialize the Webserver
WebServer MyWebServer(80);

// initialize WiFi Security
// WiFiSec WiFiSec(CA_CERT_PROG, CLIENT_CERT_PROG, CLIENT_KEY_PROG);
WiFiClient secureclient;                    // Use this for secure connection
WiFiClient unsecuclient;                    // Use this for unsecure connection


// Battery & ESP Voltage
#define Batt_Max float(4.2)                 // Battery Highest voltage.  [v]
#define Batt_Min float(2.8)                 // Battery lowest voltage.   [v]
#define Vcc float(3.3)                      // Theoretical/Typical ESP voltage. [v]
#define VADC_MAX float(3.3)                 // Maximum ADC Voltage input


// Functions //
uint32_t getChipId() {
  uint8_t chipid[6];
  uint32_t output = 0;
  esp_efuse_mac_get_default(chipid);
  output = ((chipid[3] << 16) & 0xFF0000) + ((chipid[4] << 8) & 0xFF00) + ((chipid[5]) & 0xFF);
  return output;
}

String HEXtoUpperString(uint32_t hexval, uint hexlen) {
    String strgval = String(hexval, HEX);
    String PADZero;
    for (uint i = 0; i < (hexlen - strgval.length()) ; i++) PADZero +="0";
    strgval = PADZero + strgval;
    char buffer[hexlen+1];
    strcpy(buffer, strgval.c_str());
    for (uint i = 0; i < strgval.length() ; i++) {
        if (char(buffer[i]) >= 97 ) buffer[i] = (char)(char(buffer[i] - 32));
    }
    return String(buffer);
}

String CharArray_to_StringHEX(const char *CharArray_value, uint CharArray_length) {
    String strgHEX = "";
    for (size_t i = 0; i < CharArray_length; i++)
    {
        strgHEX +=  String(CharArray_value[i], HEX);
    }
    return strgHEX;
}

uint32_t calculateCRC32( const uint8_t *data, size_t length ) {
  uint32_t crc = 0xffffffff;
  while( length-- ) {
    uint8_t c = *data++;
    for( uint32_t i = 0x80; i > 0; i >>= 1 ) {
      bool bit = crc & 0x80000000;
      if( c & i ) {
        bit = !bit;
      }

      crc <<= 1;
      if( bit ) {
        crc ^= 0x04c11db7;
      }
    }
  }

  return crc;
}

void keep_IP_address() {
if (config.DHCP) {
  if (  WiFi.status() == WL_CONNECTED && IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3]) != WiFi.localIP() ) {
    if (config.DEBUG) {Serial.print("OLD: "); Serial.print(IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3])); Serial.print("\t NEW: "); Serial.println(WiFi.localIP());}
    config.IP[0] = WiFi.localIP()[0]; config.IP[1] = WiFi.localIP()[1]; config.IP[2] = WiFi.localIP()[2]; config.IP[3] = WiFi.localIP()[3];
    config.Netmask[0] = WiFi.subnetMask()[0]; config.Netmask[1] = WiFi.subnetMask()[1]; config.Netmask[2] = WiFi.subnetMask()[2]; config.Netmask[3] = WiFi.subnetMask()[3];
    config.Gateway[0] = WiFi.gatewayIP()[0]; config.Gateway[1] = WiFi.gatewayIP()[1]; config.Gateway[2] = WiFi.gatewayIP()[2]; config.Gateway[3] = WiFi.gatewayIP()[3];
    config.DNS_IP[0] = WiFi.dnsIP()[0]; config.DNS_IP[1] = WiFi.dnsIP()[1]; config.DNS_IP[2] = WiFi.dnsIP()[2]; config.DNS_IP[3] = WiFi.dnsIP()[3];
    if (config.DEBUG) Serial.println("Storing new Static IP address for quick boot");
    storage_write();
    }
}
}

void myconfigTime(const char* tz, const char* server1, const char* server2, const char* server3) {
    configTzTime(tz, server1, server2, server3);
}

void esp_wifi_disconnect() {
    WiFi.mode(WIFI_MODE_NULL);
}


void esp_bt_connect() {
    btStart();
}

void esp_bt_disconnect() {
    btStop();
}

bool esp_bt_check() {
    return btStarted();
}

uint8_t wifi_waitForConnectResult(unsigned long timeout) {
    return WiFi.waitForConnectResult(timeout);
}

bool RTC_read()  {return false;}
bool RTC_write() {return true;}
bool RTC_reset() {return true;}
void FormatConfig() {}


/*
// Read RTC memory (where the Wifi data is stored)
bool RTC_read() {
    if (ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
        // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
        uint32_t crc = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
        if( crc == rtcData.crc32 ) {
            if (config.DEBUG) {
             Serial.print("Read  crc: " + String(rtcData.crc32) + "\t");
             Serial.print("Read  BSSID: " + CharArray_to_StringHEX((const char*)rtcData.bssid, sizeof(rtcData.bssid)) + "\t");
             Serial.print("Read  LastWiFiChannel: " + String(rtcData.LastWiFiChannel) + "\t");
             Serial.println("Read  Last Date: " + String(rtcData.lastUTCTime));
            }
             return true;
        }
    }
    return false;
}

bool RTC_write() {
// Update rtcData structure
    rtcData.LastWiFiChannel = WiFi.channel();
    memcpy( rtcData.bssid, WiFi.BSSID(), 6 ); // Copy 6 bytes of BSSID (AP's MAC address)
    rtcData.crc32 = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
    //rtcData.lastUTCTime = curUnixTime();
    if (config.DEBUG) {
    Serial.print("Write crc: " + String(rtcData.crc32) + "\t");
    Serial.print("Write BSSID: " + CharArray_to_StringHEX((const char*)rtcData.bssid, sizeof(rtcData.bssid)) + "\t");
    Serial.print("Write LastWiFiChannel: " + String(rtcData.LastWiFiChannel) + "\t");
    Serial.println("Write Last Date: " + String(rtcData.lastUTCTime));
    }

    // Write rtcData back to RTC memory
    if (ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) return true;
    else return false;
}

bool RTC_reset() {
// Update rtcData structure
    rtcData.LastWiFiChannel = 0;
    memcpy( rtcData.bssid, "000000", 6 ); // Copy 6 bytes of BSSID (AP's MAC address)
    rtcData.crc32 = 0;
    rtcData.lastUTCTime = 0;

    // Write rtcData back to RTC memory
    if (ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) return true;
    else return false;
}
*/

// ESP32
void esp_deepsleep(unsigned long Time_seconds = 0, unsigned long currUTime = 0) {
    // https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/deepsleep.html
    uint64_t calculate_sleeptime;
  // Store counter to the Preferences
    preferences.putULong("UTCTime", currUTime);
    keep_IP_address();

  // Close the Preferences
    preferences.end();

  // Configure Wake Up
#if Ext1WakeUP >= 0
    #ifdef ESP32C3
        if ( Ext1WakeUP>=0 && (Time_seconds == 0 || Time_seconds > 300) ) {
            const uint64_t ext1_wakeup_pin_1_mask = 1ULL << Ext1WakeUP;      // -1 Warning during compilling
            esp_deep_sleep_enable_gpio_wakeup(ext1_wakeup_pin_1_mask, ESP_GPIO_WAKEUP_GPIO_LOW);  //ESP_GPIO_WAKEUP_GPIO_LOW , ESP_GPIO_WAKEUP_GPIO_HIGH
        }
    #else
        if ( Ext1WakeUP>=0 && (Time_seconds == 0 || Time_seconds > 300 ) ) {

            esp_sleep_enable_ext0_wakeup(gpio_num_t(Ext1WakeUP), !Ext1WakeUP_OFF); // Ext1WakeUP_OFF = LOW or HIGH
/*
FOR MULTIPLE PINS WakeUP use:            
            const uint64_t ext1_wakeup_pin_1_mask = 1ULL << Ext1WakeUP;      // -1 Warning during compilling
            //const uint64_t ext1_wakeup_pin_1_mask = Ext1WakeUP;
            esp_sleep_enable_ext1_wakeup(ext1_wakeup_pin_1_mask, ESP_EXT1_WAKEUP_ALL_LOW);      // use ONE or THE OTHER!!
            // esp_sleep_enable_ext1_wakeup(ext1_wakeup_pin_1_mask, ESP_EXT1_WAKEUP_ANY_HIGH);  // use ONE or THE OTHER!!
    //  Example using two PINs for external Wake UP 
    //      const int ext_wakeup_pin_1 = 2;
    //      const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;
    //      const int ext_wakeup_pin_2 = 4;
    //      const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;
    //      printf("Enabling EXT1 wakeup on pins GPIO%d, GPIO%d\n", ext_wakeup_pin_1, ext_wakeup_pin_2);
    //      esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask | ext_wakeup_pin_2_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
*/
        }
    #endif
#endif

#ifdef ULP_Support
    if (config.HW_Module) {
        if(config.DEBUG) Serial.println("Enabling ULP during deepsleep");
        ulp_action(1000000);                                       // 10 second loop
    }
#endif

    if (Time_seconds > 0) {
        if (millis() < (Time_seconds * 1000UL)) {
        calculate_sleeptime = uint64_t( ((Time_seconds * 1000UL) - millis()%(Time_seconds * 1000UL)) ) * 1000ULL;
        //Serial.printf("calculate_sleeptime :%llu\n", calculate_sleeptime);
        }
        else calculate_sleeptime = uint64_t(Time_seconds * 1000000UL);
        esp_sleep_enable_timer_wakeup(calculate_sleeptime);  // time in minutes converted to microseconds
    }
    esp_deep_sleep_start();
}


float ReadVoltage(){
    double reading = analogRead(Batt_ADC_PIN); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
    float weighted_reading = 0;
    if(reading < 1 || reading > 4095) return -1;
    //weighted_reading = return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
    weighted_reading = -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
    // Added an improved polynomial, use either, comment out as required
    // https://github.com/G6EJD/ESP32-ADC-Accuracy-Improvement-function/blob/master/ESP32_ADC_Read_Voltage_Accurate.ino
    if(Batt_Res_Div) return weighted_reading * Batt_Res_High / Batt_Res_Lower + weighted_reading;
    else return weighted_reading;
}

long getRSSI() {
    // return WiFi RSSI Strength signal [dBm]
    long r = 0;

    for(int i = 0; i < Number_of_measures; i++) {
        r += WiFi.RSSI();
    }
    r = r /Number_of_measures;
    return r;
}


void ESPRestart() {
    if (config.DEBUG) Serial.println("Restarting ESP...");
    delay(100);
    esp_restart();
}

static const String RESET_REASON_to_string[] = {            // RESET_REASON
    "NO_MEAN",                 /**<=  0, OK*/
    "POWERON_RESET",           /**<=  1, Vbat power on reset*/
    "SW_RESET",                /**<=  3, Software reset digital core*/
    "OWDT_RESET",              /**<=  4, Legacy watch dog reset digital core*/
    "Deep-Sleep Wake",         /**<=  5, Deep Sleep reset digital core  original msg -> "DEEPSLEEP_RESET"*/
    "Deep-Sleep Wake",         /**<=  "SDIO_RESET" 6, Reset by SLC module, reset digital core*/  
    "TG0WDT_SYS_RESET",        /**<=  7, Timer Group0 Watch dog reset digital core*/
    "TG1WDT_SYS_RESET",        /**<=  8, Timer Group1 Watch dog reset digital core*/
    "RTCWDT_SYS_RESET",        /**<=  9, RTC Watch dog Reset digital core*/
    "INTRUSION_RESET",         /**<= 10, Instrusion tested to reset CPU*/
    "TGWDT_CPU_RESET",         /**<= 11, Time Group reset CPU*/
    "SW_CPU_RESET",            /**<= 12, Software reset CPU*/
    "RTCWDT_CPU_RESET",        /**<= 13, RTC Watch dog Reset CPU*/
    "EXT_CPU_RESET",           /**<= 14, for APP CPU, reseted by PRO CPU*/
    "RTCWDT_BROWN_OUT_RESET",  /**<= 15, Reset when the vdd voltage is not stable*/
    "RTCWDT_RTC_RESET"         /**<= 16  RTC Watch dog reset digital core and rtc module*/
};

String ESPResetReason() {                                   // RESET_REASON
  return RESET_REASON_to_string[rtc_get_reset_reason(0)];
}

static const String SLEEP_WAKEUP_to_string[] = {            // esp_sleep_wakeup_t
    "ESP_SLEEP_WAKEUP_UNDEFINED",       //!< In case of deep sleep, reset was not caused by exit from deep sleep
    "ESP_SLEEP_WAKEUP_ALL",             //!< Not a wakeup cause, used to disable all wakeup sources with esp_sleep_disable_wakeup_source
    "ESP_SLEEP_WAKEUP_EXT0",            //!< Wakeup caused by external signal using RTC_IO
    "ESP_SLEEP_WAKEUP_EXT1",            //!< Wakeup caused by external signal using RTC_CNTL
    "ESP_SLEEP_WAKEUP_TIMER",           //!< Wakeup caused by timer
    "ESP_SLEEP_WAKEUP_TOUCHPAD",        //!< Wakeup caused by touchpad
    "ESP_SLEEP_WAKEUP_ULP",             //!< Wakeup caused by ULP program
    "ESP_SLEEP_WAKEUP_GPIO",            //!< Wakeup caused by GPIO (light sleep only on ESP32, S2 and S3)
    "ESP_SLEEP_WAKEUP_UART",            //!< Wakeup caused by UART (light sleep only)
    "ESP_SLEEP_WAKEUP_WIFI",            //!< Wakeup caused by WIFI (light sleep only)
    "ESP_SLEEP_WAKEUP_COCPU",           //!< Wakeup caused by COCPU int
    "ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG", //!< Wakeup caused by COCPU crash
    "ESP_SLEEP_WAKEUP_BT",              //!< Wakeup caused by BT (light sleep only)
};

String SleepWakeUpReason() {                                  // esp_sleep_wakeup_t
    return SLEEP_WAKEUP_to_string[esp_sleep_get_wakeup_cause()];
}

String Flash_Size() {
    return String(ESP.getFlashChipSize()/1048576) + "MB";   // value in bytes converted to MB (1024 * 1024)
}

uint32_t CPU_Clock() {
    return getCpuFrequencyMhz();
}

void CPU_Boost(bool boost = true) {
    if(boost) setCpuFrequencyMhz(240U);
    else setCpuFrequencyMhz(80U);
}

/*
void FormatConfig() {                                   // WARNING!! To be used only as last resource!!!
    Serial.println(ESP.eraseConfig());
    RTC_reset();
    delay(100);
    ESP.reset();
}
*/

void hw_setup() {
  // Initiate Power Management CHIP      
    #ifdef IP5306
        ip5306_setup();
        setPowerBoostKeepOn(true);
        Serial.print("IP5306 --> Can control: "  + String(canControl()));
        setCharge(true);
        Serial.println("  -  Batt charging: "  + String(isCharging()) + "  ... and/or fully Charged: "  + String(isChargeFull()));
    #endif


  // ADC setup
  // Attenuation  input range:  ESP32           ESP32-C3        
  // ADC_0db                    100 ~ 950 mV    0 ~ 750 mV
  // ADC_2_5db                  100 ~ 1250 mV   0 ~ 1050 mV
  // ADC_6db                    150 ~ 1750 mV   0 ~ 1300 mV
  // ADC_11db                   150 ~ 2450 mV   0 ~ 2500 mV
  // https://docs.espressif.com/projects/esp-idf/en/v4.4.3/esp32/api-reference/peripherals/adc.html
  // https://docs.espressif.com/projects/esp-idf/en/v4.4.2/esp32c3/api-reference/peripherals/adc.html
    if (Batt_ADC_PIN>=0) {
        analogSetPinAttenuation(Batt_ADC_PIN,ADC_11db);   // ADC_11db provides an attenuation so that IN/OUT = 1 / 3.6.
                                                          // An input of 3 volts is reduced to 0.833 volts before ADC measurement
        adcAttachPin(Batt_ADC_PIN);                       // Pin defined in def_conf.h
    }
  // Disable BT (most of project won't use it) to save battery.
  //  esp_bt_controller_disable();

  // Non.Volatile Memory
    preferences.begin("my-app", false);
    Serial.println("Stored UTCTime: " + String(preferences.getULong("UTCTime")));
}

void hw_loop() {
  // LED handling usefull if you need to identify the unit from many
    //if (LED_ESP>=0) digitalWrite(LED_ESP, boolean(config.LED));
    yield();
}
