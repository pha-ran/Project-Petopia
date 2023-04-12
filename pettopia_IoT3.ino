#include <AWS_IOT.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <HttpClient.h>
#include <ESP32_Servo.h>
#define SEALEVELPRESSURE_HPA (1013.25)

//사물 - 짱이 사이 거리 초음파
const int trigPin = 22; //초음파 센서
const int echoPin = 23; //초음파 센서
//물 거리 초음파
const int trigPin_w = 18; //초음파 센서
const int echoPin_w = 19; //초음파 센서
//사료 거리 초음파
const int trigPin_f = 16; //초음파 센서
const int echoPin_f = 17; //초음파 센서

//본체led
const int led_main = 5;

//초음파 관련 변수
int disi = 0; //초음파 센서 거리 
long duration, distance;
//물통 초음파
long duration_w, distance_w;
//사로툥 초음파
long duration_f, distance_f;
bool boolWater, boolFood = 0; //1이면 공급

//서보 모터 + 워터 펌프
Servo servomotor;
static const int pinPump= 4;  
static const int servoPin = 15;
int posDegrees=0;
//모터 제어
int motor_state=1;
int pump_state=1;


//Server
//String base_uri = "http://AWS_인스턴스_퍼블릭_IP:5000";
String base_uri = "http://54.180.2.73:5000";

// 학교, 시드 동아리방 와이파이는 서버 통신이 안될 위험이 큼. 핫스팟이나 집 와이파이로 하기 (서버, ESP32 모두)
// const char* ssid = "House"; 와이파이 이름 (아이폰 핫스팟의 경우에 접속이 안되면 호환성 모드 켜야함)
// const char* password = "tkanfkdl98"; 와이파이 비밀번호
const char* ssid = "gyu123";
const char* password = "t12345678";

// 급이 OR 급수 횟수 카운팅 (필요시 POST하는 JSON에 추가하면 됨)
int pump_count = 0;
int motor_count = 0;

void setup()
{
  bool status;
  Serial.begin(115200);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to wifi");
  
  //서보모터 + 워터펌프
  servomotor.attach(servoPin);
  pinMode(pinPump, OUTPUT);  
  digitalWrite(pinPump, LOW);
   
  //초음파 센서
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin_w, OUTPUT);
  pinMode(echoPin_w, INPUT);
  pinMode(trigPin_f, OUTPUT);
  pinMode(echoPin_f, INPUT);
  
  //led
  pinMode(led_main, OUTPUT);
  Serial.println("motor reset");
  servomotor.write(5); // 모터의 각도를 설정합니다.
  delay(100);
}

//본체 - 애완동물 초음파 
int MechToPet()
{
  digitalWrite(trigPin, LOW); // trig low for 2us
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // trig high for 10us
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 17 / 1000;
  if(distance <= 10){
      Serial.println();
      disi = disi +1;
      Serial.print("Count = ");
      Serial.println(disi);
    }else if(distance>10){    
      disi=0;
      Serial.println(disi);
    }
    Serial.print("Pet 거리 = ");
    Serial.println(distance);
    return disi;
}


//본체 - 물통 초음파 
bool MechToWater()
{
  digitalWrite(trigPin_w, LOW); // trig low for 2us
  delayMicroseconds(2);
  digitalWrite(trigPin_w, HIGH); // trig high for 10us
  delayMicroseconds(10);
  digitalWrite(trigPin_w, LOW);
  duration_w = pulseIn(echoPin_w, HIGH);
  distance_w = duration_w * 17 / 1000;
  Serial.print("물통 거리 = ");
  Serial.println(distance_w);
  if(distance_w <= 14){ //특정 거리 이하면 공급 중단
    boolWater = 0;
  }else if(distance_w >14){
    boolWater = 1;  
  }
  return boolWater;
}

//본체 - 사료통 초음파 
bool MechToFood()
{
  digitalWrite(trigPin_f, LOW); // trig low for 2us
  delayMicroseconds(2);
  digitalWrite(trigPin_f, HIGH); // trig high for 10us
  delayMicroseconds(10);
  digitalWrite(trigPin_f, LOW);
  duration_f = pulseIn(echoPin_f, HIGH);
  distance_f = duration_f * 17 / 1000;
  Serial.print("사료통 거리 = ");
  Serial.println(distance_f);
  if(distance_f <= 12){ //특정 거리 이하면 공급 중단
    boolFood = 0;
  }else if(distance_f > 12){
    boolFood = 1;  
  }
  return boolFood;
}

// 서버에서 밥 주기 or 물 주기 받았을 때 1로 세팅 (0 이면 배급 X)
bool foodIsOn = 0;
bool waterIsOn = 0;

void doFood()
{
  while (foodIsOn) {
    Serial.println("사료를 줍니다.");
    Serial.println("-------Food Motor On-------");
    for(posDegrees = 5; posDegrees <= 80; posDegrees++) {
      servomotor.write(posDegrees); // 사료통 열기
      delay(20);
    }
    delay(3000);
    for(posDegrees = 80; posDegrees >= 0; posDegrees--) {
      servomotor.write(posDegrees); // 사료통 닫기
      delay(20);
    }
    motor_count++;
    delay(500);
    MechToFood();
    if (!boolFood) { // 그릇에 사료가 충분하면 종료
      foodIsOn = 0;
      delay(20);
      Serial.println("사료 배급 완료.");
      Serial.println("-------Food Motor Off-------");
    } 
  }
}

void doWater()
{
  while (waterIsOn) {
    Serial.println("물을 줍니다.");
    Serial.println("-------Water Pump On-------");
    digitalWrite(pinPump, HIGH);
    delay(2500);
    digitalWrite(pinPump, LOW);
    pump_count++;
    delay(2000);
    MechToWater();
    if (!boolWater) {
      waterIsOn = 0;
      delay(20);
      Serial.println("물 배급 완료.");
      Serial.println("-------Water Pump Off-------");
    }
  }
}

void WaitServer() 
{
  String uri = base_uri + "/wait";
  HTTPClient http;
  http.begin(uri);
  
  String jsonData = "";
  http.addHeader("Content-Type", "application/json");
  String httpRequestData = "{\"remain_food\":\"" + String(boolFood) + "\",\"remain_water\":\"" + String(boolWater) +"\"}";
  int responce = http.POST(httpRequestData);
  String result = http.getString();
  Serial.println(result);
  http.end();

  if (result == "food") { // 로블록스에서 먹이 주기
    foodIsOn = 1;
    doFood();
    
  } else if (result == "water") { // 로블록스에서 물 주기
    waterIsOn = 1;
    doWater();
    
  }
}

void loop()
{
  MechToPet();
  MechToWater();
  MechToFood();
  delay(1000);
  Serial.println("-------센서 측정 완료-------");
  WaitServer();
  delay(1000);
  Serial.println("-------서버 통신 완료-------");
  
}
