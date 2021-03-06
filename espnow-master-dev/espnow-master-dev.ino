#include <Arduino.h>
#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
#include <user_interface.h>
}

#define WIFI_DEFAULT_CHANNEL 9

// define pin for espresso lite
#define buttun0 0
#define buttun13 13


//uint8_t no1[] = {0x1A, 0xFE, 0x34, 0xDA, 0xF0, 0x993}; // esp number 1 - master
uint8_t no2[] = {0x1A, 0xFE, 0x34, 0xDA, 0xF1, 0xB8}; // esp number 2 - slave
//uint8_t no3[] = {0x1A, 0xFE, 0x34, 0xEE, 0xCD, 0x15}; // esp number 3 - slave
//uint8_t no4[] = {0x5E, 0xCF, 0x7F, 0x9, 0x9A, 0xFE}; // esp number 4 - slave

boolean state = false;
boolean count = false;

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

  // for espresso lite
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttun0, INPUT);
  pinMode(buttun13, INPUT);

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
    //    Serial.print("send_cb");
    //    Serial.print("mac address: ");
    //    printMacAddress(macaddr);
    //    Serial.print(" status = "); Serial.println(status);
  });

  //int res = esp_now_add_peer(no1, (uint8_t)ESP_NOW_ROLE_SLAVE, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);
  int res2 = esp_now_add_peer(no2, (uint8_t)ESP_NOW_ROLE_SLAVE, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);
  //int res3 = esp_now_add_peer(no3, (uint8_t)ESP_NOW_ROLE_SLAVE, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);
  //int res4 = esp_now_add_peer(no4, (uint8_t)ESP_NOW_ROLE_SLAVE, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);

}

bool _status = true;
uint8_t message[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
int number;

void loop() {

  if (digitalRead(buttun13) == 1 && state == false)  {
    delay(200);
    if (count == true)  {
      Serial.println("Toggle ON");
      esp_now_send(NULL, &message[0], 1);
      esp_now_send(NULL, &message[2], 1);
      esp_now_send(NULL, &message[4], 1);
    } else if (count == false)  {
      Serial.println("Toggle OFF");
      esp_now_send(NULL, &message[1], 1);
      esp_now_send(NULL, &message[3], 1);
      esp_now_send(NULL, &message[5], 1);
    }
    digitalWrite(led1, !digitalRead(led1));
    digitalWrite(led2, !digitalRead(led2));
    digitalWrite(led3, !digitalRead(led3));
    Serial.println("Press");
    state = !state;

  }

  if (digitalRead(buttun13) == 1)  {
    delay(200);
    state = false;
    count = !count;
  }

  if (digitalRead(buttun12) == 1)  {
    delay(200);
    esp_now_send(NULL, &message[0], 1);
    esp_now_send(NULL, &message[2], 1);
    esp_now_send(NULL, &message[4], 1);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    count = true;
  }
  if (digitalRead(buttun14) == 1)  {
    delay(200);
    esp_now_send(NULL, &message[1], 1);
    esp_now_send(NULL, &message[3], 1);
    esp_now_send(NULL, &message[5], 1);
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    count = false;
  }

  // esp_now_send(neo_slave, message, sizeof(message));
  // esp_now_send(bare_up_slave, message, sizeof(message));
  //  message[0] = _status;
  //    _status = !_status;
  //  digitalWrite(LED_BUILTIN, _status);
  //  delay(200);
}

