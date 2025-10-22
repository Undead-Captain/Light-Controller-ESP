#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

// LED pins
const int led1 = D1;
const int led2 = D2;
int brightness1 = 0;
int brightness2 = 0;

String htmlPage() {
  String page = "<!DOCTYPE html><html><head>";
  page += "<title>ESP8266 LED Control</title>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += "<style>body{text-align:center;font-family:Arial;} ";
  page += ".slider{width:80%;} button{padding:10px 20px;margin-top:20px;font-size:16px;}</style></head><body>";
  page += "<h2>ESP8266 AP LED Controller</h2>";

  // LED 1
  page += "<p>LED 1</p>";
  page += "<input type='range' min='0' max='255' value='" + String(brightness1) + 
          "' class='slider' oninput='updateLED(1,this.value)'><br>";

  // LED 2
  page += "<p>LED 2</p>";
  page += "<input type='range' min='0' max='255' value='" + String(brightness2) + 
          "' class='slider' oninput='updateLED(2,this.value)'><br>";

  // RESET button
  page += "<button onclick='resetLEDs()'>RESET (All OFF)</button>";

  // Javascript
  page += "<script>";
  page += "function updateLED(led,val){";
  page += "var xhttp=new XMLHttpRequest();";
  page += "xhttp.open('GET','/set?led='+led+'&val='+val,true);";
  page += "xhttp.send();}";
  page += "function resetLEDs(){";
  page += "var xhttp=new XMLHttpRequest();";
  page += "xhttp.open('GET','/reset',true);";
  page += "xhttp.send();}";
  page += "</script>";

  page += "</body></html>";
  return page;
}

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // Start Access Point
  WiFi.softAP("ESP8266_LED_AP", "pass5678"); // SSID, Password
  Serial.begin(115200);
  Serial.println("Access Point started!");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Web server routes
  server.on("/", []() {
    server.send(200, "text/html", htmlPage());
  });

  server.on("/set", []() {
    if (server.hasArg("led") && server.hasArg("val")) {
      int led = server.arg("led").toInt();
      int val = server.arg("val").toInt();
      if (led == 1) { brightness1 = val; analogWrite(led1, brightness1); }
      if (led == 2) { brightness2 = val; analogWrite(led2, brightness2); }
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/reset", []() {
    brightness1 = 0;
    brightness2 = 0;
    analogWrite(led1, 0);
    analogWrite(led2, 0);
    server.send(200, "text/plain", "All LEDs OFF");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
