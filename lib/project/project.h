// WRITE HERE all the Project's logic / Flow.
// **** Project definitions here ...

// **** Libraries to include here ...



// **** Project code functions here ...
void project_setup() {
    // Check button states using getButtonByName

    EFX = 1;    // Set EFX to 1 -> Rainbow profile
}

void project_loop() {
    color_loop();

      // Button handling
      if (A_COUNT >= 1 && A_STATUS && (millis() - Last_A > 3000)) {
        mqtt_publish(mqtt_pathtele, "PushButton", "Holded");
        telnet_println("PushButton HOLDED for 3 seconds!!!");
        EFX = 0;
        NEO_BRIGHTNESS = 200;
        pixels->setBrightness(NEO_BRIGHTNESS);
        strcpy(Color, config.InitColor);
        A_COUNT = 0;
    }


    if (A_COUNT >= 1 && !A_STATUS && (millis() - Last_A > Butt_Interval)) {
        mqtt_publish(mqtt_pathtele, "PushButton", String(A_COUNT));
        flash_LED(A_COUNT);
        if (A_COUNT == 1) {
            EFX = (EFX +1) % sizeof_EFXName;
            color_set(BLACK);
            delay(500);
            RainBow_idx = 0;
            if (EFX == 0) {
                color_set(config.InitColor);
            }
            else if (EFX >=2) {
                char **selectedProfile = profiles[EFX-2];
                pixels->setBrightness(0);
                color_set(selectedProfile[0]);
                for (size_t i = 1; i <= NEO_BRIGHTNESS; i++) {
                    pixels->setBrightness(i);
                    color_set(selectedProfile[0]);
                    delay(20);
                }
            }
        }
        if (A_COUNT == 2) GoingToSleep(0);
        if (A_COUNT == 3) global_restart("Restarting");

        A_COUNT = 0;
    }
}

