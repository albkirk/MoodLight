/*      H O W  T O   U S E   T H I S   L I B R A R Y ?
-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
Add these 3 lines to your main.cpp file:
// -- LED Lights GPIO & Configuration
//#define LED_RGB                                         // Uncoment to be used by color.h library
//#define LED_NEO                                         // Uncoment to be used by color.h library
#define PIN_RED             -1                            // PWM Output PIN for RED  -1 means NOT used!
#define PIN_GREEN           -1                            // PWM Output PIN for GREEN  -1 means NOT used!
#define PIN_BLUE            -1                            // PWM Output PIN for BLUE  -1 means NOT used!
#define NEOPixelsPIN        -1                            // NeoPixels DATA GPIO pin.
#define NEOPixelsNUM        -1                            // Number of NeoPixels LEDs attached
#define NEO_BRIGHTNESS      50                            // NEO brightness [0-255]
#include <color.h>

On SETUP function add this line:
color_setup();

On LOOP function add this line:
color_loop();

Change default color by changing the value of config.color
Change Light brightness by changing the value of: GAIN

HASSIO
Add these lines to your custohassio.h file:
void custo_hassio_disc(){
    config_entity("light","None","Light","RGB");

void custo_hassio_del(){
    delete_entity("light","None","Light","RGB");

And add these lines to your customactions.h file:
void custom_mqtt(...
    if ( command == "EFX") EFX = EFX_index(cmd_value);
    if ( command == "Gain") { GAIN = (byte)(cmd_value.toInt()); telnet_println("New GAIN: " + String(GAIN));}
    if ( command == "Color")  HARGB_to_color(cmd_value);
    if ( command == "Light") {
        if ( Light_Last == bool(cmd_value.toInt()) ) mqtt_publish(mqtt_pathtele, "Light", String(Light));
        else Light = bool(cmd_value.toInt());
    }

void custom_update(){
    telnet_println("Color: " + String(Color) + " : GAIN: " + String(GAIN) + " : EFX: " + String(EFX));
    mqtt_publish(mqtt_pathtele, "Light", String(Light));
    mqtt_publish(mqtt_pathtele, "Color", String(Color));
    mqtt_publish(mqtt_pathtele, "GAIN", String(GAIN));
    mqtt_publish(mqtt_pathtele, "EFX", EFXName[EFX]);
-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
*/
#ifndef NEOPixelsNUM
    #define NEOPixelsNUM 1
#endif

#ifdef LED_NEO
    #include <Adafruit_NeoPixel.h>
    //#include <FastLED.h>
    //#include <NeoPixelBus.h>
#endif

// CONSTANTs
#ifndef LED_NEO
    #ifdef ESP32
        #define RESOLUTION           8          // Duty cycle bits (8 -> 255) equalto FF
        #define PWM_FREQ          1000          // PWM Frequency
        #define CHANNELR             0
        #define CHANNELG             1
        #define CHANNELB             2
    #else   // is ESP8266
        #define PWM_RANGE 255                   // Duty cycle value 255 equalto FF
        #define PWM_FREQ 250                    // PWM Frequency
    #endif
#endif

    char BLACK[8] =        "#000000";       // RGB code for BLACK or LED Strip OFF
    char GRAY[8] =         "#151515";       // RGB code for Dark GRAY
    char WHITE[8] =        "#FFFFFF";       // RGB code for WHITE
    char RED[8] =          "#FF0000";       // RGB code for RED
    char GREEN[8] =        "#00FF00";       // RGB code for GREEN
    char BLUE[8] =         "#0000FF";       // RGB code for BLUE
    char ORANGE[8] =       "#FF4B00";       // RGB code for ORANGE
    char LIGHT_GREEN[8] =  "#90EE90";       // RGB code for LIGHT GREEN
    char DEEP_BLUE[8] =    "#040260";       // RGB code for Deep BLUE
    char WARM_WHITE[8] =   "#FF930F";       // RGB code for ----------
    char MILK_WHITE[8] =   "#FFF0F0";       // RGB code for SNOW
    char DEEP_YELLOW[8] =  "#FFD700";       // RGB code for GOLD
    char CYAN[8] =         "#00FFFF";       // RGB code for CYAN
    char BLUE_PURPLE[8] =  "#9370DB";       // RGB code for MEDIUM PURPLE
    char PINK_WHITE[8] =   "#FFC0CB";       // RGB code for PINK
    char YELLOW[8] =       "#FFFF00";       // RGB code for YELLOW
    char LIGHT_BLUE[8] =   "#ADD8E6";       // RGB code for LIGHT BLUE
    char INDIGO[8] =       "#4B0082";       // RGB code for PURPLE
    char PURPLE[8] =       "#800080";       // RGB code for PURPLE
    char GREEN_WHITE[8] =  "#E0FFE0";       // RGB code for HONEYDEW
    char LIGHT_YELLOW[8] = "#FFFFD0";       // RGB code for LIGHT YELLOW
    char SKY_BLUE[8] =     "#87CEEB";       // RGB code for SKY BLUE
    char BROWN[8] =        "#584125";       // RGB code for BROWN
    char BLUE_WHITE[8] =   "#E0E0FF";       // RGB code for ALICE BLUE

    // Rainbow colors: red, orange, yellow, green, blue, indigo, and violet
    char *rainbow[] = {ORANGE, YELLOW, GREEN, BLUE, INDIGO, PURPLE, RED};
    char *nature[] = {GREEN, LIGHT_GREEN, YELLOW, SKY_BLUE, GREEN_WHITE, LIGHT_BLUE, BLUE_WHITE};
    //char *sad[] = {DEEP_BLUE, BLACK, BROWN, BLACK, GRAY, BLACK};
    char *happy[] = {YELLOW, GREEN, BLUE, CYAN, PURPLE, RED, ORANGE};
    char *inspired[] = {PURPLE, RED, ORANGE, YELLOW, CYAN, BLUE_PURPLE, BLUE};
    char *relax[] = {PINK_WHITE, BLUE_WHITE, LIGHT_BLUE, LIGHT_YELLOW, GREEN_WHITE, WARM_WHITE};
    char *hot[] = {RED, ORANGE, PINK_WHITE, BROWN};
    char *cold[] = {CYAN, DEEP_BLUE, LIGHT_GREEN, LIGHT_BLUE, BLUE_WHITE, BLUE_PURPLE, GREEN_WHITE};
    char **profiles[] = {rainbow, nature, happy, inspired, relax, hot, cold}; // Define all profiles
    byte num_profiles = 7;        // The number of profiles in the list (must be manually counted)
    const String profile_names[] = {"Rainbow", "Nature", "Happy", "Inspired", "Relax", "Hot", "Cold"}; // Define all profiles
    byte profileSizes[] = {
        sizeof(rainbow) / 4,    // rainbow is the address of the first element, which is 4 bytes (32bits) long
        sizeof(nature) / 4,
        sizeof(happy) / 4,
        sizeof(inspired) / 4,
        sizeof(relax) / 4,
        sizeof(hot) / 4,
        sizeof(cold) / 4
};


//
// VARIABLES
//
    char Color[8]     = "#000000";  // RGB Color code. syntax: '#' + RED + GREEN + BLUE
    char LastColor[8] = "#000000";  // each param use 2 CHARs and range from 0 to FF (HEX format of 0-255).
    char LastNBlack[8]= "#000000";  // To track the Last NON Black color used
    byte RGB[3];                    // RRB trio byte Values to control the OUTPut PWM PINs
    byte neoID = 0;                 // NEO Pixel ID [0 - n] to change color
    byte zeros[3] = {0, 0, 0};      // trio of byte Value 0 to represnt Black color
    char Xval[2];                   // Buffer memory to convert byte into double HEX char
    char *Xarr;
    long RANGE[] = {15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207, 223, 239, 255};
    byte GAIN = 255;                // Color gain 0 to 255
    bool Light = false;             // [OFF / ON] Light switch
    bool Light_Last = false;        // [OFF / ON] Light switch  (Last state)
// EFX
    //const String EFXName[] = {"NoEFX", "Auto", "Flash", "Fade3", "Fade7", "Scan", "Rainbow"};
    const String EFXName[] = {"NoEFX", "Auto", "Rainbow", "Nature", "Happy", "Inspired", "Relax", "Hot", "Cold"};
    byte sizeof_EFXName = 9;        // The number of EFX in the list (must be manually counted)
    byte EFX = 0;                   // The EFX to be played
    byte EFX_Last = 0;              // The Last EFX to be played
    byte EFX_RGB[3] = {0, 0, 0};    // Effects RGB trio byte values used during Effects
    int  RGB_idx=0;                 // RGB array index
    long EFX_Delta=-2;              // RGB delta value change (-2 or +2)
    byte EFX_GAIN = 255;            // Effect Color gain 0 to 255
    uint EFX_Interval=1500;         // Effect interval timer in milliseconds
    byte GAIN_Last = 0;             // Last GAIN value
    uint Last_T_EFX=0;              // last time Effect status was changed
    uint Last_T_Flash=0;            // last time Flash status was changed
    bool Bool_flash=false;          // Boolean to toggle flash status
    byte RainBow_idx = 0;           // Raibow color index

 #ifdef LED_NEO
    Adafruit_NeoPixel *pixels;
#endif

//
//  AUXILIAR functions
//
byte CharHEX_to_byte(char X) {
  byte num;
  if (byte(X) >= 48 && byte(X) <= 57) num = byte(X)- 48;   // 48 ='0' and 57 = '9'
  else if (byte(X) >= 65 && byte(X) <= 70) num = byte(X) - 65 + 10;   // 65 = 'A'  and 70 ='F'
  else if (byte(X) >= 97 && byte(X) <= 102) num = byte(X) - 97 + 10;   // 97 = 'a'  and 102 ='f'
  else num = 0;
  //Serial.print(" ->" + String(num));
  return num;
}


char *byte_to_CharHEX(byte Bval, char arr[2]) {
  if (Bval/16 >= 0 && Bval/16 <= 9) arr[0] =char(48 + Bval/16);   // 48 ='0' and 57 = '9'
  else if (Bval/16 >= 10 && Bval/16 <= 15) arr[0] = char(65 + Bval/16 - 10);   // 65 = 'A'  and 70 ='F'
  if (Bval%16 >= 0 && Bval%16 <= 9) arr[1] = char(48 + Bval%16);   // 48 ='0' and 57 = '9'
  else if (Bval%16 >= 10 && Bval%16 <= 15) arr[1] = char(65 + Bval%16 - 10);   // 65 = 'A'  and 70 ='F'
  return arr;

  // then you want to use it.
//Xarr = byte_to_CharHEX(255, Xval);
}

bool barraycpy (byte *arrout, byte *arrin) {
  if (sizeof(arrin) > sizeof(arrout)) return false;
  for (size_t i = 0; i < 3; i++) {
    arrout[i] = arrin[i];
  }
  return true;
}

String RGB_to_Color(byte rgb[3]) {
    char arr[2];
    char color[8] = "#000000";
    byte_to_CharHEX(rgb[0], arr); color[1] = arr[0]; color[2] = arr[1];
    byte_to_CharHEX(rgb[1], arr); color[3] = arr[0]; color[4] = arr[1];
    byte_to_CharHEX(rgb[2], arr); color[5] = arr[0]; color[6] = arr[1];
    return String(color);
}

bool color_set_EFX_RGB (char color_code[8]) {
    if (color_code[0] == '#') {
        EFX_RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
        //telnet_println(String(byte_to_CharHEX(RGB[0], Xval)));
        EFX_RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
        EFX_RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
        return true;
    }
    else return false;
}

byte matrix_to_idx(byte x, byte y, byte width = 8) {
    // Convert 2D matrix coordinates to 1D index
    return (y * width) + x; // Assuming a matrix of 'width' columns
}

void idx_to_matrix(byte idx, byte &x, byte &y, byte width = 8) {
    // Convert 1D index to 2D matrix coordinates
    x = idx % width;        // Assuming a matrix of 'width' columns
    y = idx / width;
}

byte EFX_index(String efxname) {
    //this functions return the index position which the input string matches to the EFXName string array
    for (byte i = 0; i < sizeof_EFXName; i++) {
        if (efxname == EFXName[i]) {
            return i;
        }
    }
    return 0; // Return 0 if no match found
}

byte color_gain (byte Cval, byte Gval) {
   return (byte) (Cval * Gval/255.0);
}

void gain_change(byte cgain, char color_code[8] = Color){       // change Gain to Color
    char arr[2];
    RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
    RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
    RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
    RGB[0] = color_gain(RGB[0], cgain);
    RGB[1] = color_gain(RGB[1], cgain);
    RGB[2] = color_gain(RGB[2], cgain);
    byte_to_CharHEX(RGB[0], arr); Color[1] = arr[0]; Color[2] = arr[1];
    byte_to_CharHEX(RGB[1], arr); Color[3] = arr[0]; Color[4] = arr[1];
    byte_to_CharHEX(RGB[2], arr); Color[5] = arr[0]; Color[6] = arr[1];
    //telnet_println("Color: " + String(Color) );
}

void color_change(int C_RED = 0, int C_GREEN = 0, int C_BLUE = 0, char color_code[8] = Color){       // change R, G or B to Color
    char arr[2];
    RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
    //telnet_println(String(byte_to_CharHEX(RGB[0], Xval)));
    RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
    RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
    if (int(RGB[0])+ C_RED >= 0   && int(RGB[0])+ C_RED   <= 255 ) RGB[0] = byte(int(RGB[0]) + C_RED);
    if (int(RGB[1])+ C_GREEN >= 0 && int(RGB[1])+ C_GREEN <= 255 ) RGB[1] = byte(int(RGB[1]) + C_GREEN);
    if (int(RGB[2])+ C_BLUE >= 0  && int(RGB[2])+ C_BLUE  <= 255 ) RGB[2] = byte(int(RGB[2]) + C_BLUE);
    byte_to_CharHEX(RGB[0], arr); Color[1] = arr[0]; Color[2] = arr[1];
    byte_to_CharHEX(RGB[1], arr); Color[3] = arr[0]; Color[4] = arr[1];
    byte_to_CharHEX(RGB[2], arr); Color[5] = arr[0]; Color[6] = arr[1];
    //telnet_println("Color: " + String(Color) );
}

void HARGB_to_color(String RGBstring, char color_code[8] = Color){       // converts RGB string 100,120,23 to color array
    char arr[2];
    int r, g, b;
    // Use sscanf to extract RGB values from the input string
    sscanf(RGBstring.c_str(), "%d,%d,%d", &r, &g, &b);
    // Ensure that the RGB values are within the valid range (0-255)
    r = constrain(r, 0, 255);
    g = constrain(g, 0, 255);
    b = constrain(b, 0, 255);
    RGB[0] = byte(r);
    RGB[1] = byte(g);
    RGB[2] = byte(b);
    byte_to_CharHEX(RGB[0], arr); Color[1] = arr[0]; Color[2] = arr[1];
    byte_to_CharHEX(RGB[1], arr); Color[3] = arr[0]; Color[4] = arr[1];
    byte_to_CharHEX(RGB[2], arr); Color[5] = arr[0]; Color[6] = arr[1];
    telnet_println("Color: " + String(Color) );
}

void color_paint (byte PAINT[3], byte PGain = GAIN, byte nID = 254) {  // byte PGain = GAIN || 254
#ifndef LED_NEO
    #ifdef ESP32
        ledcWrite(CHANNELR, color_gain(PAINT[0], PGain));                     // OUTPUT RED level
        ledcWrite(CHANNELG, color_gain(PAINT[1], (byte)(PGain*CALIBRATE)));   // OUTPUT GREEN level
        ledcWrite(CHANNELB, color_gain(PAINT[2], (byte)(PGain*CALIBRATE)));   // OUTPUT BLUE level
    #else   //is ESP8266
        analogWrite(PIN_RED,   color_gain(PAINT[0], PGain));                  // OUTPUT RED level
        analogWrite(PIN_GREEN, color_gain(PAINT[1], (byte)(PGain*CALIBRATE)));// OUTPUT GREEN level
        analogWrite(PIN_BLUE,  color_gain(PAINT[2], (byte)(PGain*CALIBRATE)));// OUTPUT BLUE level
    #endif
#else
    if(nID > NEOPixelsNUM) {
        for (size_t i = 0; i < NEOPixelsNUM; i++) {
            pixels->setPixelColor(i, pixels->Color(color_gain(PAINT[0], PGain), color_gain(PAINT[1], PGain), color_gain(PAINT[2], PGain)));
        }       
    }
    else {
        pixels->setPixelColor(nID, pixels->Color(color_gain(PAINT[0], PGain), color_gain(PAINT[1], PGain), color_gain(PAINT[2], PGain)));

    }
    pixels->show(); // This sends the updated pixel color to the hardware. 
    
#endif
}

bool color_set (char color_code[8], byte nID = 254) {
    if (color_code[0] == '#') {
        RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
        //telnet_println(String(byte_to_CharHEX(RGB[0], Xval)));
        RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
        RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
        //telnet_println(" R=" + String(RGB[0]) + " G=" + String(RGB[1]) + " B=" + String(RGB[2]));
        //telnet_println(String(Color) + " : " + String(color_code) + " : " + String(LastColor));
        color_paint(RGB, GAIN, nID);

 //     strcpy(LastColor,color_code);
        barraycpy(EFX_RGB, RGB);
        EFX_GAIN = GAIN;
        return true;
    }
    else return false;
}

// Color Effect functions
void color_Auto() {
    if ( millis() - Last_T_EFX > EFX_Interval/10) {
      int idx = random(0,3);
      int deviate = random(-25,25);
      int npx = random(0,NEOPixelsNUM);
      if (EFX_RGB[idx] + deviate >=0 && EFX_RGB[idx] + deviate <=255) {
          EFX_RGB[idx] += deviate ;
          color_paint(EFX_RGB, EFX_GAIN, npx);
      }
      Last_T_EFX = millis();
    }
}

void color_Flash() {
    if ( millis() - Last_T_Flash > EFX_Interval/4) {
        if (Bool_flash) {
            EFX_GAIN = 0;
            color_paint(EFX_RGB, EFX_GAIN);
        }
        else {
            EFX_GAIN = GAIN;
            color_paint(EFX_RGB, EFX_GAIN);
        }
        Bool_flash = !Bool_flash;
        Last_T_Flash = millis();
    }
}

void color_Fade3() {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
      if (EFX_RGB[RGB_idx] + EFX_Delta >=0 && EFX_RGB[RGB_idx] + EFX_Delta <=RGB[RGB_idx]) {
          EFX_RGB[RGB_idx] += EFX_Delta;
          color_paint(EFX_RGB, EFX_GAIN);
      }
      else {
        if (EFX_Delta <0) RGB_idx = (RGB_idx +1) % 3;
        EFX_Delta = - EFX_Delta;
      }
      Last_T_EFX = millis();
    }
}

void color_Fade7() {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
      if (EFX_RGB[RGB_idx] + EFX_Delta >=0 && EFX_RGB[RGB_idx] + EFX_Delta <=RGB[RGB_idx]) {
          EFX_RGB[RGB_idx] += EFX_Delta;
          color_paint(EFX_RGB, EFX_GAIN);
      }
      else {
        RGB_idx = (RGB_idx +1) % 3;
        EFX_Delta = - EFX_Delta;
      }
      Last_T_EFX = millis();
    }
}

void color_scan() {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
      color_set(BLACK, neoID);
      RainBow_idx = (RainBow_idx + 1) % (sizeof(rainbow)/sizeof(*rainbow));
      neoID  = (neoID + 1) % NEOPixelsNUM;
      for (size_t i = 0; i < int(NEOPixelsNUM/16); i++) {
        color_set(rainbow[RainBow_idx], neoID + i);
      }
      Last_T_EFX = millis();
    }
}

void color_rainbow() {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
        //Serial.println("Color" + String(rainbow[RainBow_idx]) + "  NEO: " + neoID);
        RainBow_idx = (RainBow_idx + 1) % (sizeof(rainbow)/sizeof(*rainbow));
        for (size_t i = 0; i < int(NEOPixelsNUM); i++) {
          color_set(rainbow[RainBow_idx], i);
        }
        Last_T_EFX = millis();
    }
}

void color_profile(byte profileIndex = 0) {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
        if (RGB_to_Color(RGB) == RGB_to_Color(EFX_RGB)) {
            char **selectedProfile = profiles[profileIndex];
            RainBow_idx = (RainBow_idx + 1) % profileSizes[profileIndex];
            color_set_EFX_RGB(selectedProfile[RainBow_idx]);
        }
        else {
            int idx = 0;
            int min_diff = 255;
            // Find the index of the color channel with the minimum difference
            // between the current RGB value and the target EFX_RGB value
            for (int i = 0; i < 3; i++) {
                int diff = abs(RGB[i] - EFX_RGB[i]);
                if (diff > 0 && diff <= min_diff) {
                    min_diff = diff;
                    idx = i;
                }           
            }
            if (RGB[idx] < EFX_RGB[idx]) RGB[idx] += 1;
            else if (RGB[idx] > EFX_RGB[idx]) RGB[idx] -= 1;
            color_paint(RGB, EFX_GAIN);
        }
        Last_T_EFX = millis();
    }
}
void color_profile1(byte profileIndex = 0) {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
        char **selectedProfile = profiles[profileIndex];
        RainBow_idx = (RainBow_idx + 1) % profileSizes[profileIndex];
        color_set(selectedProfile[RainBow_idx]);
        Last_T_EFX = millis();
    }
}
//barraycpy(EFX_RGB, RGB);

void color_spiral (byte width = 8, byte height = 8) {
    // This function is used to create a spiral effect on the NeoPixels
    // It will iterate through the pixels in a spiral pattern and set their colors
    // You can customize the colors and speed of the spiral effect as needed
    int x, y;
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
        RainBow_idx = (RainBow_idx + 1) % (sizeof(rainbow)/sizeof(*rainbow));
        neoID = (neoID + 1) % NEOPixelsNUM; // Increment the pixel index
        // Set the color of the pixel at (x, y) in a spiral pattern
        x = (neoID + width/2) % width; // Assuming a matrix of 'width' columns
        y = (neoID - height/2) / height; // Assuming a matrix of 'height' rows
        // You can use any color you like here
        color_set(rainbow[RainBow_idx], matrix_to_idx(x, y));
        Last_T_EFX = millis();
    }
}


//
//  MAIN functions
//
void color_setup() {
    // ***********  Color SETUP
#ifndef LED_NEO
    #ifdef ESP32
        ledcSetup(CHANNELR, PWM_FREQ, RESOLUTION);  // Duty cycle Range of values [o-PWMRANGE] equal to FF
        ledcSetup(CHANNELG, PWM_FREQ, RESOLUTION);  // PWM Frequency 1000Hz
        ledcSetup(CHANNELB, PWM_FREQ, RESOLUTION);
        // attach the channel to the GPIO to be controlled
        ledcAttachPin(PIN_RED, CHANNELR);
        ledcAttachPin(PIN_GREEN, CHANNELG);
        ledcAttachPin(PIN_BLUE, CHANNELB);
    #else  // is 8266
        pinMode(PIN_RED,   OUTPUT);
        pinMode(PIN_GREEN, OUTPUT);
        pinMode(PIN_BLUE,  OUTPUT);
        analogWriteRange(PWM_RANGE);        // Duty cycle Range of values [o-PWMRANGE] equal to FF
        analogWriteFreq(PWM_FREQ);          // PWM Frequency 250Hz
    #endif
#else
    // Set NeoPixel configuration
    pixels = new Adafruit_NeoPixel(NEOPixelsNUM, NEOPixelsPIN, NEO_GRB + NEO_KHZ800);

    // Start NeoPixel library with all LEDs off
    pixels->begin();                     // This initializes the NeoPixel library.
    pixels->clear();                    // This clears the pixel colors (turns them off).
    pixels->setBrightness(NEO_BRIGHTNESS);
#endif

    for (size_t i = 0; i < NEOPixelsNUM; i++) color_set(BLACK, i);  // Initiate LED Strip turned off
    strcpy(LastNBlack,config.InitColor);    // Setup the initial color for the Light/Switch command
    delay(200);
    if (config.SWITCH_Default) {
        strcpy(Color,config.InitColor);     // for faster control of RGB, independent of WiFI/MQTT/system status
        Light = true;                       // Turning LED Strip to ON
        Light_Last = false;                 // and force publishing initial Light State
        for (size_t i = 0; i < NEOPixelsNUM; i++) color_set(Color, i);                   // Turning LED Strip to initial color (typicaly white...)
        strcpy(LastColor,BLACK);            // and force publishing initial color after Wifi and MQQT initialization
    }
}

  // Color commands to run on loop function.
void color_loop() {
    if (String(Color) != String(LastColor)) {
        if (String(Color) != String(BLACK)) {
            Light = true;
            strcpy(LastNBlack,Color);
        }
        else Light = false;
        if (color_set(Color)) {
            for (size_t i = 1; i < NEOPixelsNUM; i++) color_set(Color, i);
            mqtt_publish(mqtt_pathtele, "Color", String(Color));
        }
        strcpy(LastColor,Color);
    }
    if ( GAIN!= GAIN_Last) {
        if (color_set(Color)) {
            for (size_t i = 1; i < NEOPixelsNUM; i++) color_set(Color, i);
            mqtt_publish(mqtt_pathtele, "Gain", String(GAIN));
        }
        GAIN_Last = GAIN;
    }
    if (Light != Light_Last) {
        if (Light) { EFX = 0; strcpy(Color,LastNBlack); }
        else { EFX = 0; strcpy(Color,BLACK); }
        mqtt_publish(mqtt_pathtele, "Light", String(Light));
        //mqtt_publish(mqtt_pathtele, "Color", String(Color));
        //mqtt_publish(mqtt_pathtele, "Gain", String(GAIN));
        Light_Last = Light;
    }
    if (EFX != EFX_Last) {
         mqtt_publish(mqtt_pathtele, "EFX", EFXName[EFX]);
         EFX_Last = EFX;
    }
    // {"NoEFX", "Auto", "Rainbow", "Nature", "Happy", "Inspired", "Relax", "Hot", "Cold"}
    switch (EFX) {
        case 0:
            color_paint (RGB, GAIN);
            break;
        case 1:
            color_Auto();
            break;
        case 2:
            //color_Flash();
            color_profile(0);   // rainbow
            break;
        case 3:
            //color_Fade3();
            color_profile(1);   // nature
            break;
        case 4:
            //color_Fade7();
            color_profile(2);   // happy
            break;
        case 5:
            //color_scan();
            color_profile(3);   // inspired
            break;
        case 6:
            //color_rainbow();
            color_profile(4);   // relax
            break;
        case 7:
            color_profile(5);   // hot
            break;
        case 8:
            color_profile(6);   // cold
            break;
        default:
            EFX = 0;
    }
    yield();
}
