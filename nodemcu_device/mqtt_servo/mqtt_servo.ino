// MQTT를 활용한 nodeMCU-Raspberr Pi servo moter 제어
// Publish: Raspberry Pi
// Subscribe: nodeMCU

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

#define ServoMainPin D0   // GPIO 16
#define ServoRestPin D7   // GPIO 13
#define InitAngle 90      // default Angle


//WiFI 및 MQTT 변수 설정 
const char* ssid = "RaspberryPanda";            // Wif 연결을 위한 정보
const char* password = "457a896a**";
char* mqtt_server = "192.168.0.111";            // MQTT brocker server IP address
char* lightControlTopic = "raspberryPanda/control/servo";   // Light Control topic
//char* lightStatusTopic = "raspbeeryPnada/status/servo";     // Light Statues topic
char* clientID = "servo";

char msg[50];                  

WiFiClient espClient;
PubSubClient client(espClient);

// servo 모터 관련 설정 
bool light_main = false;
bool light_restroom = false;
Servo servo_main;
Servo servo_restroom;

void setup(){
  Serial.begin(115200);
  delay(10);
  servo_main.attach(ServoMainPin);
  servo_restroom.attach(ServoRestPin);
  
  Serial.println("\nInitial Main Light");
  controlMain(light_main);
  Serial.println("Intial Restroom Light");
  controlRestroom(light_restroom);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.connect(clientID);
  client.subscribe(lightControlTopic); 
}

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");  
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;

  Serial.println("Message arrived: topic = " + String(topic));
  Serial.println("Length: "+ String(length,DEC));

  //create character buffer with ending null terminator (string)
  for(i=0; i<length; i++){
    msg[i] = payload[i];
  }
  msg[i]= '\0';

  String msgString = String(msg);
  Serial.println("Payload: "+ msgString);
  
  //전송된 메시지가 "main"이면 light_main를 토글시키고 이에 따라 servo_main 동작(ON/OFF)
  if (msgString == "main"){
    light_main = !light_main;
    controlMain(light_main);
  }

  //전송된 메시지rk "restroom"이면 lgiht_restroom을 토글시키고 이에 따라 servo_restroom 동작(ON/OFF)
  if(msgString == "restroom"){
    light_restroom = !light_restroom;
    controlRestroom(light_restroom);
  }
}

void controlMain(bool light_main){
  if (light_main){
    servo_main.write(30);
    Serial.println("Main Light : ON");
    delay(1000);
    servo_main.write(InitAngle);
    delay(1000);
  }
  else {
    servo_main.write(150);
    Serial.println("Main Light : OFF");
    delay(1000);
    servo_main.write(InitAngle);
    delay(1000);
  }
}

void controlRestroom(bool light_restroom){
  if (light_restroom){
    servo_restroom.write(140);
    Serial.println("Restroom Light : ON");
    delay(1000);
    servo_restroom.write(InitAngle);
    delay(1000);
  }
  else {
    servo_restroom.write(50);
    Serial.println("Restroom Light : OFF");
    delay(1000);
    servo_restroom.write(InitAngle);
    delay(1000);
  }
}

void loop() {
  client.loop();
}
