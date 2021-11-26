#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "HX711.h"

#define calibration_factor - 7050
#define DOUT D5
#define CLK D6

// 네트워크 변수 설정 
const char* ssid = "RaspberryPanda";  // 네트워크 설정
const char* password = "457a896a**";

ESP8266WebServer server(80);

// loadcell 관련 변수 설정
float Weight;
HX711 scale;

// LED 관련 변수 설정(LED 애노드 타입[+극이 3색 공통])
int R = 16;   //GPIO 16(D0)
int G = 4;    //GPIO 4(D2)
int B = 5;    //GPIO 5(D1)

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);

  // WiFi 접속
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/weight", handleweight);

  server.begin();
  Serial.println("HTTP server started.");
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
  
  return(w);  
}

void handleweight(){
    server.send(200, "text/plain", String(Weight));
}

void loop() {
  Weight = getWeight();
  controlLED(Weight);
  server.handleClient();
  delay(1000);
}
