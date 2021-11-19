#define BLYNK_TEMPLATE_ID "TMPL2UsqGSFb"
#define BLYNK_DEVICE_NAME "AR smarthouse NodeMCU"
#define BLYNK_AUTH_TOKEN "0Q8dNguEb21BVHAjrfP3s7xyabRxqkmO"
#define BLYNK_PRINT Serial

#define ServoMainPin 16   // GPIO 16(D0)
#define ServoRestPin 14   // GPIO 14(D5)
#define InitAngle 90      // default Angle

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

char ssid[] = "RaspberryPanda";
char pass[] = "457a896a**";
char auth[] = BLYNK_AUTH_TOKEN;

int light_main = 0;
int light_restroom = 0;
Servo servo_main;
Servo servo_restroom;

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

BLYNK_WRITE(V0)
{
  light_main = param.asInt();
  controlMain(light_main);
}

BLYNK_WRITE(V1)
{
  light_restroom = param.asInt();
  controlRestroom(light_restroom);
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  servo_main.attach(ServoMainPin);
  servo_restroom.attach(ServoRestPin);

  // 각 서보모터 초기 설정
  servo_main.write(InitAngle);
  servo_restroom.write(InitAngle);
  /*
  Serial.println("\nInitial Main Light");
  controlMain(light_main);
  Serial.println("Intial Restroom Light");
  controlRestroom(light_restroom);  
  */
}

void loop()
{
  Blynk.run();
}
