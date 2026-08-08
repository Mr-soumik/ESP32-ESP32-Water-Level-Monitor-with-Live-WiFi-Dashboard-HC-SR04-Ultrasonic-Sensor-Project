#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#define TRIG_PIN 5
#define ECHO_PIN 22

const char* AP_SSID = "TechTadka360_WaterLevel";
const char* AP_PASSWORD = "12345678";

const float EMPTY_DISTANCE_CM = 9.86;
const float FULL_DISTANCE_CM = 4.60;
const float LOW_LEVEL_PERCENT = 20.0;
const float FULL_LEVEL_PERCENT = 90.0;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

float distanceCm = 0;
float levelPercent = 0;
String alertStatus = "NORMAL";

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  return (duration * 0.0343) / 2.0;
}

float calculateLevelPercent(float distance) {
  float percent = ((EMPTY_DISTANCE_CM - distance) / (EMPTY_DISTANCE_CM - FULL_DISTANCE_CM)) * 100.0;
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return percent;
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>TechTadka360 Water Level Monitor</title>
<style>
* { margin:0; padding:0; box-sizing:border-box; font-family:'Segoe UI', Arial, sans-serif; }
body { background:linear-gradient(135deg,#0f2027,#203a43,#2c5364); min-height:100vh; display:flex; align-items:center; justify-content:center; padding:20px; }
.container { background:rgba(255,255,255,0.05); backdrop-filter:blur(10px); border-radius:20px; padding:30px; width:100%; max-width:400px; box-shadow:0 8px 32px rgba(0,0,0,0.3); border:1px solid rgba(255,255,255,0.1); }
h1 { color:#fff; text-align:center; font-size:20px; margin-bottom:5px; }
.sub { color:#8fd3f4; text-align:center; font-size:12px; margin-bottom:25px; letter-spacing:1px; }
.tank { position:relative; width:160px; height:260px; margin:0 auto 25px; border:4px solid #8fd3f4; border-radius:10px; overflow:hidden; background:rgba(255,255,255,0.03); }
.water { position:absolute; bottom:0; left:0; width:100%; background:linear-gradient(180deg,#36d1dc,#5b86e5); transition:height 0.6s ease; }
.water::before { content:''; position:absolute; top:-6px; left:0; width:200%; height:12px; background:rgba(255,255,255,0.4); border-radius:50%; animation:wave 3s linear infinite; }
@keyframes wave { 0% { transform:translateX(0); } 100% { transform:translateX(-50%); } }
.percent-label { position:absolute; top:50%; left:50%; transform:translate(-50%,-50%); color:#fff; font-size:32px; font-weight:bold; text-shadow:0 2px 6px rgba(0,0,0,0.5); z-index:2; }
.stats { display:flex; justify-content:space-between; color:#cfe8f5; font-size:13px; margin-bottom:20px; }
.stat-box { background:rgba(255,255,255,0.05); border-radius:10px; padding:10px; width:48%; text-align:center; }
.stat-box .value { font-size:18px; font-weight:bold; color:#fff; margin-top:4px; }
.alert { text-align:center; padding:10px; border-radius:10px; font-weight:bold; font-size:13px; letter-spacing:1px; }
.alert-normal { background:rgba(76,209,55,0.15); color:#4cd137; }
.alert-low { background:rgba(232,65,24,0.15); color:#e84118; }
.alert-full { background:rgba(0,168,255,0.15); color:#00a8ff; }
.status-dot { display:inline-block; width:8px; height:8px; border-radius:50%; background:#4cd137; margin-right:6px; animation:pulse 1.5s infinite; }
@keyframes pulse { 0%,100% { opacity:1; } 50% { opacity:0.3; } }
</style>
</head>
<body>
<div class="container">
<h1>TechTadka360</h1>
<div class="sub"><span class="status-dot"></span>WATER LEVEL MONITOR</div>
<div class="tank">
<div class="water" id="water" style="height:0%"></div>
<div class="percent-label" id="percentLabel">0%</div>
</div>
<div class="stats">
<div class="stat-box"><div>DISTANCE</div><div class="value" id="distanceVal">0 cm</div></div>
<div class="stat-box"><div>STATUS</div><div class="value" id="statusVal">--</div></div>
</div>
<div class="alert alert-normal" id="alertBox">NORMAL LEVEL</div>
</div>
<script>
let ws = new WebSocket('ws://' + window.location.hostname + '/ws');
ws.onmessage = function(event) {
  let data = JSON.parse(event.data);
  document.getElementById('water').style.height = data.percent + '%';
  document.getElementById('percentLabel').textContent = Math.round(data.percent) + '%';
  document.getElementById('distanceVal').textContent = data.distance.toFixed(1) + ' cm';
  document.getElementById('statusVal').textContent = data.alert;
  let alertBox = document.getElementById('alertBox');
  alertBox.className = 'alert';
  if (data.alert === 'LOW') {
    alertBox.classList.add('alert-low');
    alertBox.textContent = 'LOW WATER LEVEL';
  } else if (data.alert === 'FULL') {
    alertBox.classList.add('alert-full');
    alertBox.textContent = 'TANK FULL';
  } else {
    alertBox.classList.add('alert-normal');
    alertBox.textContent = 'NORMAL LEVEL';
  }
};
ws.onclose = function() {
  setTimeout(() => { window.location.reload(); }, 3000);
};
</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  String json = "{\"percent\":" + String(levelPercent, 1) + ",\"distance\":" + String(distanceCm, 1) + ",\"alert\":\"" + alertStatus + "\"}";
  ws.textAll(json);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    notifyClients();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

unsigned long lastRead = 0;

void loop() {
  ws.cleanupClients();

  if (millis() - lastRead > 1000) {
    lastRead = millis();
    float d = readDistanceCM();
    if (d > 0) {
      distanceCm = d;
      levelPercent = calculateLevelPercent(d);

      if (levelPercent <= LOW_LEVEL_PERCENT) alertStatus = "LOW";
      else if (levelPercent >= FULL_LEVEL_PERCENT) alertStatus = "FULL";
      else alertStatus = "NORMAL";

      notifyClients();
      Serial.printf("Distance: %.1f cm | Level: %.1f%% | %s\n", distanceCm, levelPercent, alertStatus.c_str());
    }
  }
}
