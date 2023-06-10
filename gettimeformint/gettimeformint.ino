#include <Arduino.h>
#include "SSD1306Wire.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_now.h>
uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0x90, 0x78};
typedef struct struct_message {
    int id; //注意这里的id非常重要，作为区分不同发送端板子的标
    int x;
    int y;
} struct_message;
struct_message myData;//

typedef struct realtime {
    String time;
}WeatherData;
realtime internettime;//实例化
//发送回调函数
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery ");
}

void post(int idofdata){
  myData.id=idofdata; //注意这里的id，每块发送的板子不能重复
  // 向指定MAC地址发送数据
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   Serial.print(myData.x);
   Serial.print(myData.y);
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

};

const int SDA_PIN = 4;  //引脚连接，ESP32
const int SCL_PIN = 5;  //
SSD1306Wire display(0x3c, SDA_PIN, SCL_PIN);
//有关天气的结构体

void gettime(realtime *data){
  HTTPClient http;
  String url = "http://quan.suning.com/getSysTime.do";
   
  http.begin(url);
  int httpCode=http.GET();
  delay(1000);
  if(httpCode>0)
  {
    if(httpCode==HTTP_CODE_OK)
    {
    String payload = http.getString();
    display.drawString(0, 24,payload);
    Serial.print(payload);
    Serial.print("\n");
    DynamicJsonDocument  jsonBuffer(2048);
    deserializeJson(jsonBuffer, payload);
    JsonObject root = jsonBuffer.as<JsonObject>();
    String tmp = root["sysTime1"];

    Serial.print(tmp);
    Serial.print("\n");
    data->time=tmp;
    }
  }
  else{
  display.drawString(0, 24,"get time failed");
  display.display();
  }
}

const char* ssid     = "关注兰音谢谢喵"; //wifi名字
const char* password = "pxt20020918"; //wifi密码

void setup() {

  WiFi.mode(WIFI_STA);
  // 初始化ESP_NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // 注册发送回调函数
  esp_now_register_send_cb(OnDataSent);
  
  // 注册通信频道
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
      
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  Serial.begin(115200);
  display.init();
  display.clear();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);  

  display.drawString(0, 0,"Connecting...");
  display.display();
  WiFi.begin(ssid, password); 
   while (WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
  };
  display.clear();
  display.drawString(0,0,"Connected!");
  display.display();
  delay(1000);  
  display.clear();
  display.display();

}
void loop() {
  display.clear();
  display.display();
  gettime(&internettime);
  display.drawString(0, 0,internettime.time);
  String year=internettime.time.substring(0, 4);
  String md=internettime.time.substring(4, 8);
  String hm=internettime.time.substring(8, 12);
  String s=internettime.time.substring(12, 14);
  Serial.print("year");Serial.print("md");Serial.print("hm");Serial.print("s");
  myData.x=year.toInt();
  myData.y=md.toInt();
  post(6);
  myData.x=hm.toInt();
  myData.y=s.toInt();
  post(7);
  display.display();
  delay(1000);
}
