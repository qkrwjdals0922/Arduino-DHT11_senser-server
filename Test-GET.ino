#include <ESP8266WiFi.h>     
#include <DHT.h>          
#include <WiFiClient.h>     
#include <ESP8266HTTPClient.h> 
#include <ArduinoJson.h>   

const char* ssid = "박정민 핫스팟";  // WiFi SSID
const char* password = "qwer1234";  // WiFi 비밀번호

#define DHTPIN 2         // DHT11 데이터 핀 2
#define DHTTYPE DHT11    // DHT11 센서 사용

DHT dht(DHTPIN, DHTTYPE);

const char* serverAddress = "http://34.64.163.160/insert_db.php";
// 웹 서버 주소 설정 

void setup() {
  Serial.begin(115200);
  connectWiFi();
  dht.begin();
}

void loop() {
  String app_code = "IppTest"; 
  String car_value = "20";         // 주차장 포화도 임의 설정
  String barrier_value = "Y";     // 차수벽 on/off 상태 임의 설정
  float temp_value = dht.readTemperature(); // 온도 측정 
  float humi_value = dht.readHumidity();    // 습도 측정 

  // 센서값 측정

  DynamicJsonDocument json(256);
  json["app_code"] = app_code;
  json["car_value"] = car_value.toFloat();
  json["barrier_value"] = barrier_value;
  json["temp_value"] = String(temp_value, 1);  // 센서값 소수점 1의 자리에서 자르기
  json["humi_value"] = String(humi_value, 1);  // 센서값 소수점 1의 자리에서 자르기
  // 센서값을 json 변수에 저장 

String jsonString;
serializeJson(json, jsonString); // json을 문자열로 변환하여 jsonString에 저장 
Serial.println(jsonString); // JSON 문자열을 시리얼 모니터에 출력

  Serial.println(" ");
  Serial.print("습도: ");
  Serial.print(humi_value);
  Serial.print(" %\t");
  Serial.print("온도: ");
  Serial.print(temp_value);
  Serial.println(" *C");
  // 원본 센서값 출력 

  sendHttpRequest(app_code, car_value, barrier_value, temp_value, humi_value);
   // HTTP 요청 보내기
   
  delay(5000);
}

void connectWiFi() {
  Serial.print("와이파이 연결중... ");
  WiFi.begin(ssid, password);    

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  // 와이파이 연결 

  Serial.println("");
  Serial.println("와이파이 연결 완료");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void sendHttpRequest(
  String app_code, 
  String car_value, 
  String barrier_value, 
  float temp_value, 
  float humi_value) 
  {
  WiFiClient client;
  HTTPClient http;
  
  // HTTP 요청 설정
  String url = String(serverAddress) + "?";
  url += "app_code=" + app_code;
  url += "&car_value=" + car_value;
  url += "&barrier_value=" + barrier_value;
  url += "&temp_value=" + String(temp_value, 1);
  url += "&humi_value=" + String(humi_value, 1);
  
  http.begin(client, url);
  
  int httpResponseCode = http.GET(); // GET 방식 요청 보내기
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.println(url);
  } else {
    Serial.println("Error sending HTTP request");
  }
  http.end();
}
