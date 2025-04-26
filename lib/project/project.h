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
      if (A_COUNT >= 1 && A_STATUS && (millis() - Last_A > 5000)) {
        mqtt_publish(mqtt_pathtele, "PushButton", "Holded");
        telnet_println("PushButton HOLDED for 5 seconds!!!");
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
            if (EFX >=2) {
                char **selectedProfile = profiles[EFX-2];
                color_set(BLACK);
                delay(100);
                color_set(selectedProfile[0]);
                delay(500);
                color_set(BLACK);
            }
        }
        if (A_COUNT == 3) GoingToSleep(0);
        if (A_COUNT == 5) global_restart("Restarting");

        A_COUNT = 0;
    }
}

