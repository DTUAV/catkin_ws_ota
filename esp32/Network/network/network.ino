#include <WiFi.h>
#include "network.h"
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>

Network net;

void setup() {
Serial.begin(115200);
net.init();
}

void ProcessRecvMsg(const std::string& recv_data, int message_type) {
  switch(message_type) {
    case 0: {

    }
    break;
    case 99: {
      String update_url = String(recv_data.c_str());
      t_httpUpdate_return ret = ESPhttpUpdate.update(update_url);

#ifdef debug
     Serial.println(update_url);
    Serial.println(ret);
#endif

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;
      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
      default:
        Serial.println(ret);
    }
    }
    break;
    default:
    break;
  }
}

void loop() {
bool flag = false;
std::string recv_data = " ";
int message_type = 0;
flag = net.GetNetworkData(recv_data, message_type);
ProcessRecvMsg(recv_data, message_type);
delay(100);
}
