//ESP8266 Based Patient Health Monitoring System
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>

#define REPORTING_PERIOD_MS     1000
/Put your SSID & Password/
const char* ssid = "Your ssid";  // Enter SSID here
const char* password = "Your pwd";  //Enter Password here

float BPM,SpO2;
PulseOximeter pox;
uint32_t tsLastReport = 0;
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(16, OUTPUT);
  delay(100);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  
}
void loop() {
  server.handleClient();
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();

    Serial.print("BPM: ");
    Serial.println(BPM);

    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");


    Serial.println("*");
    Serial.println();
    tsLastReport = millis();
  }


}

void handle_OnConnect() {

  server.send(200, "text/html", SendHTML( BPM, SpO2));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML( float BPM, float SpO2) {
String html = "<!DOCTYPE html>";
html += "<html>";
html += "<head>";
html += "<title>STERISCOUT</title>";
html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
html += "<link rel='stylesheet' type='text/css' href='styles.css'>";
html += "<style>";
html += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}";
html += "#page { margin: 20px; background-color: #fff;}";
html += ".container { height: inherit; padding-bottom: 20px;}";
html += ".header { padding: 20px;}";
html += ".header h1 { padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
html += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}";
html += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}";
html += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}";
html += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
html += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
html += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
html += ".sensor { margin: 12px 0px; font-size: 2.5rem;}";
html += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
html += ".units { font-size: 1.2rem;}";
html += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
html += "</style>";

//Ajax Code Start
  html += "<script>\n";
  html += "setInterval(loadDoc,1000);\n";
  html += "function loadDoc() {\n";
  html += "var xhttp = new XMLHttpRequest();\n";
  html += "xhttp.onreadystatechange = function() {\n";
  html += "if (this.readyState == 4 && this.status == 200) {\n";
  html += "document.body.innerHTML =this.responseText}\n";
  html += "};\n";
  html += "xhttp.open(\"GET\", \"/\", true);\n";
  html += "xhttp.send();\n";
  html += "}\n";
  html += "</script>\n";
  //Ajax Code END
  
html += "</head>";
html += "<body>";
html += "<div id='page'>";
html += "<div class='header'>";
html += "<h1>STERISCOUT</h1>";
html += "<h3><a href='https://iotprojectsideas.com'>https://theiotprojects.com</a></h3>";
html += "</div>";
html += "<div id='content' align='center'>";
html += "<div class='box-full' align='left'>";
html += "<h2>Sensors Readings</h2>";
html += "<div class='sensors-container'>";


//For Heart Rate
html += "<p class='sensor'>";
html += "<i class='fas fa-heartbeat' style='color:#cc3300'></i>";
html += "<span class='sensor-labels'> Heart Rate </span>";
html += (int)BPM;
html += "<sup class='units'>BPM</sup>";
html += "</p>";
html += "<hr>";

//For Sp02
html += "<p class='sensor'>";
html += "<i class='fas fa-burn' style='color:#f7347a'></i>";
html += "<span class='sensor-labels'> Sp02 </span>";
html += (int)SpO2;
html += "<sup class='units'>%</sup>";
html += "</p>";
html += "<hr>";

return html;
}