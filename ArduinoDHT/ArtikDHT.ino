#include <WiFi101.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Web REST API params
char server[] = "api.artik.cloud";  
int port = 443; //(port 443 is default for HTTPS)

String AuthorizationData = "Authorization: Bearer 1d8a1c1806d64015948a726ef86f5cac";

float insTemp;
char buf[200];
#define LED 6
int ledState = 0;

WiFiSSLClient client;
char ssid[] = "TP-LINK_MAKERS";      //  your network SSID (name)
char pass[] = "MAKERS-NS";
int status = WL_IDLE_STATUS;

#define DHTPIN 5    // what pin we're connected to, pin1 is 5th pin from end
#define DHTTYPE DHT11  // DHT 11
DHT dht(DHTPIN,DHTTYPE);

void setup() {

  pinMode(LED,OUTPUT);

  Serial.begin(9600);
  while (status != WL_CONNECTED){
    Serial.println("Attempting to connect to WiFi");
    status = WiFi.begin(ssid,pass);
  }
  Serial.println("connected to WiFi ");
  
}
  
void loop() {
  Serial.println("loop");
  ledToggle();
  client.connect(server, port);
  delay(1000);
  if (!client.connected()) { 
    Serial.println(" error ");
  } else {
      int t = dht.readTemperature();
      int h = dht.readHumidity();
      Serial.println("Sending data"+String(t));
      client.println("POST /v1.1/messages HTTP/1.1");
      client.println("Host: api.artik.cloud");
      client.println("Accept: */*");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println(AuthorizationData);

       // Automated POST data section
       client.print("Content-Length: ");
       client.println(loadBuffer(t,h)); // loads buf, returns length
       client.println();
       client.println(buf);
       Serial.println("Data posted");
       client.stop();       
  }
 
  delay(5*60*1000);


  //exit(0);
}

int loadBuffer(int insTemp, int insHumid ) {  
   StaticJsonBuffer<200> jsonBuffer; // reserve spot in memory

   JsonObject& root = jsonBuffer.createObject(); // create root objects
     root["sdid"] = "afdd76ad8d0d4fbaac111a4c2abe2d6c"; // FIX 
     root["type"] = "message";

   JsonObject& dataPair = root.createNestedObject("data"); // create nested objects
     dataPair["temp"] = insTemp;  
     dataPair["humid"] = insHumid;

   root.printTo(buf, sizeof(buf)); // JSON-print to buffer

   return (root.measureLength()); // also return length
 }

 void ledToggle(){
  if (ledState == 0){
    digitalWrite(LED,LOW);
    ledState = 1;
  } else {
    digitalWrite(LED,HIGH);
    ledState = 0;
  }
 }

