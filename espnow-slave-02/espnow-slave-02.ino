#include <Arduino.h>
#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
#include <user_interface.h>
}

#define WIFI_DEFAULT_CHANNEL 9
#define RELAY 15

/* 0b00000001 esp 04 
   0b00000010 esp 03 
   0b00000100 esp 02 
*/
#define CMMC_DEVICE_ID   1
#define CMMC_DEVICE_MASK 0b00000010

// esp number 1 master
// esp number 2 {0x1A, 0xFE, 0x34, 0xDA, 0xF1, 0xB8};

// {0x18,0xFE,0x34,0xEE,0xCA,0xED} = bare
//uint8_t mac[] = {0x5C, 0xCF, 0x7F, 0x9, 0xCC, 0x35};

uint8_t mac[] = {0x1A, 0xFE, 0x34, 0xDA, 0xF0, 0x99};

//uint8_t mac[] = {0x18, 0xFE, 0x34, 0xEE, 0xEC, 0xA7};
uint16_t counter = 0;

#include <Ticker.h>

Ticker ticker;

void printMacAddress(uint8_t* macaddr) {
  Serial.print("{");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    Serial.print(macaddr[i], HEX);
    if (i < 5) Serial.print(',');
  }
  Serial.println("}");
}

void setup() {
  WiFi.disconnect();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY, OUTPUT);

  Serial.begin(115200);
  Serial.println("Initializing... SLAVE");

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RELAY, HIGH);

  WiFi.mode(WIFI_AP_STA);

  uint8_t macaddr[6];
  wifi_get_macaddr(SOFTAP_IF, macaddr);
  Serial.print("mac address (SOFTAP_IF): ");
  printMacAddress(macaddr);

  if (esp_now_init() == 0) {
    Serial.println("init");
  } else {
    Serial.println("init failed");
    ESP.restart();
    return;
  }

  Serial.println("SET ROLE SLAVE");
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb([](uint8_t *macaddr, uint8_t *data, uint8_t len) {

    Serial.print(data[0], BIN);
    Serial.print(" ");
    Serial.println( data[0] & CMMC_DEVICE_MASK );

    digitalWrite(LED_BUILTIN, !(data[0] & CMMC_DEVICE_MASK));
    digitalWrite(RELAY, data[0] & CMMC_DEVICE_MASK);

    /*** esp now 02 ***/
    //    if (data[0] == 4)  { // for esp slave number 02
    //      digitalWrite(LED_BUILTIN, LOW);
    //      digitalWrite(RELAY, HIGH);
    //    } else if (data[0] == 5) {
    //      digitalWrite(LED_BUILTIN, HIGH);
    //      digitalWrite(RELAY, LOW);
    //    }
    /*** esp now 03 ***/
    //    if (data[0] == 0 || (data[0] & CMMC_DEVICE_MASK) == 2)  { // for esp slave number 03
    //      digitalWrite(LED_BUILTIN, LOW);
    //      digitalWrite(RELAY, HIGH);
    //    } else if (data[0] == 1 || (data[0] & CMMC_DEVICE_MASK) == 0) {
    //      digitalWrite(LED_BUILTIN, HIGH);
    //      digitalWrite(RELAY, LOW);
    //    }
    /*** esp now 04 ***/
    //    if (data[0] == 2)  { // for esp slave number 04
    //      digitalWrite(LED_BUILTIN, LOW);
    //      digitalWrite(RELAY, HIGH);
    //    } else if (data[0] == 3) {
    //      digitalWrite(LED_BUILTIN, HIGH);
    //      digitalWrite(RELAY, LOW);
    //    }
    /*** Toggle ***/
    //    if (data[0] == 2)  { // for esp slave number 03
    //      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    //      digitalWrite(RELAY, !digitalRead(RELAY));
    //    }
    //    if (data[0] == 4)  { // for esp slave number 04
    //      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    //      digitalWrite(RELAY, !digitalRead(RELAY));
    //    }

    //    digitalWrite(LED_BUILTIN, !data[0]);
    //    Serial.println(data[0]);
    delay(500);
  });

  esp_now_register_send_cb([](uint8_t* macaddr, uint8_t status) {
    Serial.println("send_cb");

    Serial.print("mac address: ");
    printMacAddress(macaddr);

    Serial.print("status = "); Serial.println(status);
  });

  int res = esp_now_add_peer(mac, (uint8_t)ESP_NOW_ROLE_CONTROLLER, (uint8_t)WIFI_DEFAULT_CHANNEL, NULL, 0);

  if (res) {
  }

  Serial.println("BEGIN TICKER");
  ticker.attach_ms(1000, [&]() {
    //    Serial.printf("Counter = %lu\r\n", counter);
    counter = 0;
  });

  //  esp_now_unregister_recv_cb();
  //  esp_now_deinit();
}

void loop() {
  yield();
}

