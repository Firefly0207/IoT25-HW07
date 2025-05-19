// HW07_Advanced.ino

#include <WiFi.h>
#include "BluetoothSerial.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"


BluetoothSerial SerialBT;
String serialInput = "";
String btInput = "";
String deviceName = "CHAE_HEEJAE"; 

const int MEASURED_POWER = -69;
const float N = 2.0;
esp_bd_addr_t connectedAddr;
bool deviceConnected = false;
int rssiValue = 0;
bool rssiReceived = false;

const int led26 = 26;
String led26State = "off";

const char* ssid = "Hotspot";
const char* password = "qwety123";
WiFiServer server(80);
String wifiIP = "";
float globalRssi;
float globalDistance;

void bt_gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
  if (event == ESP_BT_GAP_READ_RSSI_DELTA_EVT) {
    if (param->read_rssi_delta.stat == ESP_BT_STATUS_SUCCESS) {
      rssiValue = param->read_rssi_delta.rssi_delta;
      rssiReceived = true;
    }
  }
}

void bt_spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    for (int i = 0; i < ESP_BD_ADDR_LEN; i++) {
      connectedAddr[i] = param->srv_open.rem_bda[i];
    }
    deviceConnected = true;
    Serial.println("device link success");
  } else if (event == ESP_SPP_CLOSE_EVT) {
    deviceConnected = false;
    Serial.println("device link delete");
  }
}


bool verifyWiFiNetwork(const char* target_ssid) {
  Serial.println("Scanning for WiFi networks...");
  int n = WiFi.scanNetworks();
  bool found = false;
  if (n == 0) {
    Serial.println("No WiFi networks found.");
  } else {
    for (int i = 0; i < n; ++i) {
      if (WiFi.SSID(i) == target_ssid) found = true;
      delay(10);
    }
  }
  return found;
}

bool connectWiFi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 20000; 
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(500);
  }
  return WiFi.status() == WL_CONNECTED;
}


void handleWebClient() {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClient client = server.available();
  if (!client) return;

  String currentLine = "";
  String header = "";
  unsigned long timeout = millis();

  while (client.connected() && millis() - timeout < 2000) {
    if (client.available()) {
      char c = client.read();
      header += c;
      if (c == '\n') {
        if (currentLine.length() == 0) {
        
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<style>.button {padding: 20px 40px; font-size: 24px;}</style>");
          client.println("</head><body>");
          client.println("<h1>ESP32 distance ble</h1>");
          client.println("<p>distance distance: " + String(globalDistance, 1) + "</p>");
          client.println("<p>distance rssi: " + String(globalRssi, 1) + "</p>");
          client.println("<p>IP Address: " + wifiIP + "</p>");
          client.println("</body></html>");

          break; 
        } else {
          currentLine = "";
        }
      } else if (c != '\r') {
        currentLine += c;
      }
    }
  }


  if(globalDistance <= 1){
    digitalWrite(led26, HIGH);
    led26State = "on";
    delay(100);  
    digitalWrite(led26, LOW);
  }

  delay(1);        
  client.flush();  
  client.stop();   
}


void processSerialInput() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      handleCommand(serialInput);
      serialInput = "";
    } else {
      serialInput += c;
    }
  }
}

void processBluetoothInput() {
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      handleCommand(btInput);
      btInput = "";
    } else {
      btInput += c;
    }
  }
}


void handleCommand(String cmd) {
  cmd.trim();
  if (cmd == "webinfo") {
    String response = "[Web Server Info]\nIP: " + wifiIP;
    Serial.println(response);
    SerialBT.println(response);
  } else if (cmd == "rssi") {

    if (deviceConnected) {
      esp_err_t result = esp_bt_gap_read_rssi_delta(connectedAddr);
      if (result == ESP_OK) {
        unsigned long startTime = millis();
        while (!rssiReceived && millis() - startTime < 200) {
          delay(10);
        }
        if (rssiReceived) {
          
          //In the team project hw06 absolute coordinate rssi, 0m has a measurement of approximately -52  
          //Added -52 to the relative coordinate rssi calculation formula
          float distance = ((abs(rssiValue) / 10.0));
        String msg = "RSSI: " + String(rssiValue-52) + " dBm, distance(m): " + String(distance, 1) + " m";
        
          globalRssi = String(rssiValue-52).toFloat();
        globalDistance =  distance;

          Serial.println(msg);
          SerialBT.println(msg);
          rssiReceived = false;
        } else {
          Serial.println("RSSI is nor received");
          SerialBT.println("RSSI is nor received");
        }
      } else {
        Serial.println("RSSi request failed");
        SerialBT.println("RSSI request failed");
      }
    } else {
      Serial.println("linked device none");
      SerialBT.println("linked device none");
    }
  } else if (cmd.startsWith("led")) {
    int pin = cmd.substring(3, 5).toInt();
    String state = cmd.substring(6);
  } else if (cmd != "") {
    String echo = "Received command: " + cmd;
    Serial.println(echo);
    SerialBT.println(echo);
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(led26, OUTPUT);
  digitalWrite(led26, LOW);


  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  if (verifyWiFiNetwork(ssid) && connectWiFi(ssid, password)) {
    server.begin();
    wifiIP = WiFi.localIP().toString();
    Serial.println("Web server started | IP: " + wifiIP);
  }

  SerialBT.begin(deviceName);
  SerialBT.register_callback(bt_spp_callback);
  esp_bt_gap_register_callback(bt_gap_callback);
  Serial.println("Bluetooth Ready");
}

void loop() {
  handleWebClient();
  processSerialInput();
  processBluetoothInput();
  handleCommand("rssi"); 
  delay(1000);  //delay
}
