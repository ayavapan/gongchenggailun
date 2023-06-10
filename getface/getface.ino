//温度传感器
#include <esp_now.h>
#include <WiFi.h>
#include "driver/temp_sensor.h"
uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0x90, 0x78};

const char* ssid     = "关注兰音谢谢喵"; //wifi名字
const char* password = "pxt20020918"; //wifi密码
// 还是创建一个结构体类型
typedef struct struct_message {
    int id; //注意这里的id非常重要，作为区分不同发送端板子的标
    int x;
    int y;
} struct_message;
struct_message myData;
//发送回调函数
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void post(){
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
void setup() {
  myData.id = 5; //注意这里的id，每块发送的板子不能重复
  Serial.begin(115200);
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
  WiFi.begin(ssid, password); 
   while (WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
  };
  temp_sensor_config_t temp_sensor = {
    .dac_offset = TSENS_DAC_L2,
    .clk_div = 6,
  };

}
int getcomplete=0;
int RXsta=0;
int id=0;

void serialEvent()
{
  if(Serial.available())
  {
    int getRX=Serial.read();
    if(RXsta==0&&getRX==0xFF)
      RXsta=1;
    else if(RXsta==1&&getRX==0xFF)
    {
      RXsta==2;
    }
    else if(RXsta==2)
    {
      id=getRX;
      RXsta==3;
    }
    else if(RXsta==3&&getRX==0xAA)
    {
      RXsta==0;
      myData.x=id;
      myData.y=0;
      getcomplete=1;
    }
    else
    {
      RXsta=0;
    }
  }
}
void loop() {

  if( getcomplete==1)
  {
    post();
    getcomplete=0;
  }
  delay(2000);
}

