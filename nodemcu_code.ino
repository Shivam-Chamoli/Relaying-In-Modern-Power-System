#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SoftwareSerial.h>
SoftwareSerial s(D6,D5);

#define WIFI_SSID "ded"
#define WIFI_PASSWORD "123456789"
#define tempVal A0
#define theftPinR D2
#define theftPinY D3
#define theftPinB D4

#define FIREBASE_HOST "https://relaying-e6b69-default-rtdb.firebaseio.com/"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "Qo0WYGSYx053WaMCNih2lbOt8XUUMmDwA5khwAKy"

//Define FirebaseESP8266 data object
FirebaseData fbdo;

FirebaseJson json;

void printResult(FirebaseData &data);

char c;
String dataIn;
int8_t indexofR, indexofY, indexofB;

unsigned long power_send_time;



float data1, data2, data3;

// Set these to run example.
#define FIREBASE_HOST "https://test-b29f1-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "RS7Dy2cwmSlfOWZHw6x7a6ASVJwAzre6kQZ9grwm"
#define WIFI_SSID "ded"
#define WIFI_PASSWORD "123456789"
 
void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  s.begin(115200);

  pinMode(theftPinR,OUTPUT);
  pinMode(theftPinY,OUTPUT);
  pinMode(theftPinB,OUTPUT);

  digitalWrite(theftPinR, LOW);
  digitalWrite(theftPinY, LOW);
  digitalWrite(theftPinB, LOW);

  ///////firebase

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  fbdo.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  fbdo.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");

  //optional, set the decimal places for float and double data to be stored in database
  Firebase.setFloatDigits(2);
  Firebase.setDoubleDigits(6);

  /*
  This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
  Firewall that allows only GET and POST requests.
  
  Firebase.enableClassicRequest(fbdo, true);
  */
  String path = "/Test";
}
 
void loop() {

   if (Firebase.getBool(fbdo, "Project/theftFlags/RPhase")) {
    if (fbdo.dataType() == "boolean"){
      Serial.println(fbdo.boolData() == 1 ? "true" : "false");
      if(fbdo.boolData() == 1) digitalWrite(theftPinR, HIGH);
    }  
   }
   if (Firebase.getBool(fbdo, "Project/theftFlags/YPhase")) {
    if (fbdo.dataType() == "boolean"){
      Serial.println(fbdo.boolData() == 1 ? "true" : "false");
      if(fbdo.boolData() == 1) digitalWrite(theftPinY, HIGH);
    }  
   }
   if (Firebase.getBool(fbdo, "Project/theftFlags/BPhase")) {
    if (fbdo.dataType() == "boolean"){
      Serial.println(fbdo.boolData() == 1 ? "true" : "false");
      if(fbdo.boolData() == 1) digitalWrite(theftPinB, HIGH);
    }  
   }
     
  
  while(s.available()>0){
        c = s.read();
        if(c=='\n') break;
        else{
          dataIn+=c;
          }
    }
    if(c='\n'){
      if(dataIn.length()>0){
          indexofR = dataIn.indexOf("R");
          indexofY = dataIn.indexOf("Y");
          indexofB = dataIn.indexOf("B");
        
          String data1str= dataIn.substring(0,indexofR);
          String data2str= dataIn.substring(indexofR+1,indexofY);
          String data3str= dataIn.substring(indexofY+1,indexofB);
        
          data1=data1str.toFloat();
          data2=data2str.toFloat();
          data3=data3str.toFloat();
        
          delay(10);
      
          Serial.println("Data1=" + data1str);
          Serial.println("Data2=" + data2str);
          Serial.println("Data3=" + data3str);
  
  
        Serial.println("XXXXXXXXXXXXX__________________XXXXXXXXXXXXXXX");
        c='0';
        dataIn="";
       }
    }

    if(millis()-power_send_time>10000){
        if (Firebase.setFloat(fbdo, "Project/Phases/PhaseR", data1)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
          Serial.println("ETag: " + fbdo.ETag());
          Serial.print("VALUE: ");
          printResult(fbdo);
          Serial.println("------------------------------------");
          Serial.println();
        }
        else
        {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
          Serial.println("------------------------------------");
          Serial.println();
        }
        delay(500);
        if (Firebase.setFloat(fbdo, "Project/Phases/PhaseY", data2)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
          Serial.println("ETag: " + fbdo.ETag());
          Serial.print("VALUE: ");
          printResult(fbdo);
          Serial.println("------------------------------------");
          Serial.println();
        }
        else
        {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
          Serial.println("------------------------------------");
          Serial.println();
        }
        delay(500);
         if (Firebase.setFloat(fbdo, "Project/Phases/PhaseB", data3)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
          Serial.println("ETag: " + fbdo.ETag());
          Serial.print("VALUE: ");
          printResult(fbdo);
          Serial.println("------------------------------------");
          Serial.println();
        }
        else
        {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
          Serial.println("------------------------------------");
          Serial.println();
        }
        delay(500);  

       power_send_time=millis();
    }
}

//////firebase

void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
  else if (data.dataType() == "blob")
  {

    Serial.println();

    for (int i = 0; i < data.blobData().size(); i++)
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      if (i < 16)
        Serial.print("0");

      Serial.print(data.blobData()[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  else if (data.dataType() == "file")
  {

    Serial.println();

    File file = data.fileStream();
    int i = 0;

    while (file.available())
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      int v = file.read();

      if (v < 16)
        Serial.print("0");

      Serial.print(v, HEX);
      Serial.print(" ");
      i++;
    }
    Serial.println();
    file.close();
  }
  else
  {
    Serial.println(data.payload());
  }
}
