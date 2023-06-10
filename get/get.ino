#include <esp_now.h>
#include <WiFi.h>
#include "SSD1306Wire.h"
//oled显示
const int SDA_PIN = 4;  //引脚连接，ESP32
const int SCL_PIN = 5;  //
SSD1306Wire display(0x3c, SDA_PIN, SCL_PIN);

// 和发送端一样，这里也声明一个结构体
typedef struct struct_message {
  int id;
  int x;
  int y;
}struct_message;

struct_message myData;

//123为采集的温度
struct_message board1;//温度传感器光强传感器
struct_message board2;
struct_message board3;
struct_message board4;//第四组是从网上找的环境温度
struct_message board5;//第5组来自openmv
struct_message board6;//年月日
struct_message board7;//时分秒
// 创建一个结构体数组
struct_message boardsStruct[7] = {board1, board2,board3,board4,board5,board6,board7};
const char* ssid     = "关注兰音谢谢喵"; //wifi名字
const char* password = "pxt20020918"; //wifi密码
// 接收回调函数
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  //Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  boardsStruct[myData.id-1].x = myData.x;
  boardsStruct[myData.id-1].y = myData.y;
  //Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // 更新数据
  //if(myData.id==5)
  //{
  ///boardsStruct[myData.id-1].id =myData.id;
  //if(boardsStruct[myData.id-1].x==0)
     // boardsStruct[myData.id-1].x = myData.x;
 // boardsStruct[myData.id-1].y = 0;
  //}
  //else
  //{
    //boardsStruct[myData.id-1].id =myData.id;
    //boardsStruct[myData.id-1].x = myData.x;
    //boardsStruct[myData.id-1].y = myData.y;
 //}
  //Serial.printf("x value: %d \n", boardsStruct[myData.id-1].x);
  //Serial.printf("y value: %d \n", boardsStruct[myData.id-1].y);
  //Serial.println();
}

String year,month,day,hour,mins,sec;

void setup() {
  display.init();
  display.clear();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16); 
  display.drawString(0, 0,"Connecting...");
  display.display();
  delay(1000); 
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); 
   while (WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
  };

  //初始化ESP_NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  //注册接收回调函数
  esp_now_register_recv_cb(OnDataRecv);
  display.clear();
  display.drawString(0, 0,"Connected...");
  display.display();
  delay(1000); 
  display.clear();
  display.display();
}
//所有数据转换为2位，如1为01，0为00
String num2wei(int a)
{
  String getstring;
  if(a==0)
    getstring="00";
  else if(a>0&&a<10)
    getstring="0"+String(a);
  else
    getstring=String(a);
  return getstring;
}
void loop() {
  display.clear();
  display.drawString(12*(1-1), 16*(1-1),String(board6.x));
  display.drawString(12*(5-1)+6, 16*(1-1),num2wei(board6.y/100));
  display.drawString(12*(8-1), 16*(1-1),num2wei(board6.y%100));
  display.drawString(12*(2-1), 16*(2-1),num2wei(board7.x/100));
  display.drawString(12*(4-1)+6, 16*(2-1),num2wei(board7.x%100));
  display.drawString(12*(7-1), 16*(2-1),num2wei(board7.y));
  switch(board5.x)
  {
    case 0: display.drawString(12*(1-1), 16*(3-1),"No people");display.drawString(12*(1-1), 16*(4-1),"Normal");break;
    case 1: display.drawString(12*(1-1), 16*(3-1),"attention");display.drawString(12*(1-1), 16*(4-1),"warden 1");break;
    case 2: display.drawString(12*(1-1), 16*(3-1),"attention");display.drawString(12*(1-1), 16*(4-1),"warden 2");break;
    case 3: display.drawString(12*(1-1), 16*(3-1),"attention");display.drawString(12*(1-1), 16*(4-1),"warden 3");break;
    case 4: display.drawString(12*(1-1), 16*(3-1),"attention");display.drawString(12*(1-1), 16*(4-1),"warden 3");break;
    case 0xA0: display.drawString(12*(1-1), 16*(3-1),"have find");display.drawString(12*(1-1), 16*(4-1),"stranger");break;
    deafult:display.drawString(12*(1-1), 16*(3-1),"No people");display.drawString(12*(1-1), 16*(4-1),"Normal");break;
  }
  board5.x=0;
  display.display();
  delay(5000); 
  display.clear();
  display.drawString(12*(1-1), 16*(1-1),"light");
  display.drawString(12*(6-1), 16*(1-1),String(board1.y));
  display.drawString(12*(1-1), 16*(2-1),"temp");
  display.drawString(12*(6-1), 16*(2-1),num2wei(board1.x/100));
  display.drawString(12*(8-1)+6, 16*(2-1),".");
  display.drawString(12*(9-1), 16*(2-1),num2wei(board1.x%100));
  display.drawString(12*(1-1), 16*(3-1),"stand");
  display.drawString(12*(6-1), 16*(3-1),String(board4.x));
  display.display();
  delay(3000);
  if(board1.y<1000||(board1.x/100)>board4.x+5||board1.y>3000||(board1.x/100)<board4.x-5)
  {
    if(board1.y<1000)
       display.drawString(12*(1-1), 16*(4-1),"dark");
    else if(board1.y>3000)
      display.drawString(12*(1-1), 16*(4-1),"light");
    if((board1.x/100)<board4.x-5)
       display.drawString(12*(6-1), 16*(4-1),"cold");
    else if((board1.x/100)>board4.x+5)
      display.drawString(12*(6-1), 16*(4-1),"hot");
  }
  else
    display.drawString(12*(1-1), 16*(4-1),"Normal");
  display.display();
  delay(3000);
}

