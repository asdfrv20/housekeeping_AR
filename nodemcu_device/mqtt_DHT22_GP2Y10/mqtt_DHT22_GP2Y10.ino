#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTTYPE DHT22
#define DHTPIN 2

//WiFI 및 MQTT 변수 설정 
const char* ssid = "RaspberryPanda";
const char* password = "457a896a**";
//const char* mqtt_server = "192.168.0.111";  // Raspberry Pi3를 서버로 사용하는 경우
const char* mqtt_server = "192.168.0.102";    // Windows(Notebook)를 서버로 사용하는 경우
const char* topic = "raspberryPanda/sensor/dhtDust";
const char* clientID = "dhtDust";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[75];
String packet;

// DHT 온습도 관련 변수 설정
float Humi, Temp; 
DHT dht(DHTPIN, DHTTYPE);

// 미세먼지 검출기 Pin & 변수 설정
int measurePin = A0;   // Connect PM sensor to Arduino A0 pin 
int ledPower = 16;     // GPIO 16 (D0)

int samplingTime = 280; // 3개의 항목은 제조사에서 지정한 값(변경X)
int deltaTime = 40;     
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

float DustDensity;

void setup(){
  Serial.begin(115200);
  dht.begin();
  pinMode(ledPower, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.connect(clientID);
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

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");
  for (int i=0; i<length; i++){
    Serial.print((char)payload[i]);  
  }  
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1'){
    digitalWrite(BUILTIN_LED, LOW);  
  }else{
    digitalWrite(BUILTIN_LED, HIGH);  
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

float getHumi() {         //DHT22 습도를 받아오는 함수
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  return(h);
}

float getTemp() {       //DHT22 온도를 받아오는 함수
  float t = dht.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t");

  return(t);
}

// particlaeSensing: 미세먼지 검출 함수
float particleSensing(){
  digitalWrite(ledPower, LOW);
  delayMicroseconds(samplingTime);
  
  voMeasured = analogRead(measurePin);    // read the dust value 

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH);           // turn the LED off
  delayMicroseconds(sleepTime);
  
  // 0-5V mapped to 0-1023 integer values
  // recover voltage
  calcVoltage = voMeasured *(5.0 / 1024.0);
  dustDensity = (0.17*calcVoltage - 0.1) * 1000;   // 단위: [ug/m^3]
  
  Serial.print("Dust Densitiy: ");
  Serial.println(dustDensity);
  
  return dustDensity;
}

// MQTT publish: MQTT 방식으로 publishing을 하는 함수
void mqtt_publish(float Humi, float Temp, float DustDensity){
  if(!client.connected()){
    reconnect();  
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000){
    lastMsg = now;

    packet = "{ \"temperature\": " + String(Temp) + ", \"humiditiy\": " + String(Temp) + ", \"dustdensity\": " + String(DustDensity) + " }";
    packet.toCharArray(msg, 75);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic, msg);
  }
  delay(2000); 
}

void loop(){
  Humi = getHumi();
  Temp = getTemp();
  DustDensity = particleSensing();
  
  mqtt_publish(Humi, Temp, DustDensity);
}
