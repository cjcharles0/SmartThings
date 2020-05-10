/*
    Automatic valve controller for ESP when WiFi AP has a device connected
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>


#define FLASH_EEPROM_SIZE 4096
extern "C" {
#include "spi_flash.h"
}
extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;
extern "C" uint32_t _SPIFFS_page;
extern "C" uint32_t _SPIFFS_block;


#define DEFAULT_ONTIME                 900
#define DEFAULT_USE_PASS               false
#define DEFAULT_UI_PASS                "Password123"
#define DEFAULT_WIFI_PASS              "Password123"
#define DEFAULT_RESET_COUNTER          0
#define RESET_LIMIT                    5
struct SettingsStruct
{
  long      ontime;
  boolean       useUIPassword;
  char          UIPassword[26];
  char        wifiPassword[26];
  byte          resetCounter;
  byte          settingsVersion;
} Settings;


//Useful defines
#define Firmware_Date __DATE__
#define Firmware_Time __TIME__
#define PRINTF_BUF 512 // define the tmp buffer size (change if desired)
#define JsonHeaderText "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nConnection: close\r\n\r\n"

#define SWITCH_PIN 0
#define RELAY_PIN 12
#define LED_PIN 13

#define PRESSED_ON 1
#define WIFI_ON 2

int currently_on = 0;



ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;


//These functions will load our Settings struct into memory or read it back
void SaveSettings(void)
{
  SaveToFlash(0, (byte*)&Settings, sizeof(struct SettingsStruct));
}
boolean LoadSettings()
{
  LoadFromFlash(0, (byte*)&Settings, sizeof(struct SettingsStruct));
}
void SaveToFlash(int index, byte* memAddress, int datasize)
{
  if (index > 33791) // Limit usable flash area to 32+1k size
  {
    return;
  }
  uint32_t _sector = ((uint32_t)&_SPIFFS_start - 0x40200000) / SPI_FLASH_SEC_SIZE;
  uint8_t* data = new uint8_t[FLASH_EEPROM_SIZE];
  int sectorOffset = index / SPI_FLASH_SEC_SIZE;
  int sectorIndex = index % SPI_FLASH_SEC_SIZE;
  uint8_t* dataIndex = data + sectorIndex;
  _sector += sectorOffset;

  // load entire sector from flash into memory
  noInterrupts();
  spi_flash_read(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(data), FLASH_EEPROM_SIZE);
  interrupts();

  // store struct into this block
  memcpy(dataIndex, memAddress, datasize);

  noInterrupts();
  // write sector back to flash
  if (spi_flash_erase_sector(_sector) == SPI_FLASH_RESULT_OK)
    if (spi_flash_write(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(data), FLASH_EEPROM_SIZE) == SPI_FLASH_RESULT_OK)
    {
      //Serial.println("flash save ok");
    }
  interrupts();
  delete [] data;
  //String log = F("FLASH: Settings saved");
  //addLog(LOG_LEVEL_INFO, log);
}
void LoadFromFlash(int index, byte* memAddress, int datasize)
{
  uint32_t _sector = ((uint32_t)&_SPIFFS_start - 0x40200000) / SPI_FLASH_SEC_SIZE;
  uint8_t* data = new uint8_t[FLASH_EEPROM_SIZE];
  int sectorOffset = index / SPI_FLASH_SEC_SIZE;
  int sectorIndex = index % SPI_FLASH_SEC_SIZE;
  uint8_t* dataIndex = data + sectorIndex;
  _sector += sectorOffset;

  // load entire sector from flash into memory
  noInterrupts();
  spi_flash_read(_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(data), FLASH_EEPROM_SIZE);
  interrupts();

  // load struct from this block
  memcpy(memAddress, dataIndex, datasize);
  delete [] data;
}
void EraseFlash()
{
  uint32_t _sectorStart = (ESP.getSketchSize() / SPI_FLASH_SEC_SIZE) + 1;
  uint32_t _sectorEnd = _sectorStart + (ESP.getFlashChipRealSize() / SPI_FLASH_SEC_SIZE);

  for (uint32_t _sector = _sectorStart; _sector < _sectorEnd; _sector++)
  {
    noInterrupts();
    if (spi_flash_erase_sector(_sector) == SPI_FLASH_RESULT_OK)
    {
      interrupts();
      Serial.print(F("FLASH: Erase Sector: "));
      Serial.println(_sector);
      delay(10);
    }
    interrupts();
  }
}

void handleRoot() {
  if (Settings.UIPassword[0] != 0 && Settings.useUIPassword == true) {
    if (!server.authenticate("admin", Settings.UIPassword))
      return server.requestAuthentication();
  }
  //This returns the 'homepage' with links to each other main page
  unsigned long days = 0, hours = 0, minutes = 0;
  unsigned long val = os_getCurrentTimeSec();

  days = val / (3600 * 24);
  val -= days * (3600 * 24);

  hours = val / 3600;
  val -= hours * 3600;

  minutes = val / 60;
  val -= minutes * 60;

  byte mac[6];
  WiFi.macAddress(mac);

  char szTmp[PRINTF_BUF * 2];
  sprintf(szTmp, "<html>"
          "<b>Dashboard for esp8266 controlled Visonic Powermax.</b><br><br>"
          "MAC Address: %02X%02X%02X%02X%02X%02X<br>"
          "Uptime: %02d:%02d:%02d.%02d<br>"
          "Free heap: %u<br><br>"
          "Web Commands<br>"
          "<a href='/config'>Config ?X=Y - ontime (secs), wifipassword, uipassword, useuipassword (1 or 0)</a><br>"
          "<a href='/settings'>Show Saved Details</a><br>"
          "<a href='/reboot'>Reboot device (soft reset is not as reliable as a power cycle)</a><br>"
          "<a href='/update'>Update Firmware</a>"
          "</html>",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
          (int)days, (int)hours, (int)minutes, (int)val,
          ESP.getFreeHeap());
  server.send(200, "text/html", szTmp);
}



void handleSettings() {
  if (Settings.UIPassword[0] != 0 && Settings.useUIPassword == true) {
    if (!server.authenticate("admin", Settings.UIPassword))
      return server.requestAuthentication();
  }
  //This returns the current arm state in case it gets out of sync somehow
  char tempstring[26];
  WiFiClient client = server.client();

  client.print(JsonHeaderText);
  client.print("{\"ontime\":");
  itoa(Settings.ontime, tempstring, 10);
  client.print(tempstring);

  client.print("\",\r\n\"firmware_date\":\"");
  client.print(Firmware_Date);
  client.print(" - ");
  client.print(Firmware_Time);

  client.print("\"}\r\n");
  client.stop();
}


void handleConfig() {
  if (Settings.UIPassword[0] != 0 && Settings.useUIPassword == true) {
    if (!server.authenticate("admin", Settings.UIPassword))
      return server.requestAuthentication();
  }
  //Start the response
  char tmpString[64];
  WiFiClient client = server.client();
  client.print(JsonHeaderText);
  client.print("{\"update_config\":\"success\",\r\n");

  //If we have received an ontime variable then update it
  if (server.hasArg("ontime")) {
    Settings.ontime = atoi(server.arg("ontime").c_str());

    client.print("\"ontime\":\"");
    client.print(Settings.ontime);
    client.print("\",\r\n");
  }

  //If we have received a message to use UI password then update as needed
  if (server.hasArg("useuipassword")) {
    //Update setting

    if (atoi(server.arg("useuipassword").c_str()) == 1) {
      Settings.useUIPassword = true;
    }
    else {
      Settings.useUIPassword = false;
    }

    client.print("\"useuipassword\":\"");
    client.print(Settings.useUIPassword);
    client.print("\",\r\n");
  }
  //If we have received a new UI password then update as needed
  if (server.hasArg("uipassword")) {
    //Update setting
    server.arg("uipassword").toCharArray(Settings.UIPassword, 25);

    client.print("\"uipassword\":\"");
    client.print(Settings.UIPassword);
    client.print("\",\r\n");
  }
  //If we have received a new WiFi password then update as needed
  if (server.hasArg("wifipassword")) {
    //Update setting
    server.arg("wifipassword").toCharArray(Settings.wifiPassword, 25);

    client.print("\"wifipassword\":\"");
    client.print(Settings.wifiPassword);
    client.print("\",\r\n");
  }


  client.print("}\r\n");
  client.stop();

  //Now save the values
  SaveSettings();
}





void handleRestart() {
  if (Settings.UIPassword[0] != 0 && Settings.useUIPassword == true) {
    if (!server.authenticate("admin", Settings.UIPassword))
      return server.requestAuthentication();
  }
  //This restarts the ESP from a web command in case it is needed
  server.send(200, "text/plain", "ESP is restarting (can also reset with changed URL)");
  server.stop();
  delay(2000);
  ESP.restart();
}

void handleReboot() {
  handleRestart();
}

void handleReset() {
  if (Settings.UIPassword[0] != 0 && Settings.useUIPassword == true) {
    if (!server.authenticate("admin", Settings.UIPassword))
      return server.requestAuthentication();
  }
  //This resets the ESP from a web command in case it is needed
  server.send(200, "text/plain", "ESP is resetting (can also restart with changed URL)");
  server.stop();
  delay(2000);
  ESP.reset();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



void setup(void) {

  pinMode(SWITCH_PIN, INPUT);

  digitalWrite(RELAY_PIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  digitalWrite(LED_PIN, HIGH);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  //Read settings from SPIFFS, increase reset counter and save new reset counter
  LoadSettings();
  Settings.resetCounter++;
  SaveSettings();

  if (Settings.resetCounter >= RESET_LIMIT) {
    //We have reached reset limit so clear settings and restart
    EraseFlash();
    delay(1000);
    ESP.restart();
  }

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Starting...");

  //Wait 2s before continuing (this is the boot delay)
  delay(2000);

  if ((Settings.settingsVersion < 100) || (Settings.settingsVersion == 255)) {
    Settings.ontime = DEFAULT_ONTIME;
    Settings.useUIPassword = DEFAULT_USE_PASS;
    strncpy(Settings.UIPassword, DEFAULT_UI_PASS, sizeof(Settings.UIPassword));
    strncpy(Settings.wifiPassword, DEFAULT_WIFI_PASS, sizeof(Settings.wifiPassword));
    Settings.resetCounter = DEFAULT_RESET_COUNTER;
    Settings.settingsVersion = 100;
  }


  Settings.resetCounter = 0;
  SaveSettings();

  WiFi.softAP("WiFi-Water", Settings.wifiPassword);

  Serial.println("WiFi Water Valve");

  //Now setup variables for use in SSDP/mDNS
  char macstring[16];
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(macstring, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Set up mDNS responder:
  char mdnsname[30] = "WifiTap-";
  mdnsname[strlen(mdnsname)] = macstring[8];
  mdnsname[strlen(mdnsname)] = macstring[9];
  mdnsname[strlen(mdnsname)] = macstring[10];
  mdnsname[strlen(mdnsname)] = macstring[11];
  mdnsname[strlen(mdnsname)] = '\0';
  if (!MDNS.begin(mdnsname)) {
    //Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  server.on("/", handleRoot);
  server.on("/settings", handleSettings);
  server.on("/config", handleConfig);
  server.on("/restart", handleRestart);
  server.on("/reset", handleReset);
  server.on("/reboot", handleReboot);


  //Link the HTTP Updater with the web sesrver
  httpUpdater.setup(&server);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started.");


  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

}

void turn_on() {
  //Turn on relay and LED
  currently_on = true;
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
}

void turn_off() {
  //Turn off relay and LED
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
}

void loop(void) {
  unsigned long now = millis();
  static long last_connect = 0;
  static long last_press = 0;

  server.handleClient();

  if (digitalRead(SWITCH_PIN) == LOW) {
    last_press = now;

    if (!currently_on) {
      //Turn on the output as it isnt already on
      turn_on();
    }
    //Record that the on command came from pressing
    currently_on = PRESSED_ON;
  }
  else {
    //If we havent been pressed for the last 1 second then turn off
    if ((now - last_press >= 1000) && (currently_on == PRESSED_ON)) {
      currently_on = false;
      turn_off();
    }
  }


  if (WiFi.softAPgetStationNum() > 0) {
    //Log that we have a device connected at this time
    last_connect = now;

    if (!currently_on) {
      //Turn on the output as it isnt already on
      turn_on();
    }
    //Record that the on command came from wifi
    currently_on = WIFI_ON;
  }
  else {
    //If we havent been connected in ontime then turn off the output
    if ((now - last_connect >= Settings.ontime * 1000) && (currently_on == WIFI_ON)) {
      currently_on = false;
      turn_off();
    }
  }

  delay(100);
}


unsigned long os_getCurrentTimeSec()
{
  static unsigned int wrapCnt = 0;
  static unsigned long lastVal = 0;
  unsigned long currentVal = millis();

  if (currentVal < lastVal)
  {
    wrapCnt++;
  }

  lastVal = currentVal;
  unsigned long seconds = currentVal / 1000;

  //millis will wrap each 50 days, as we are interested only in seconds, let's keep the wrap counter
  return (wrapCnt * 4294967) + seconds;
}
