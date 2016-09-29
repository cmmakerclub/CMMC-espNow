#include <Arduino.h>
#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
#include <user_interface.h>
}

#define WIFI_DEFAULT_CHANNEL 9

// neo = {0x1A,0xFE,0x34,0xEE,0xCA,0xED}
//uint8_t neo_slave[] = {0x1A,0xFE,0x34,0xEE,0xCA,0xED};
//uint8_t bare_up_slave[] = {0x5E,0xCF,0x7F,0x9,0x98,0x4E};
//uint8_t bare_no_up_nodht[] = {0x1A,0xFE,0x34,0xDA,0xEA,0xD0};
//uint8_t no[2][6]= {
//    {0x1A,0xFE,0x34,0xDA,0xEF,0x5F},
//    {0x1A,0xFE,0x34,0xDB,0x32,0xEB},
// };

//uint8_t no1[] = {0x1A, 0xFE, 0x34, 0xDA, 0xEF, 0x5F}; // default

//uint8_t no1[] = {0x1A, 0xFE, 0x34, 0xDA, 0xF1, 0xD3}; // esp number 1 - master 0x1A,0xFE,0x34,0xDA,0xF0,0x99
uint8_t no2[] = {0x1A, 0xFE, 0x34, 0xDA, 0xF1, 0xB8}; // esp number 2 - slave
uint8_t no3[] = {0x1A, 0xFE, 0x34, 0xEE, 0xCD, 0x15}; // esp number 3 - slave
uint8_t no4[] = {0x5E, 0xCF, 0x7F, 0x9, 0x9A, 0xFE}; // esp number 4 - slave

void printMacAddress(uint8_t* macaddr) {
  Serial.print("{");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    Serial.print(macaddr[i], HEX);
    if (i < 5) Serial.print(', ');
  }
  Serial.println("}");
}

void setup() {
  WiFi.disconnect();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(0, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Initializing...");

  WiFi.mode(WIFI_STA);

  uint8_t macaddr[6];
  wifi_get_macaddr(STATION_IF, macaddr);
  Serial.print("mac address (STATION_IF): ");
  printMacAddress(macaddr);

  wifi_get_macaddr(SOFTAP_IF, macaddr);
  Serial.print("mac address (SOFTAP_IF): ");
  printMacAddress(macaddr);

  if (esp_now_init() == 0) {
    Serial.println("direct link  init ok");
  } else {
    Serial.println("dl init failed");
    ESP.restart();
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_recv_cb([](uint8_t *macaddr, uint8_t *data, uint8_t len) {
    Serial.println("recv_cb");

    Serial.print("mac address: ");
    printMacAddress(macaddr);

    Serial.print("data: ");
    for (int i = 0; i < len; i++) {
      Serial.print(data[i], HEX);
    }
    Serial.print("  ");
  });

  esp_now_register_send_cb([](uint8_t* macaddr, uint8_t status) {
    Serial.print("send_cb");

    Serial.print("mac address: ");
    printMacAddress(macaddr);

    Serial.print(" status = "); Serial.println(status);
  });

  //  int res = esp_now_add_peer(no1, (uint8_t)ESP_NOW_ROLE_SLAVE, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);
  int res2 = esp_now_add_peer(no2, (uint8_t)ESP_NOW_ROLE_SLAVE, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);
  int res3 = esp_now_add_peer(no3, (uint8_t)ESP_NOW_ROLE_SLAVE, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);
  int res4 = esp_now_add_peer(no4, (uint8_t)ESP_NOW_ROLE_SLAVE, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);
  
  //  int res2 = esp_now_add_peer(no[1], (uint8_t)ESP_NOW_ROLE_SLAVE,(uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);

  //  res = esp_now_add_peer(bare_up_slave, (uint8_t)ESP_NOW_ROLE_SLAVE,(uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);
  //  res = esp_now_add_peer(bare_no_up_nodht, (uint8_t)ESP_NOW_ROLE_SLAVE,(uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);

  // ESP.deepSleep(2.5e6, WAKE_RF_DEFAULT);

  //  esp_now_unregister_recv_cb();
  //  esp_now_deinit();
}

bool _status = true;
uint8_t message[] = { 0x00, 0x01 };
int number;

void loop() {
  if (digitalRead(0) == 0)  {
    delay(200);
    //    message = 1;
    esp_now_send(NULL, &message[0], 1);
    //    esp_now_send(NULL, message[0], 1);
    Serial.println("Button press");
  } else {
    //    delay(200);
    //    message = 0;
    esp_now_send(NULL, &message[1], 1);
    //    esp_now_send(NULL, message[0], 1);
    //    Serial.println("Button not press");
  }

  // esp_now_send(neo_slave, message, sizeof(message));
  // esp_now_send(bare_up_slave, message, sizeof(message));
  //  message[0] = _status;
  //    _status = !_status;
  //  digitalWrite(LED_BUILTIN, _status);
  //  delay(200);
}

