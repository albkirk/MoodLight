//JSON Variables
char telemetry_jsonString[256];
DynamicJsonDocument telemetry_doc(256);


void send_Telemetry() {
    char fbat[3];               // long enough to hold complete floating string
    telemetry_doc.clear();      // Purge old JSON data and Load new values
    //telemetry_doc["Interval"] = config.SLEEPTime;
    telemetry_doc["Timestamp"] = curUnixTime();
    telemetry_doc["BatLevel"] = String(dtostrf(getBattLevel(),3,0,fbat)).toFloat();
    telemetry_doc["RSSI"] = getRSSI();
    #ifdef Ambient_Enabled
        ambient_get_data();
        //ambient_send_data();
        if(Temperature != -100) telemetry_doc["Temperature"] = String(Temperature, 1);
        if (Humidity != -1) {
            telemetry_doc["Humidity"] = String(Humidity);
            telemetry_doc["HumVelocity"] = String(HumVelocity, 1);     // Delta Humdity per minute
        }
        if (Lux != -1) telemetry_doc["Lux"] = String(Lux);
        if (Gas != -1) telemetry_doc["Gas"] = String(Gas);
    #endif
    telemetry_doc["Interval"] = String(rtcData.ByteValue*config.DEEPSLEEP);
    #ifdef GPS_Enabled
        gps_update();
        if (GPS_Valid){
            telemetry_doc["Sat"] = String(GPS_Sat).toInt();
            telemetry_doc["Lat"] = String(GPS_Lat, 6).toFloat();
            telemetry_doc["Lng"] = String(GPS_Lng, 6).toFloat();
            telemetry_doc["Speed"] = String(GPS_Speed, 1).toFloat();
        }
    #endif

    serializeJson(telemetry_doc, telemetry_jsonString);             //Serialize JSON data to string
    //telnet_println("Telemetry: " + String(telemetry_jsonString));
    mqtt_publish(mqtt_pathtele, "Telemetry", String(telemetry_jsonString));
}
