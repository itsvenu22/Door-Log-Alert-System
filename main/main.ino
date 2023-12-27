/********************************************************************************************
 *  Create a file named "credentials.h" in the same directory with the following contents:
 *  
 *  WiFi Credentials:
 *  #define WIFI_SSID "<your_wifi_ssid>"
 *  #define WIFI_PASSWORD "<your_wifi_password>"
 *
 *  Telegram Bot Credentials:
 *  #define BOT_TOKEN "<your_token_from_botfather>"
 *  #define CHAT_ID "<your_chat_id>"
 *  
 *  CHAD DUDES AVOID THIS --> #define WIFI_SSID "<myhomenetwork>"
 * 
 *  Make sure to replace the placeholder values with your actual credentials.
 ********************************************************************************************/


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "credentials.h"

#define BOT_MESSAGE_OPEN "ALERT DOOR OPENED"
#define BOT_MESSAGE_CLOSE "-------------------------------DOOR CLOSED"

const int BUTTON = 14;
bool log_flag = false;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  Serial.println();

 // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  IPAddress localIP = WiFi.localIP();
  String ip_address = localIP.toString();
  Serial.println(ip_address);

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(CHAT_ID, "Bot started up...\nMy IP address : "+ ip_address, "");


}
void door_open() {

  // Changed to send a Telegram message
  bot.sendMessage(CHAT_ID, BOT_MESSAGE_OPEN);


}
void door_closed() {

  // Changed to send a Telegram message
  bot.sendMessage(CHAT_ID, BOT_MESSAGE_CLOSE);

}
void loop() {
  int button = digitalRead(BUTTON);
  if (button == LOW)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    door_closed();

  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    door_open();
  }
}