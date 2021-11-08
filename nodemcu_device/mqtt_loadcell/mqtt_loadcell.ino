#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

#define calibration_factor - 7050
#define DOUT D5
#define CLK D6

//WiFi 및 MQTT 변수 설정 
const char* ssid = "RaspberryPanda";
const char* password = "457a896a**";
const char* mqtt_server = "192.168.0.111";
const char* topic = "raspberryPanda/sensor/loadcell";
const char* clientID = "loadcell";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
String packet;

// loadcell 관련 변수 설정
float Weight;
HX711 scale;

// LED 관련 변수 설정(LED 애노드 타입[+극이 3색 공통])
int R = 16;   //GPIO 16(D0)
int G = 4;    //GPIO 4(D2)
int B = 5;    //GPIO 5(D1)

void setup() {
  Serial.begin(115200);
  
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);  
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.connect(clientID);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  //Loop until we're recnnected
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Clinet")){
      Serial.println("connectd");
      client.publish("outTopic", "hello world");
      client.subscribe("inTopci");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }  
}

// LED 제어 - LED 애노드 타입이므로 신호 반대로 줄 것.
void controlLED(float w){

  if(w<=1){                //1kg 이하: Blue
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);
    delay(100);
  }else if(w<=5){          // 1kg~5kg: Green
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);
    delay(100);
  } else{                  // 5kg 초과: Red
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);
    delay(100);
  } 
}

float getWeight(){
  float w = scale.get_units()*0.07267;
  
  if(isnan(w)){
    Serial.println("Failed to read weight sensor."); 
  }

  Serial.print("Weight: ");
  Serial.print(w);
  Serial.println(" kg");

  controlLED(w);
  
  return(w);  
}

void mqtt_publish(float Weight){
  if(!client.connected()){
    reconnect();  
  }
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    packet = "{ \"weight\" : " + String(Weight) + " }"; 
    packet.toCharArray(msg, 50); 
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic, msg);
  }
  delay(2000); //2초 단위로 Publishing
}

void loop(){
  Weight = getWeight();
  mqtt_publish(Weight);
}
