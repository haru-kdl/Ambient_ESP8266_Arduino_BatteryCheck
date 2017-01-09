// ESP8266とArduino側で通信を行う
// Arduino側 Slave側
#include <Wire.h>
#define I2CAddressESPWifi 6

const int BTTIN1 = A0;    // 電池をA0ピンに入力
const int BTTIN2 = A1;    // 電池をA1ピンに入力
const float VREF = 3.3;  // Analog reference voltage is 5.0V
const float R1 = 30.0;   // 30kOhm is used
const float R2 = 130.0;  // 130kOhm is used
char response[10] ="0.12|0.12";

void setup()
{
  Serial.begin(115200);
  pinMode(5,INPUT_PULLUP); //内部プルアップ抵抗を使う
  Wire.begin(I2CAddressESPWifi);
  Wire.onReceive(espWifiReceiveEvent);
  Wire.onRequest(espWifiRequestEvent);
}

void loop()
{
  delay(1);
}

void espWifiReceiveEvent(int count)
{
  //Serial.print("Received[");
  while (Wire.available())
  {
    char c = Wire.read();
    //Serial.print(c);
  }
  //Serial.println("]");
  //calc response.
  String ts = getData();
  strncpy(response,ts.c_str(),sizeof(response)-1);
}

void espWifiRequestEvent()
{
  /*Serial.print("Sending:[");
  Serial.print(response);
  Serial.println("]");*/
  Wire.write(response);
}

String getData(){
  char data1buf[12], data2buf[12];
  String str;
  double a1 = analogRead(BTTIN1); // アナログポートより電圧値を取得
  if(a1<0.1) a1 = 0.1; // 0除算を回避
  a1 *= VREF ;
  a1 /= 1024.0;
//    a1 /= R1;  // 分圧回路の場合の抵抗値
//    a1 *= R2; // 分圧回路の場合の抵抗値

  double a2 = analogRead(BTTIN2); // アナログポートより電圧値を取得
  if(a2<0.1) a2 = 0.1; // 0除算を回避
  a2 *= VREF ;
  a2 /= 1024.0;

  // 送信データ作成
  dtostrf(a1, 3, 2, data1buf);
  dtostrf(a2, 3, 2, data2buf);
  str = data1buf;
  str = str + "|";
  str = str + data2buf;
  //Serial.println(str);
  return(str);
}
