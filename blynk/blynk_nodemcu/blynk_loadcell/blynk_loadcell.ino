#define BLYNK_TEMPLATE_ID "TMPL2UsqGSFb"
#define BLYNK_DEVICE_NAME "AR smarthouse NodeMCU"
#define BLYNK_AUTH_TOKEN "0Q8dNguEb21BVHAjrfP3s7xyabRxqkmO"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "HX711.h"

#define calibration_factor - 7050
#define DOUT 14   // GPIO 14(D5)
#define CLK 12    // GPIO 12(D6)

char auth[] = "0Q8dNguEb21BVHAjrfP3s7xyabRxqkmO";
char ssid[] = "RaspberryPanda";
char pass[] = "457a896a**";

// Blynk, loadcell 변수
BlynkTimer timer;
HX711 scale;
 
// LED 관련 변수 설정(LED 애노드 타입[+극이 3색 공통])
int R = 16;   //GPIO 16(D0)
int G = 4;    //GPIO 4(D2)
int B = 5;    //GPIO 5(D1)

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

void sendWeight(){
  float w = scale.get_units()*0.07267;
  
  if(isnan(w)){
    Serial.println("Failed to read weight sensor."); 
  }
  Serial.print("Weight: ");
  Serial.print(w);
  Serial.println(" kg");

  controlLED(w);
  
  Blynk.virtualWrite(V7, w);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);

  timer.setInterval(1000L, sendWeight);
}

void loop() {
  Blynk.run();
  timer.run();
}
