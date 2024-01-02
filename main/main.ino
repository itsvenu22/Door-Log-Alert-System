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

#define BOT_MESSAGE_OPEN "🆘🆘🆘🆘🆘🆘🆘🆘🆘🆘\n\n⚠️ ALERT DOOR OPENED ⚠️"
#define BOT_MESSAGE_CLOSE "❇️ ALERT DOOR CLOSED ✅"

const int BUTTON = 0;
bool door_close = false;

// if you are adding any pass/act-ive indicators use "true"
bool breq_flag = false;
bool buzz_flag = false;
const int BUZZ_PIN = 10;

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

    String chat_title = bot.messages[i].chat_title;
    String date = bot.messages[i].date;
    String type = bot.messages[i].type;

    // Additional properties (not needed tho :} )
    String file_caption = bot.messages[i].file_caption;
    String file_path = bot.messages[i].file_path;
    String file_name = bot.messages[i].file_name;
    boolean hasDocument = bot.messages[i].hasDocument;
    long file_size = bot.messages[i].file_size;
    float longitude = bot.messages[i].longitude;
    float latitude = bot.messages[i].latitude;
    int update_id = bot.messages[i].update_id;
    int message_id = bot.messages[i].message_id;

    // Reply information
    int reply_to_message_id = bot.messages[i].reply_to_message_id;
    String reply_to_text = bot.messages[i].reply_to_text;

    // Query ID (assuming query_id is associated with some specific type of message)
    String query_id = bot.messages[i].query_id;

    // Construct your message add whatever you want :3


    if (from_name == "")
      from_name = "Guest";

    if (text == "/sendipaddress" && chat_id == CHAT_ID)
    {
      bot.sendChatAction(CHAT_ID, "typing");

      IPAddress localIP = WiFi.localIP();
      String ip_address = localIP.toString();

      String new_message = "𝐈𝐧𝐢𝐭𝐢𝐚𝐭𝐢𝐧𝐠  𝐃𝐨𝐨𝐫-𝐋𝐨𝐠-𝐀𝐥𝐞𝐫𝐭-𝐒𝐲𝐬𝐭𝐞𝐦-𝐁𝐨𝐭  𝐒𝐞𝐪𝐮𝐞𝐧𝐜𝐞" ;
      new_message += "\n\n💠 Trigger Mode : Super User ~ MANUAL 🛑" ;
      new_message += "\n\n💠 Triggered By : " + from_name ;
      new_message += "\n\nBot Status : ONLINE AND READY ✅" ;
      new_message += "\n\n🌐 IP address : " + ip_address ;

      bot.sendMessage(CHAT_ID, new_message, "");
    }
    else if (text != "/sendipaddress" && chat_id == CHAT_ID)
    {
      String new_message2 = "Super User...\n\n" + text ;
      new_message2 += "\n\nThat's not allowed 🥺💚" ;
      bot.sendMessage(CHAT_ID, new_message2);
    }
    else
    {
      String message ="❎ RANDOM TRIGGER ❎\n\nName : " + from_name +
                      "\nSent Text : " + text +
                      "\n\nChat ID : " + chat_id +
                      "\nChat Title : " + chat_title +
                      "\nDate : " + date +
                      "\nType : " + type +
                      "\nUpdate ID : " + update_id +
                      "\nMessage ID : " + message_id +
                      "\nFile Caption : " + file_caption +
                      "\nFile Path : " + file_path +
                      "\nFile Name : " + file_name +
                      "\nHas Document : " + hasDocument +
                      "\nFile Size : " + file_size +
                      "\nLatitude  : " + latitude +
                      "\nLongitude : " + longitude +
                      "\nReply to Message I'D : " + reply_to_message_id +
                      "\nReply to Text : " + reply_to_text +
                      "\nQuery ID : " + query_id;
      bot.sendMessage(CHAT_ID, message, "");
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
void buzz_ntfy(bool bflag) {
  if (breq_flag) {
    if (bflag) {
      digitalWrite(BUZZ_PIN, HIGH);
    }
    else {
      digitalWrite(BUZZ_PIN, LOW);
    }
  }
}
void door_open() {
  // Changed to send a Telegram message
  bot.sendMessage(CHAT_ID, BOT_MESSAGE_OPEN);
  door_close = false;
  buzz_ntfy(true);
}
void door_closed() {
  // Changed to send a Telegram message
  bot.sendMessage(CHAT_ID, BOT_MESSAGE_CLOSE);
  door_close = true;
  buzz_ntfy(false);
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
    if (door_close == false)
    {
      door_closed();
    }
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    door_open();
  }
}