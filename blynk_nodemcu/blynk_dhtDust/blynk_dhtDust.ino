#define BLYNK_TEMPLATE_ID "TMPL2UsqGSFb"
#define BLYNK_DEVICE_NAME "AR smarthouse NodeMCU"
#define BLYNK_AUTH_TOKEN "0Q8dNguEb21BVHAjrfP3s7xyabRxqkmO"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "RaspberryPanda";
char pass[] = "457a896a**";

#define DHTTYPE DHT22
#define DHTPIN 2

DHT dht(DHTPIN, DHTTYPE);

// 미세먼지 검출기 Pin & 변수 설정
int measurePin = A0;   // Connect PM sensor to Arduino A0 pin 
int ledPower = 16;     // GPIO 16 (D0)

int samplingTime = 280; // 3개의 항목은 제조사에서 지정한 값(변경X)
int deltaTime = 40;     
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;

BlynkTimer timer;

void sendDHTDust() {                //DHT22 온습도 측정 및 전송
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float dustDensity = 0;

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  digitalWrite(ledPower, LOW);
  delayMicroseconds(samplingTime);
  
  voMeasured = analogRead(measurePin);    // read the dust value 

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH);           // turn the LED off
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured *(5.0 / 1024.0);
  dustDensity = (0.17*calcVoltage - 0.1) * 1000;
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("Dust Densitiy: ");
  Serial.println(dustDensity);
  
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V4, t);
  Blynk.virtualWrite(V6, dustDensity);
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  pinMode(ledPower, OUTPUT);

  timer.setInterval(1000L, sendDHTDust);   //시간 단위: [ms]
}

void loop()
{
  Blynk.run();
  timer.run();
}
