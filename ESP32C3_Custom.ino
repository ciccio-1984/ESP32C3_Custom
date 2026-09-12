#pragma GCC optimize("-O2")

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "MiningJob.h"
#include "Settings.h"

MiningConfig *configuration;
MiningJob *job;

namespace {

void RestartESP(const char *msg) {
#if defined(SERIAL_PRINTING)
Serial.println(msg);
Serial.println("Restarting ESP...");
#endif


delay(100);
ESP.restart();

}

void VerifyWifi() {
if (WiFi.status() == WL_CONNECTED)
return;


#if defined(SERIAL_PRINTING)
    Serial.println("WiFi reconnecting...");
#endif

WiFi.disconnect();
delay(250);
WiFi.begin(SSID, PASSWORD);


}

String httpGetString(const char *url) {
WiFiClientSecure client;
HTTPClient https;


client.setInsecure();

if (!https.begin(client, url))
    return "";

https.addHeader("Accept", "*/*");

int httpCode = https.GET();
String payload;

#if defined(SERIAL_PRINTING)
    Serial.printf("HTTP response: %d\n", httpCode);
#endif

if (httpCode == HTTP_CODE_OK ||
    httpCode == HTTP_CODE_MOVED_PERMANENTLY) {

    payload = https.getString();

} else {

    #if defined(SERIAL_PRINTING)
        Serial.printf("Poolpicker error: %s\n",
                      https.errorToString(httpCode).c_str());
    #endif

    VerifyWifi();
}

https.end();

return payload;


}

void UpdateHostPort(const String &input) {
DynamicJsonDocument doc(256);


if (deserializeJson(doc, input)) {
    #if defined(SERIAL_PRINTING)
        Serial.println("Invalid poolpicker response");
    #endif
    return;
}

const char *name = doc["name"];
const char *ip   = doc["ip"];
int port         = doc["port"];

if (!name || !ip || port <= 0) {
    #if defined(SERIAL_PRINTING)
        Serial.println("Incomplete poolpicker response");
    #endif
    return;
}

configuration->host = ip;
configuration->port = port;

node_id = name;

#if defined(SERIAL_PRINTING)
    Serial.println("Mining node: " + node_id);
    Serial.println("Host: " + configuration->host);
    Serial.println("Port: " + String(configuration->port));
#endif


}

void SelectNode() {
uint8_t waitTime = 1;


while (true) {

    #if defined(SERIAL_PRINTING)
        Serial.println("Fetching mining node...");
    #endif

    String input =
        httpGetString("https://server.duinocoin.com/getPool");

    if (input.length() > 0) {
        DynamicJsonDocument doc(256);

        if (!deserializeJson(doc, input)) {

            const char *ip   = doc["ip"];
            const char *name = doc["name"];
            int port         = doc["port"];

            if (ip && name && port > 0) {
                UpdateHostPort(input);
                return;
            }
        }
    }

    #if defined(SERIAL_PRINTING)
        Serial.printf("Poolpicker unavailable. Retry in %u s\n",
                      waitTime);
    #endif

    delay(waitTime * 1000UL);

    waitTime <<= 1;

    if (waitTime > 32)
        RestartESP("Node fetch unavailable");
}


}

void SetupWifi() {


#if defined(SERIAL_PRINTING)
    Serial.print("Connecting to WiFi: ");
    Serial.println(SSID);
#endif

WiFi.mode(WIFI_STA);

// Disable WiFi power saving.
// This is preferable for a continuously running miner.
WiFi.setSleep(false);

WiFi.begin(SSID, PASSWORD);

unsigned long startAttempt = millis();

while (WiFi.status() != WL_CONNECTED) {

    delay(100);

    #if defined(SERIAL_PRINTING)
        Serial.print(".");
    #endif

    if (millis() - startAttempt > 20000UL)
        RestartESP("WiFi connection timeout");
}

#if defined(SERIAL_PRINTING)
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Rig: ");
    Serial.println(RIG_IDENTIFIER);
#endif

SelectNode();


}

} // namespace

void setup() {


pinMode(LED_BUILTIN, OUTPUT);

// Idle state
digitalWrite(LED_BUILTIN, HIGH);

#if defined(SERIAL_PRINTING)
    Serial.begin(SERIAL_BAUDRATE);
    delay(100);
    Serial.println();
    Serial.println("Duino-Coin " SOFTWARE_VERSION);
    Serial.println("ESP32-C3 minimal miner");
#endif

// Wallet grouping identifier
WALLET_ID = String(random(0, 2811));

configuration = new MiningConfig(
    DUCO_USER,
    RIG_IDENTIFIER,
    MINER_KEY
);

// Create the single mining job.
job = new MiningJob(0, configuration);

SetupWifi();

// CPU at maximum frequency.
setCpuFrequencyMhz(160);

// Startup indication.
job->blink(BLINK_SETUP_COMPLETE);

#if defined(SERIAL_PRINTING)
    Serial.println("Miner ready.");
    Serial.println("Starting mining...");
#endif


}

void loop() {
job->mine();
}