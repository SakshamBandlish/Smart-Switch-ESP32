#include <WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

// ============================================================================
// 🛠️ NETWORK & CLOUD CREDENTIALS CONFIGURATION
// ============================================================================
#define WIFI_SSID         "YOUR_WIFI_NAME"      // Your 2.4GHz Wi-Fi Network Name
#define WIFI_PASS         "YOUR_WIFI_PASSWORD"  // Your Wi-Fi Network Password
#define APP_KEY           "YOUR_APP_KEY"        // Copy from Sinric Pro Dashboard
#define APP_SECRET        "YOUR_APP_SECRET"     // Copy from Sinric Pro Dashboard
#define SWITCH_ID         "YOUR_DEVICE_ID"      // Copy from Sinric Pro Dashboard

// ============================================================================
// 🔌 HARDWARE INTERFACE PIN MAPPING
// ============================================================================
#define RELAY_PIN         4                     // Boot-safe independent GPIO pin (D4)

// ============================================================================
// 🧠 CLOUD INTERACTION CALLBACKS
// ============================================================================
bool onPowerState(const String &deviceId, bool &state) {
  if (state) {
    digitalWrite(RELAY_PIN, LOW);   // Active-LOW logic: Pulling LOW energizes coil (ON)
    Serial.println("🌐 [CLOUD] Smart Switch Target State -> ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Active-LOW logic: Pulling HIGH de-energizes coil (OFF)
    Serial.println("🌐 [CLOUD] Smart Switch Target State -> OFF");
  }
  return true; 
}

// ============================================================================
// ⚙️ SYSTEM INITIALIZATION
// ============================================================================
void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Set HIGH immediately to keep relay OFF during boot

  Serial.printf("\n📡 Initiating wireless link to SSID: %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("•");
  }
  
  Serial.println("\n✅ Wi-Fi Network Connection Established.");
  Serial.printf("🤖 Assigned Node Local IP Address: %s\n", WiFi.localIP().toString().c_str());

  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
  mySwitch.onPowerState(onPowerState); 

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void loop() {
  SinricPro.handle(); // Keeps background cloud TCP keep-alive packets ticking
}