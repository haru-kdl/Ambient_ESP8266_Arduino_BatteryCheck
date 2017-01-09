# ambient.ioを使って、電圧データを可視化する
* ESP8266
* Arduino pro mini 3.3V
* 放電機

これらを組み合わせ、バッテリーの放電状況をグラフ表示してみました。
ESP8266を Master側に、Arduinoを Slave側にしてI2C通信を行います。
