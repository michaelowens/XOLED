#ifndef XOLED_MQTT_H
#define XOLED_MQTT_H

void mqtt_setup();
void mqtt_reconnect();
void mqtt_loop();
bool mqtt_is_connected();

#endif // XOLED_MQTT_H