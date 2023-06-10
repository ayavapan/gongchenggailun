//温度传感器
#include <esp_now.h>
#include <WiFi.h>
#include "driver/temp_sensor.h"
//发送目标板子的地址，别改
uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0x90, 0x78};
//连接的wifi名字和密码，要调试就改成你们的，注意“”号别删
const char* ssid     = "关注兰音谢谢喵"; //wifi名字
const char* password = "pxt20020918"; //wifi密码
// 还是创建一个结构体类型
//-----------------------------------------------------------------------------------------------------------//
//以下部分是发送信息的结构体定义和wifi发送信息函数，慎改
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
  pinMode(A0, INPUT); // 设置IO1为模拟输入
  pinMode(A1, INPUT); // 设置IO2为模拟输入
  myData.id = 1; //注意这里的id，每块发送的板子不能重复
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

}
 int temp,light;
//-----------------------------------------------------------------------------------------------------------//
void loop() {
  // 设置发送的数据
  //获得芯片温度传感器数据，可删
  //最重要的是要用想发送的数据给myData.xmyData.y赋值（时间，外接温度传感器），注意要用int形，再用post（）函数实现发送。
  temp= analogRead(A0);
  light= analogRead(A1);
  myData.x=(int)temp;
  myData.y=(int)light;
  post();
  delay(2000);
}