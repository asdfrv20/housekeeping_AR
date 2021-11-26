#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

#define ServoMainPin 16   // GPIO 16(D0)
#define ServoRestPin 14   // GPIO 14(D5)
#define InitAngle 90      // default Angle

// 네트워크 변수 설정 
const char* ssid = "RaspberryPanda";  // 네트워크 설정
const char* password = "457a896a**";

ESP8266WebServer server(80);

// light control
int light_main = 0;
int light_restroom = 0;
Servo servo_main;
Servo servo_restroom;

void setup()
{
  // 시리얼 setting
  Serial.begin(115200);
  Serial.println();

  // servo motor setting
  servo_main.attach(ServoMainPin);
  servo_restroom.attach(ServoRestPin);

  servo_main.write(InitAngle);
  servo_restroom.write(InitAngle);

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

  server.on("/main_on", handleMainOn);
  server.on("/main_off", handleMainOff);
  server.on("/restroom_on", handleRestOn);
  server.on("/restroom_off", handleRestOff);

  server.begin();
  Serial.println("HTTP server started.");
}

void controlMain(int light_main){
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

void controlRestroom(int light_restroom){
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

void handleMainOn(){
  light_main = 1;
  controlMain(light_main);
  server.send(200, "text/plain", "Main Light On");
}

void handleMainOff(){
  light_main = 0;
  controlMain(light_main);
  server.send(200, "text/plain", "Main Light Off");
}

void handleRestOn(){
  light_restroom = 1;
  controlRestroom(light_restroom);
  server.send(200, "text/plain", "Restroom Light On");
}

void handleRestOff(){
  light_restroom = 0;
  controlRestroom(light_restroom);
  server.send(200, "text/plain", "Restroom Light Off");
}

void loop() {
  server.handleClient();  
}
