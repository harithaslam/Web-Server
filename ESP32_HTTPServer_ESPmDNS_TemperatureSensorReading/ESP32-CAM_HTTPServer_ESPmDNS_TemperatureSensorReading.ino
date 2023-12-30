/*Using ESP-32 CAM Module and Read Temperature on HTTP Client Server over WiFi*/  //reference https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/
/*A clock Module RTC DS1302*/    //library used https://github.com/Makuna/Rtc/wiki
/*Web Server used is local for my initital testing which is using your own Wifi IP Address*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <RtcDS1302.h>
#include <ThreeWire.h>

// DHT  settings starts
#include "DHT.h"
#define DHTPIN 2  //what digital pin we're connected to

//Uncomment whatever type you're using

#define DHTTYPE DHT11  //DHT11
//#define DHTTYPE DHT22  //DHT 22 (AM2302), AM2321
//#define DHTTYPE DHT21  //DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

ThreeWire myWire(13, 12, 15);  //DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);
float tValue;
//  ***** DHT settings end

const int refresh = 3;  // 3 seconds

//Date&Time Value using RTC sensor
int day;
int month;
int year;

int hour;
int minute;
int second;
String date;
String t;

#ifndef STASSID
#define STASSID "************"  //Your Wifi
#define STAPSK "************"      //Your Wifi password
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WebServer server(80);  //HTTP we use port 80

void sendTemp() {
  String page = "<!DOCTYPE html>\n\n";
  page += "    <meta http-equiv='refresh'content ='";
  page += String(refresh);  //how often temperature is read
  page += "'/n>";
  page += "<html>\n";
  page += "<p body>\n";
  page += "<h1 {text-align: center;}>";
  page += "<h1>My First Web Server V1</h1>";
  page += "<ul>";
  page += "<li><style=\"font-size:25px;\">";
  page += "<style=\"color:red; font-size:25px;\">";
  page += date;
  page += "    ";
  page += "<style=\"font-size:25px;\">";
  page += "<style=\"color:red; font-size:25px;\">";
  page += t;
  page += "</li><li><style=\"font-size:25px;\">Temperature: ";
  page += "<style=\"color:red; font-size:25px;\">";
  page += String(tValue, 2);
  page += "&deg;C</li>";
  page += "</ul>";
  page += "</p>\n";
  page += "</body>\n";
  page += "</html>\n";
  server.send(200, "text/html", page);  //200 meeans pages okay or respond okay
                                        //text/html shows the content is text
}
//server function edit
// void sendTemp() {
//   String page = "<!DOCTYPE html>\n\n";
//   page += "    <meta http-equiv='refresh'content ='";
//   page += String(refresh);  //how often temperature is read
//   page += "'/n>";
//   page += "<html>\n";
//   page += "<body>\n";
//   page += "<p style=\"font-size:50px;\">Temperature<br/>\n";
//   page += "<p style=\"color:red; font-size:50px;\">";
//   page += String(tValue, 2);
//   page += "</p>\n";
//   page += "</body>\n";
//   page += "</html>\n";
//   server.send(200, "text/html", page);  //200 meeans pages okay or respond okay
//                                         //text/html shows the content is text
// }
void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i);
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  dht.begin();  //Begin DHT library
  Rtc.Begin();  //Begin RTC Library
  Serial.begin(115200);

  RtcDateTime currentTime = RtcDateTime(__DATE__, __TIME__);  //These two line of code need to update once
  Rtc.SetDateTime(currentTime);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  //Wait for Connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Adress: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", sendTemp);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();  //this will start the server
  //MDNS.update();
  float c = dht.readTemperature();      //Read temperature as Celcius
  float h = dht.readHumidity();         //Read temperature as Celcius
  float f = dht.readTemperature(true);  //Read temperature as Farenheit
  RtcDateTime now = Rtc.GetDateTime();
  //edit here for display date&time float
  day = now.Day();
  month = now.Month();
  year = now.Year();

  hour = now.Hour();
  minute = now.Minute();
  second = now.Second();

  date = "Date: " + String(day) + "/" + String(month) + "/" + String(year);
  t = "Time: " + String(hour) + ":" + String(minute) + ":" + String(second);

  Serial.println(c);
  tValue = c;
  delay(300);
}
