// MQTT를 활용한 nodeMCU-Raspberr Pi LED 제어
// Publish: Raspberry Pi
// Subscribe: nodeMCU

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "RaspberryPanda";  // Wif 연결을 위한 정보
const char* password = "457a896a**";

char* topic = "raspberryPanda/led";   // topic: #로 하면 모든 토픽으로부터 수신을 한다.
char* server = "192.168.0.111";       // MQTT brocker server IP address

char message_buff[100];               //initialise storage buffer

WiFiClient wifiClient;                //클라이언트로 작동
int ledPin = 4;                       //GPIO 4

// callback(): 메세지를 받았을 때, Serial 통신으로 그 정보를 표시해주는 함수
void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;

  Serial.println("Message arrived: topic = " + String(topic));
  Serial.println("Length: "+ String(length,DEC));

  //create character buffer with ending null terminator (string)
  for(i=0; i<length; i++){
    message_buff[i] = payload[i];
  }
  message_buff[i]= '\0';

  String msgString = String(message_buff);
  Serial.println("Payload: "+ msgString);
  int state = digitalRead(ledPin);
  //전송된 메시가 "led"이면 LED를 받을 때마다 켜고 끈다.(토글)
  if (msgString == "led"){
    digitalWrite(ledPin, !state);
    Serial.println("Switching LED");
  }
}

PubSubClient client(server, 1883, callback, wifiClient);   // MQTT 시작  

void setup() {
  Serial.begin(115200);
  delay(10);
  
  pinMode(ledPin, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(ledPin, LOW);   // LED off

  //Connect to wifi my network;
  Serial.println();
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //wifi에 연결을 시도한다.
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  //연결되면 접속한 IP가 뭔지 출력한다.
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  //Connection to broker
  if (client.connect("arduinoClient")){ //arduinoClient는 어디서 온거지?
    client.publish("nodeMCU", "Hello World");
    client.subscribe(topic);
  }
}

void loop() {
  client.loop(); 
}
