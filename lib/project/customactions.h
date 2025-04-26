// Function to insert customized MQTT Configuration actions
void custom_mqtt(String command, String cmd_value) {
    if ( command == "BckpRstr" ) {
        if (cmd_value == "") telnet_println("Restore data is empty");
        else {
            DeserializationError error = deserializeJson(config_doc, cmd_value); //Deserialize string to JSON data
            if (error) {telnet_print("JSON DeSerialization failure: "); telnet_println(error.f_str());}
            else {
                strcpy(config.DeviceName, config_doc["DeviceName"]);
                strcpy(config.Location, config_doc["Location"]);
/*
                config.LOWER_LEVEL =    config_doc["LOWER_Pos"];
                config.UPPER_LEVEL =    config_doc["UPPER_Pos"];
*/
                storage_write();
                bckp_rstr_flag = true;
                telnet_println("BckpRstr with success");
            }
        }
    }

    if ( command == "EFX") EFX = EFX_index(cmd_value);
    if ( command == "Gain") { GAIN = (byte)(cmd_value.toInt()); telnet_println("New GAIN: " + String(GAIN));}
    if ( command == "Color")  HARGB_to_color(cmd_value);
    if ( command == "Light") {
        if ( Light_Last == bool(cmd_value.toInt()) ) mqtt_publish(mqtt_pathtele, "Light", String(Light));
        else Light = bool(cmd_value.toInt());
    }
//  if ( command == "I2C_scan") if (bool(cmd_value.toInt()) == true) I2C_scan();  // ambient.h
//  if ( command == "send_Telemetry" && bool(cmd_value.toInt())) { gps_update(); print_gps_data(); send_Telemetry(); }

}

void custom_update(){
    yield();
    mqtt_publish(mqtt_pathtele, "DEEPSLEEP", String(config.DEEPSLEEP));
    mqtt_publish(mqtt_pathtele, "SLEEPTime", String(config.SLEEPTime));
    mqtt_publish(mqtt_pathtele, "ONTime", String(config.ONTime));
    //mqtt_publish(mqtt_pathtele, "Switch_Def", String(config.SWITCH_Default));
    //ambient_data();
    //mqtt_dump_data(mqtt_pathtele, "Telemetry");
    telnet_println("Color: " + String(Color) + " : GAIN: " + String(GAIN) + " : EFX: " + String(EFX));
    mqtt_publish(mqtt_pathtele, "Light", String(Light));
    mqtt_publish(mqtt_pathtele, "Color", String(Color));
    mqtt_publish(mqtt_pathtele, "GAIN", String(GAIN));
    mqtt_publish(mqtt_pathtele, "EFX", EFXName[EFX]);
}
