#include "xoled.h"
#include <optional>

PubSubClient mqtt_client(esp_client);
int led_status = LOW;

void callback(char* topic, byte* payload, unsigned int length) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.println("ERROR DESERIALIZING");
  } else {
    if (doc["print"] == nullptr) return;

    if (doc["print"]["command"] != nullptr) {
      char command[32];
      strlcpy(command, doc["print"]["command"], 32);

      if (strcmp(command, "project_prepare") == 0) {
          StateData new_data;
          new_data.printing.percentage = 0;
          g_Printer.set_state(PrinterState::Printing, new_data);
        return;
      }
    }

    if (doc["print"]["gcode_state"] == nullptr) return;
    char gcode_state[32];
    strlcpy(gcode_state, doc["print"]["gcode_state"], 32);

    if (strcmp(gcode_state, "RUNNING") == 0) {
      switch (g_Printer.state) {
        case PrinterState::Loading:
        case PrinterState::Finished:
        case PrinterState::Error: {
          StateData new_data;
          new_data.printing.percentage = doc["print"]["mc_percent"];
          g_Printer.set_state(PrinterState::Printing, new_data);
          break;
        }

        case PrinterState::Printing: {
          uint8_t new_percentage = doc["print"]["mc_percent"];
          // Sometimes messages report 0%
          if (new_percentage == 0) break;
          g_Printer.data.printing.percentage = new_percentage;
          break;
        }
      }
    } else if (strcmp(gcode_state, "FINISH") == 0) {
      switch (g_Printer.state) {
        case PrinterState::Loading:
        case PrinterState::Printing: {
          StateData new_data;
          new_data.finished.since = millis();
          g_Printer.set_state(PrinterState::Finished, new_data);
          break;
        }
      }
    } else if (strcmp(gcode_state, "FAILED") == 0) {
      switch (g_Printer.state) {
        case PrinterState::Error: {
          break;
        }

        default: {
          StateData new_data;
          g_Printer.set_state(PrinterState::Error, new_data);
          break;
        }
      }
    }
  }
}


void mqtt_reconnect() {
  if (strlen(config.mqtt_server) < 1) return;

  EVERY_N_SECONDS(5) {
    mqtt_client.setServer(config.mqtt_server, 8883);

    if (!mqtt_client.connected()) {
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
      }
    }
  }
}

bool mqtt_setup_has_run = false;
void mqtt_setup() {
  if (mqtt_setup_has_run) return;
  mqtt_setup_has_run = true;
  
  pinMode(LED_BUILTIN, OUTPUT);
  esp_client.setInsecure();
  mqtt_client.setCallback(callback);
  mqtt_client.setBufferSize(JSON_BUFFER_SIZE);
}


void mqtt_update_status_led() {
  int expected = mqtt_client.connected() ? 0 : 1;
  if (expected != led_status) {
    led_status = expected;
    Serial.print("Changing MQTT status led to: ");
    Serial.println(led_status ? "on" : "off");
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void mqtt_loop() {
  mqtt_update_status_led();

  if (WiFi.status() == WL_CONNECTED) {
    if (!mqtt_client.connected()) {
      mqtt_reconnect();
    }
    mqtt_client.loop();
  }
}

bool mqtt_is_connected() {
  return mqtt_client.connected();
}