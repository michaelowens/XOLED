#include "xoled.h"

PubSubClient mqtt_client(esp_client);

void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message received [");
  // Serial.print(topic);
  // Serial.print("] ");
  // parse payload...

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.println("ERROR DESERIALIZING");
  } else {
    // Serial.print("Progress: ");
    progress = doc["print"]["mc_percent"];
    // Serial.println(progress);
  }
}

void mqtt_reconnect() {
  if (strlen(config.mqtt_server) < 1) return;
  mqtt_client.setServer(config.mqtt_server, 8883);

  while (!mqtt_client.connected()) {
    Serial.print("Attempting to (re)connect to Bambu printer... ");

    bool connected;
    if (strlen(config.mqtt_user) > 0 || strlen(config.mqtt_pass) > 0) {
      connected = mqtt_client.connect("arduinoClient", "bblp", config.mqtt_pass);
    } else {
      Serial.print("(no credentials) ");
      connected = mqtt_client.connect("arduinoClient");
    }

    if (connected) {
      Serial.println("connected");
      // mqtt_client.publish();
      char topic[32];
      sprintf(topic, "device/%s/report", config.mqtt_device_serial);
      mqtt_client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqtt_setup() {
  esp_client.setInsecure();
  mqtt_client.setCallback(callback);
  mqtt_client.setBufferSize(JSON_BUFFER_SIZE);
}

void mqtt_loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqtt_client.connected()) {
      mqtt_reconnect();
    }
    mqtt_client.loop();
  }
}