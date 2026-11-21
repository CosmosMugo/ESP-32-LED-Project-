#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "John Cena";
const char* password = "abcd1234";

const int LED_PIN = 13;

WebServer server(80);

bool ledState = false;
bool blinking = false;
unsigned long lastToggle = 0;
const unsigned long blinkInterval = 500; // ms

String getStatus() {
  if (blinking) return "Blinking";
  return ledState ? "ON" : "OFF";
}

String buildPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>LED Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: #f3ecff;
      margin: 0;
      padding: 20px;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      box-sizing: border-box;
    }
    .card {
      background: #ffffff;
      padding: 30px;
      border-radius: 20px;
      box-shadow: 0 4px 20px rgba(160, 120, 220, 0.25);
      text-align: center;
      width: 100%;
      max-width: 340px;
    }
    h1 { color: #8a5cd0; font-size: 24px; margin-top: 0; }
    .status { color: #6b4aa0; font-size: 18px; margin-bottom: 20px; }
    a {
      display: block;
      padding: 14px;
      margin: 12px 0;
      background: #b794f4;
      color: #ffffff;
      text-decoration: none;
      font-size: 18px;
      border-radius: 12px;
    }
    a:active { background: #9f7aea; }
  </style>
</head>
<body>
  <div class="card">
    <h1>LED Control</h1>
    <div class="status">Status: <b>)rawliteral";

  html += getStatus();

  html += R"rawliteral(</b></div>
    <a href="/on">Turn ON</a>
    <a href="/off">Turn OFF</a>
    <a href="/blink">Blink</a>
  </div>
</body>
</html>
)rawliteral";

  return html;
}

void redirectHome() {
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRoot() {
  server.send(200, "text/html", buildPage());
}

void handleOn() {
  blinking = false;
  ledState = true;
  digitalWrite(LED_PIN, HIGH);
  redirectHome();
}

void handleOff() {
  blinking = false;
  ledState = false;
  digitalWrite(LED_PIN, LOW);
  redirectHome();
}

void handleBlink() {
  blinking = true;
  lastToggle = millis();
  redirectHome();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.softAP(ssid, password);
  Serial.print("Hotspot started. Open http://");
  Serial.println(WiFi.softAPIP()); // usually 192.168.4.1

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/blink", handleBlink);
  server.begin();
}

void loop() {
  server.handleClient();

  if (blinking && millis() - lastToggle >= blinkInterval) {
    lastToggle = millis();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}
