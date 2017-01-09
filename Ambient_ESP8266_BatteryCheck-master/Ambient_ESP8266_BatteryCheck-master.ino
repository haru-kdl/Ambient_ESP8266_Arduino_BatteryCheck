#include <ESP8266WiFi.h>
#include <Wire.h>
#include "Ambient.h"

extern "C" {
#include "user_interface.h"
}

#define _DEBUG 1
#if _DEBUG
#define DBG(...) { Serial.print(__VA_ARGS__); }
#define DBGLED(...) { digitalWrite(__VA_ARGS__); }
#else
#define DBG(...)
#define DBGLED(...)
#endif /* _DBG */

#define I2CAddressESPWifi 6
#define LED 16
#define SDA 4
#define SCL 5
#define RDY 12

#define PERIOD 5


const char* ssid = "SSID";
const char* password = "SSID-PASSWORD";

unsigned int channelId = 750;
const char* writeKey = "ambient-writeKey";

WiFiClient client;
Ambient ambient;

int x=32;

void setup()
{
    wifi_set_sleep_type(LIGHT_SLEEP_T);

#ifdef _DEBUG
    Serial.begin(115200);
    delay(20);
#endif
    DBG("\r\nStart\r\n");

    Wire.begin();  //マスターに設定

    pinMode(LED, OUTPUT);

    WiFi.begin(ssid, password);
    DBG("wifi connect ");

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DBGLED(LED, i++ % 2);
        DBG(".");
    }

    DBGLED(LED, LOW);
    DBG("WiFi connected\r\n");
    DBG("IP address: ");
    DBG(WiFi.localIP());
    DBG("\r\n");

    //ambient接続開始
    ambient.begin(channelId, writeKey, &client);
}

void loop()
{
    myReadLine(1);
    delay(PERIOD * 1000);
}

void myReadLine(int a){
    char charA_out[80];
    int cnt_buf = 0;

    // Slave側へデータ作成実行
    Wire.beginTransmission(I2CAddressESPWifi);
    Wire.write(x);
    Wire.endTransmission();
    x++;
    if(x >= 2147483647) x = 32;
    delay(1);//Wait for Slave to calculate response.

    // Slave側データ読み込み
    Wire.requestFrom(I2CAddressESPWifi,9);
    DBG("Request Return:[");
    if(Wire.available() > 0){
      cnt_buf = Wire.available();  //受信文字数
      for (int iii = 0; iii < cnt_buf; iii++){
        delay(1);
        charA_out[iii] = Wire.read();
      }
      charA_out[cnt_buf] = '\0';  //終端文字
      String c = charA_out;
      DBG(c);
    }
    DBG("]"); DBG("\r\n");
    snd_ambient(charA_out);
}

void snd_ambient(char indata[80]){
    // ambientへの送信準備
    char *charB;
    charB = strtok(indata,"|");    //1つ目のトークン表示
    ambient.set(1, charB);   // ambientへデータ1送信
    DBG("data1:"); DBG(charB); DBG("\r\n");
    while(charB!=NULL){          //トークンがNULLになるまでループ
      charB = strtok(NULL,"|");  //2回目以降は第一引数はNULL
      if(charB!=NULL){
        ambient.set(2, charB);   // ambientへデータ2送信
        DBG("data2:"); DBG(charB); DBG("\r\n");
      }
    }
    DBG("\r\n");

    // ambientへデータ送信
    DBGLED(LED, HIGH);
    ambient.send();
    DBGLED(LED, LOW);
}

