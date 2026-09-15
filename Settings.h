#ifndef SETTINGS_H
#define SETTINGS_H

// ============================================================
// ESP32-C3 MINIMAL DUINO-COIN MINER
// ============================================================

// ---------------------- USER SETTINGS -----------------------

// Duino-Coin username
char *DUCO_USER = "XXXXXXXXXX";

// Mining key
char *MINER_KEY = "XXXXXXXXX";

// Miner name.
char *RIG_IDENTIFIER = "ESP32C3 Custom";

// WiFi network
const char SSID[] = "XXXXXXXXXX";

// WiFi password
const char PASSWORD[] = "XXXXXXXXXX";

// ---------------------- MINER OPTIONS ------------------------

// Enable LED status indication.
#define LED_BLINKING

// Serial output.
// Comment this line out for a slightly leaner/quiet build.
// #define SERIAL_PRINTING

// Serial speed
#define SERIAL_BAUDRATE 115200

// ---------------------- ESP32-C3 -----------------------------

// On the ESP32-C3 Mini/SuperMini boards normally used
// with this miner, the onboard LED is GPIO8.
#if defined(CONFIG_IDF_TARGET_ESP32C3)
#ifndef LED_BUILTIN
#define LED_BUILTIN 8
#endif
#else
#error "This firmware is intended for ESP32-C3 only"
#endif

// ---------------------- MINER CONSTANTS ----------------------

#define BLINK_SETUP_COMPLETE 2
#define BLINK_CLIENT_CONNECT 5

#define SOFTWARE_VERSION "4.3"

// ---------------------- MINING STATISTICS --------------------
// These are still used by MiningJob / submit.

unsigned int hashrate = 0;
unsigned int hashrate_core_two = 0;
unsigned int difficulty = 0;

unsigned long share_count = 0;
unsigned long accepted_share_count = 0;

String node_id = "";
String WALLET_ID = "";

unsigned int ping = 0;

#endif
