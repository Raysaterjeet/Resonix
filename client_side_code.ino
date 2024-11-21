#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
//#include <AsyncTCP.h>

#define RELAY_PIN 4
const char *ssid = "Resonix";
const char *password = "12345678";
const char *serverIP="192.168.4.1";

// Create an instance of the web server
AsyncWebServer server(80);

int delayTime = 100;
bool conditionMet = false;
int executionTime = 0;
String flag;
bool permission = false;
String finalFrequency;

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  // Serve the HTML page
  String html = "<!DOCTYPE html>";
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  server.on("/sounds/connected.mp3", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/sounds/connected.mp3", "audio/mpeg"); });
  server.on("/sounds/optimizing.mp3", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/sounds/optimizing.mp3", "audio/mpeg"); });
  // Start the server
  server.begin();
  WiFiClient client;
  int range = random(5, 10);
  Serial.println(range);
  float delayTimeFloat = (range * 2100) / 100.0;
  delayTime = int(delayTimeFloat);
  server.on("/setVariable", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      String dataParam = request->arg("data");
      flag = dataParam; // Store the data from JavaScript
      request->send(200, "text/plain", "Variable set to: " + flag);
      permission=true;
      serverIP="192.168.4.1"; });//this is the true server ip which means the server will only connect when connect button is pressed on the web server
      server.on("/run-script", HTTP_POST, [](AsyncWebServerRequest *request){
         // Execute your external Python script here
         system("python /data/python.py");
          
          // Send a response back to the client
          request->send(200, "text/plain", "executing ai script...");
        });
        /*****************************************************************/
          server.on("/getFinalFrequency", HTTP_GET, [](AsyncWebServerRequest *request)
                {
//              String value=String(finalFrequency);
                String value="check";
//              Serial.println(value);
              request->send(200,"text/plain",value); });
          /*****************************************************************/
        // Start the server
        server.begin();
  while (true)
  {
    if (client.connect(serverIP, 80))
    {
      Serial.println("Connected to the server");
      server.on("/checkCondition", HTTP_GET, [](AsyncWebServerRequest *request)
                {
              conditionMet = true;
              // Send the condition status as JSON
              String response = conditionMet ? "{\"conditionMet\":true}" : "{\"conditionMet\":false}";
              request->send(200, "application/json", response); });
      server.on("/getVariableValue", HTTP_GET, [](AsyncWebServerRequest *request)
                {
              String value=String(delayTime);
//              Serial.println(value);
              request->send(200,"text/plain",value); });
      delay(2000);
      int startTime = millis();
      for (int i = 1; i <= range; i++)
      {
        int frequency = random(3.5, 300);
        client.print(frequency);
        delay(1000);
        String receivedFrequency = client.readString();
        if (receivedFrequency != "")
        {
          Serial.println("Received frequency: " + receivedFrequency + "Ghz");
          finalFrequency = receivedFrequency;
        }
        else
        {
          if (!(client.connect(serverIP, 80)))
          {
            Serial.println("Disconnected from the server");
          }
          else
          {
            Serial.println("No response from server!");
          }
        }
        if (i == 9)
        {
          break;
        }
      }
      delay(2000);
      int endTime = millis();
      executionTime = (endTime - startTime);
      Serial.println();
      Serial.println("DATA RECEIVED");
      Serial.println();
      Serial.println("Final frequency is: " + finalFrequency + "Ghz");
      Serial.println((String) "Time taken in optimizing frequency is: " + (executionTime / 1000) + "s");
      digitalWrite(RELAY_PIN, LOW);
      while (true)
      {
        if (!(client.connect(serverIP, 80)))
        {
          digitalWrite(RELAY_PIN, HIGH);
        }
      }
    }
    else
    {
      Serial.println("not Connected to the server yet");
      continue;
    }
    // while(true){}
    //        client.stop();
    break;
  }
}
void loop()
{
}
