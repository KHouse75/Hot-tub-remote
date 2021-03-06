#ifndef HOTTUBMQTT_H
#define HOTTUBMQTT_H

#include <ArduinoJson.h>
#include "HotTub.h"
#include "PubSubClient.h"
#include <ESP8266WiFi.h>


class HotTubMqtt {
  public:
    HotTubMqtt(HotTub* hotTubInstance);
    
    void callback(char* topic, byte* payload, unsigned int length);
    
    void setup(void (&onMqttEvent)(char * topic, byte* payload, unsigned int length));
    void setServer(char* ipAddress, int port);
    void setCredentials(char* user, char* pass);
	  void loop();

    void sendStatus();
  private:
    HotTub* hotTub;
    WiFiClient espClient;
    PubSubClient client;

    void reconnect();
    void subscribe();
    char* getClientState();
    bool connect();

    char* mqttServer;
    int mqttPort;

    bool hasCredentials;
    char* mqttUser;
    char* mqttPass;

    void handleStateMessages(char* topic, char* message, unsigned int length);
    void handleValueMessages(char* topic, char* message, unsigned int length);
    void handleRawCommand(char* message);
};

#endif
