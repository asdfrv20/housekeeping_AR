#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#define DHTTYPE DHT22
#define DHTPIN 2              //GPIO 2(D4)

//네트워크 변수 설정 
const char* ssid = "RaspberryPanda";  // 네트워크 설정
const char* password = "457a896a**";
uint8_t MAC_array[6];                 //MAC주소 출력을 위한 변수
char MAC_char[18];
String webString="";

//DHT 변수 설정
float Temp, Humi;
unsigned long previousMillis = 0;
const long interval = 1000;

// 미세먼지 검출기 Pin & 변수 설정
int measurePin = A0;   // Connect PM sensor to Arduino A0 pin 
int ledPower = 16;     // GPIO 16 (D0)

int samplingTime = 280; // 3개의 항목은 제조사에서 지정한 값(변경X)
int deltaTime = 40;     
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float Dust = 0;

ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  pinMode(ledPower, OUTPUT);

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

  //server 시작
  server.on("/temp", handletemp);
  server.on("/humi", handlehumi);
  server.on("/dust", handleDust);
  
  server.begin();
  Serial.println("HTTP server started.");
}

void getTempHumi(){
    Temp = dht.readTemperature();
    Humi = dht.readHumidity();
    if (isnan(Temp)|isnan(Humi)){
      Serial.println("Failed to read dht sensor");  
    }
    Serial.print("Temperature: ");
    Serial.print(Temp);
    Serial.print(" 'C, ");
    Serial.print("Humidity: ");
    Serial.print(Humi);
    Serial.print(" %, "); 
}

void getDust(){
  digitalWrite(ledPower, LOW);
  delayMicroseconds(samplingTime);
  
  voMeasured = analogRead(measurePin);    // read the dust value 

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH);           // turn the LED off
  delayMicroseconds(sleepTime);
  
  // 0-5V mapped to 0-1023 integer values
  // recover voltage
  calcVoltage = voMeasured *(5.0 / 1024.0);
  Dust = (0.17*calcVoltage - 0.1) * 1000;   // 단위: [ug/m^3]
  
  Serial.print("Dust Densitiy: ");
  Serial.print(Dust);
  Serial.println("ug/m^3");
}

void handletemp(){
  server.send(200, "text/plain", String(Temp));
}

void handlehumi(){
  server.send(200, "text/plain", String(Humi));
}

void handleDust(){
  server.send(200, "text/plain", String(Dust));
}

void loop() {
  getTempHumi();
  getDust();
  server.handleClient();
  delay(1000);
}
