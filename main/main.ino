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
#include "bot-commands.h"

#define BOT_MESSAGE_OPEN "-------------------------------\n\nALERT DOOR OPENED\n\n-------------------------------"
#define BOT_MESSAGE_CLOSE "-------------------------------DOOR CLOSED"

const int BUTTON = 14;
bool log_flag = false;
String ip_address;

const unsigned long BOT_MTBS = 1000;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;          // last time messages' scan has been done
bool Start = false;

void handleNewMessages(int numNewMessages)
{
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    String user = bot.messages[i].from_id;
    String user_id = bot.messages[i].from_name;

    
    if (from_name == "")
      from_name = "Guest";

    if (text == "/send-iproto" && chat_id == CHAT_ID)
    {
      bot.sendChatAction(CHAT_ID, "typing");
      
      String new_message = "𝐈𝐧𝐢𝐭𝐢𝐚𝐭𝐢𝐧𝐠  𝐃𝐨𝐨𝐫-𝐋𝐨𝐠-𝐀𝐥𝐞𝐫𝐭-𝐒𝐲𝐬𝐭𝐞𝐦-𝐁𝐨𝐭  𝐒𝐞𝐪𝐮𝐞𝐧𝐜𝐞" ;
      new_message += "\n\n💠 Trigger Mode : MANUAL 🛑" ;
      new_message += "\n\n💠 Triggered By : " + from_name ;
      new_message += "\n\nBot Status : ONLINE AND READY ✅" ;
      new_message += "\n\n🌐 IP address : " + ip_address ;

      bot.sendMessage(CHAT_ID, new_message, "");
    }
    else if (text != "/send-iproto" && chat_id == CHAT_ID)
    {
      bot.sendMessage(CHAT_ID, "COMMON man "+text);
    }
    else
    {
      bot.sendMessage(CHAT_ID, "RANDOM TRIGGER : " + from_name + "\n\n"+user+"\n\n"+user_id+"\n\n"+text);
    }
  }
}

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

  bot.sendMessage(CHAT_ID, "𝐈𝐧𝐢𝐭𝐢𝐚𝐭𝐢𝐧𝐠  𝐃𝐨𝐨𝐫-𝐋𝐨𝐠-𝐀𝐥𝐞𝐫𝐭-𝐒𝐲𝐬𝐭𝐞𝐦-𝐁𝐨𝐭  𝐒𝐞𝐪𝐮𝐞𝐧𝐜𝐞\n\n💠 Trigger Mode : BOOT\n\nBot Started...\n\nBOT ONLINE AND READY ✅\n\n🌐 IP address : "+ ip_address + "", "");

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

  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

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