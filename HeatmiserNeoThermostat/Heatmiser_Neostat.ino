#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
//#include <WebSocketClient.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
//No longer needed as reading JsonChunkSize bytes at a time
//#include <PrintEx.h>


#define Firmware_Date __DATE__
#define Firmware_Time __TIME__

//This enables flashing of ESP via OTA (WiFI)
#define PM_ENABLE_OTA_UPDATES

//Default values for SmartThings, these are only used when first booting and stored in EEPROM from first boot onwards, no need to change
#define JsonChunkSize 2920

char IP_FOR_ST[17];
char PORT_FOR_ST[7];
char IP_FOR_NEOHUB[17];
char PORT_FOR_NEOHUB[7];
char jsontext[JsonChunkSize];

#define JsonHeaderText "HTTP/1.1 200 OK\r\nContent-Type: application/json;charset=utf-8\r\nServer: NeoHub Bridge\r\n\r\n"
//#define JsonHeaderText "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nConnection: close\r\n\r\n"

#define EEPROM_ST_IP_ADDR_LOC 100
#define EEPROM_ST_PORT_ADDR_LOC 120
#define EEPROM_NEOHUB_IP_ADDR_LOC 130
#define EEPROM_NEOHUB_PORT_ADDR_LOC 150


void ReadEEPROMSettings() {
  for (int ix = 0; ix < sizeof(IP_FOR_ST); ix++) {
    IP_FOR_ST[ix] = EEPROM.read(EEPROM_ST_IP_ADDR_LOC + ix);
  }
  for (int ix = 0; ix < sizeof(PORT_FOR_ST); ix++) {
    PORT_FOR_ST[ix] = EEPROM.read(EEPROM_ST_PORT_ADDR_LOC + ix);
  }

  for (int ix = 0; ix < sizeof(IP_FOR_NEOHUB); ix++) {
    IP_FOR_NEOHUB[ix] = EEPROM.read(EEPROM_NEOHUB_IP_ADDR_LOC + ix);
  }
  for (int ix = 0; ix < sizeof(PORT_FOR_NEOHUB); ix++) {
    PORT_FOR_NEOHUB[ix] = EEPROM.read(EEPROM_NEOHUB_PORT_ADDR_LOC + ix);
  }
}

void WriteEEPROMSettings(int EEPROM_ADDR_LOC) {
  switch (EEPROM_ADDR_LOC) {
    case EEPROM_ST_IP_ADDR_LOC:
      for (int ix = 0; ix < sizeof(IP_FOR_ST); ix++) {
        EEPROM.write(EEPROM_ST_IP_ADDR_LOC + ix, IP_FOR_ST[ix]);
      }
      break;
    case EEPROM_ST_PORT_ADDR_LOC:
      for (int ix = 0; ix < sizeof(PORT_FOR_ST); ix++) {
        EEPROM.write(EEPROM_ST_PORT_ADDR_LOC + ix, PORT_FOR_ST[ix]);
      }
      break;
    case EEPROM_NEOHUB_IP_ADDR_LOC:
      for (int ix = 0; ix < sizeof(IP_FOR_NEOHUB); ix++) {
        EEPROM.write(EEPROM_NEOHUB_IP_ADDR_LOC + ix, IP_FOR_NEOHUB[ix]);
      }
      break;
    case EEPROM_NEOHUB_PORT_ADDR_LOC:
      for (int ix = 0; ix < sizeof(PORT_FOR_NEOHUB); ix++) {
        EEPROM.write(EEPROM_NEOHUB_PORT_ADDR_LOC + ix, PORT_FOR_NEOHUB[ix]);
      }
      break;
    default:
      //Shouldnt ever get here if we have correctly listed all EEPROM locations above
      break;
  }
  EEPROM.commit();
}

//////////////////////////////////////////////////////////////////////////////////

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;


void handleRoot() {
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

  char szTmp[2000];
  sprintf(szTmp, "<html><b>Dashboard for esp8266 NeoHub Bridge.</b><br><br>MAC Address: %02X%02X%02X%02X%02X%02X<br>"
          "Uptime: %02d:%02d:%02d.%02d<br>Free Heap: %u<br><br>"
          "Web Commands<br><a href='/neorelay'>NeoHub Relay</a><br><br>"
          "Setup SmartHome Platform (create/removezones)<br>"
          "<a href='/setupsmarthome'>Configure Child Devices</a><br><br>"
          "JSON Endpoints<br><a href='/checkrelay'>Neo Hub Relay Status</a><br><a href='/settings'>ST and IP Details</a><br><br>"
          "Configuration<br><a href='/config'>Update port/ip _for_ st/neohub</a><br><a href='/update'>Update Firmware</a></html>", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], (int)days, (int)hours, (int)minutes, (int)val, ESP.getFreeHeap());
  server.send(200, "text/html", szTmp);
}

void handleSetupSmartHome() {
  //This is a holding page to configure ST zones
  server.send(200, "text/html", "Send info to SmartHome (Caution)<br><a href='/createchildzones'>Create Child Zones</a><br><a href='/removechildzones'>CAUTION Remove Child Zones</a><br><br>");
}

void handleCreateChildZones() {
  //This sends the server response first of all
  //server.send(200, "text/html", "Attempting to create child zones");

  //Now send the message to SmartHome to create child zones
  //Start building the response with the original request and then add in the Neohub response
  char tempstring[26];

  char commandchararray[250];
  strncpy(commandchararray, "{\"GET_ZONES\":0}", sizeof(commandchararray)-1);

  WiFiClient st; //This is for st
  WiFiClient neohub; //This is for the NeoHub

  if (st.connect(IP_FOR_ST, atoi(PORT_FOR_ST))) {
    //Serial.println("Connected to ST");
  }
  else {
    Serial.println("connection failed to ST");
    server.send(200, "text/html", "Couldn't connect to ST");
    st.stop();
    neohub.stop();
    return;
  }

  if (neohub.connect(IP_FOR_NEOHUB, atoi(PORT_FOR_NEOHUB))) {
    //Serial.println("Connected to Neohub");
  }
  else {
    Serial.println("Connection to Neohub failed");
    server.send(200, "text/html", "Couldn't connect to Neohub");
    st.stop();
    neohub.stop();
    return;
  }
  
  delay(5);
  neohub.write((const uint8_t *)commandchararray, strlen(commandchararray) + 1);
  delay(5);

  //Just have this wait in case the response is not back from Neohub yet
  unsigned long timeout = millis();
  while (neohub.available() == 0) {
    if (millis() - timeout > 4000) {
      server.send(200, "text/html", "Neohub timed out, try again");
      st.stop();
      neohub.stop();
      return;
    }
  }

  timeout = millis();
  while (neohub.available()) {
    int bytestoread = min(1452, min(neohub.available(), JsonChunkSize - 2));

    neohub.readBytes(jsontext, bytestoread); //readBytes

    jsontext[bytestoread] = '\0';
    delay(10);
    
    if (millis() - timeout > 8000) {
      server.send(200, "text/html", "Neohub timed out #2, try again");
      st.stop();
      neohub.stop();
      return;
    }
  }


  char endchararray[10];
  //reuse commandchararray because we can
  strncpy(commandchararray, "{\"update_type\":\"create_zones\",\r\n\"relaydevice\":\"esptriggeredsetup\",\r\n\"relayresult\":", sizeof(commandchararray)-1);
  strncpy(endchararray, "}", sizeof(endchararray)-1);

  //Now workout the full send length
  int contentLength = 0;
  contentLength += strlen(commandchararray);
  contentLength += strlen(jsontext);
  contentLength += strlen(endchararray);

  //Build the header
  char header_text[200];
  header_text[0] = '\0';
  strcat(header_text, "POST / HTTP/1.1\r\nHost: ");
  strcat(header_text, IP_FOR_ST);
  strcat(header_text, ":");
  strcat(header_text, PORT_FOR_ST);
  strcat(header_text, "\r\nContent-Type: application/json;charset=utf-8\r\nContent-Length: ");
  itoa(contentLength, tempstring, 10);
  strcat(header_text, tempstring);
  strcat(header_text, "\r\nServer: Neohub Relay\r\n\r\n");

  if (!st.connected()) {
    Serial.println("ST timed out");
    server.send(200, "text/html", "ST connection timed out");
    st.stop();
    neohub.stop();
    return;
  }

  //Send the header followed by the various parts of the message to ST
  st.print(header_text);

  st.print(commandchararray);
  st.print(jsontext);
  st.print(endchararray);

  //Finally close out all temporary objects to be safe and send success message
  server.send(200, "text/html", "Successfully sent information to ST");
  st.flush();
  st.stop();
  neohub.stop();
}

void handleRemoveChildZones() {
  //This attempts to remove
  server.send(200, "text/html", "Attempting to remove child zones (look at live logging to see any errors)");

  //Now assemble the message ready to send to SmartHome to remove child zones
  char tempstring[26];

  char message_text[100];
  message_text[0] = '\0';
  strcat(message_text, "{\"update_type\":\"remove_zones\"}");

  char header_text[200];
  header_text[0] = '\0';
  strcat(header_text, "POST / HTTP/1.1\r\nHost: ");
  strcat(header_text, IP_FOR_ST);
  strcat(header_text, ":");
  strcat(header_text, PORT_FOR_ST);
  strcat(header_text, "\r\nContent-Type: application/json;charset=utf-8\r\nContent-Length: ");
  itoa(strlen(message_text), tempstring, 10);
  strcat(header_text, tempstring);
  strcat(header_text, "\r\nServer: Neohub Relay\r\n\r\n");

  //Create the connection and send the message
  WiFiClient client;
  
  if (!client.connect(IP_FOR_ST, atoi(PORT_FOR_ST))) {
    //Serial.println("connection failed");
    return;
  }
  client.print(header_text);
  client.print(message_text);
  client.flush();
}

void handleNeoRelay() {
  //Start building the response with the original request and then add in the Neohub response
  char commandchararray[500];
  char devicechararray[50];
  int commandlength;
  bool has_device_arg = false;
  WiFiClient client;
  WiFiClient neohub; //This is for the NeoHub

  //Connect to the server - which is the requester
  client = server.client();

  char timingstring[2000];
  char timestr[8];
  long int startTime = millis();
  timingstring[0] = '\0';

  strcat(timingstring, ", \"timingstr\":\"");
  strcat(timingstring, "start:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("start:");
  Serial.println(millis() - startTime);

  if (server.hasArg("command")) {
    //If we have a command then lets log it for manipulation
    strncpy(commandchararray, server.arg("command").c_str(), 499);

    //Ensure the ending is null char
    commandchararray[499] = '\0';
    commandlength = strlen(commandchararray);
    Serial.println(commandchararray);

    if (server.hasArg("device")) {
      has_device_arg = true;
      strncpy(devicechararray, server.arg("device").c_str(), 49);
      devicechararray[49] = '\0';
    }
  }
  else {
    //No command so send a response to say so
    Serial.println("No command received");
    client.print(JsonHeaderText);
    client.print("{\"No command received\":\"Add /?command=blahblahblah\"}");
    client.stop();
    neohub.stop();
    return;
  }

  strcat(timingstring, " gotcommand:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("gotcommand:");
  Serial.println(millis() - startTime);

  strcpy(jsontext, "{\"relaycommand\": ");
  strcat(jsontext, commandchararray);
  strcat(jsontext, ",\r\n");
  if (has_device_arg) {
    strcat(jsontext, "\"relaydevice\": \"");
    strcat(jsontext, devicechararray);
    strcat(jsontext, "\",\r\n");
  }
  strcat(jsontext, "\"relayresult\":");

  strcat(timingstring, " processedresult:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("processedresult:");
  Serial.println(millis() - startTime);

  //client.setNoDelay(true);
  //neohub.setNoDelay(true);

  if (neohub.connect(IP_FOR_NEOHUB, atoi(PORT_FOR_NEOHUB))) {
    Serial.println("Connected to Neohub");
  }
  else {
    Serial.println("Connection to Neohub failed");
    client.print(JsonHeaderText);
    client.print("{\"Neohub connection failed\":\"Maybe try again\"}");
    client.stop();
    neohub.stop();
    return;
  }

  strcat(timingstring, " connectedneohub:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("connectedneohub:");
  Serial.println(millis() - startTime);

  //We can send the command to the neohub to give us some more processing time
  //neohub.write((const uint8_t *)commandchararray, commandlength + 1);

  strcat(timingstring, " postwrite:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("postwrite:");
  Serial.println(millis() - startTime);

  //Now set up the client for printing the return
  Serial.println("Using server connection");

  //client.setTimeout(200);
  //client.setNoDelay(1);

  strcat(timingstring, " postclientconnect:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("postclientconnect:");
  Serial.println(millis() - startTime);

  //Send the header
  client.write((const uint8_t *)JsonHeaderText, strlen(JsonHeaderText));
  //client.print(JsonHeaderText);

  strcat(timingstring, " postheader:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("postheader:");
  Serial.println(millis() - startTime);

  //Send the command and device char arrays
  client.write((const uint8_t *)jsontext, strlen(jsontext));
  //client.print(jsontext);

  //client.setTimeout(200);
  //neohub.setTimeout(200);

  strcat(timingstring, " postjsoncommand:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("postjsoncommand:");
  Serial.println(millis() - startTime);

  if (!neohub.connected()) {
    Serial.println(">>> Didnt connect before write !");
    client.print(JsonHeaderText);
    client.print("{\"Neohub connection lost\":\"Should try again\"}");
    client.stop();
    neohub.stop();
    return;
  }
  delay(5);

  neohub.write((const uint8_t *)commandchararray, commandlength + 1);

  delay(5);

  //Just have this wait in case the response is not back from Neohub yet
  unsigned long timeout = millis();
  while (neohub.available() == 0) {
    if (millis() - timeout > 4000) {
      Serial.println(">>> Client Timeout !");
      client.print(JsonHeaderText);
      client.print("{\"Neohub connection timeout\":\"Should try again\"}");
      client.stop();
      neohub.stop();
      return;
    }
  }

  strcat(timingstring, " waitedfortimeout:");
  sprintf(timestr, "%d", millis() - startTime);
  strcat(timingstring, timestr);
  Serial.print("waitedfortimeout:");
  Serial.println(millis() - startTime);
  
  timeout = millis();
  delay(5);
  while (neohub.available()) {
    int bytestoread = min(1452, min(neohub.available(), JsonChunkSize - 2));
    neohub.readBytes(jsontext, bytestoread); //readBytes
    
    strcat(timingstring, " availableloop:");
    sprintf(timestr, "%d", millis() - startTime);
    strcat(timingstring, timestr);
    strcat(timingstring, "-bytes:");
    sprintf(timestr, "%d", bytestoread);
    strcat(timingstring, timestr);
    Serial.print("availableloop:");
    Serial.println(millis() - startTime);
    Serial.print("-bytes:");
    Serial.println(bytestoread);

    jsontext[bytestoread] = '\0';
    client.write((const uint8_t *)jsontext, strlen(jsontext));
    //client.print(jsontext);
    Serial.println(jsontext);

    strcat(timingstring, " finishedloop:");
    sprintf(timestr, "%d", millis() - startTime);
    strcat(timingstring, timestr);

    if (millis() - timeout > 8000) {
      Serial.println(">>> Client Timeout 2!");
      client.print(JsonHeaderText);
      client.print("{\"Neohub connection timeout2\":\"Should try again\"}");
      client.stop();
      neohub.stop();
      return;
    }

    //Final delay to allow the buffer to fill
    delay(15);
  }

  strcat(timingstring, "\"}\r\n\0");
  //client.write((const uint8_t *)timingstring, strlen(timingstring));
  client.print(timingstring);

  //client.println("\"}\r\n");

  //client.print("}\r\n");

  //neohub.flush();
  neohub.stop();

  //Now we should close the connection to the client and neohub
  client.flush();
  client.stop();

  Serial.print("finished:");
  Serial.println(millis() - startTime);
}

void handleCheck() {
  //This returns a string showing the HUB is OK
  WiFiClient client = server.client();

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/plain\r\n");
  client.print("Connection: close\r\n");
  client.print("\r\n{\"neohub_relay_status\":\"");

  client.print("OK");

  client.print("\",\r\n\"neohub_relay_firmware_date\":\"");

  client.print(Firmware_Date);
  client.print(" - ");
  client.print(Firmware_Time);

  client.print("\"}\r\n");

  client.stop();
}

void handleSettings() {
  //This returns the current arm state in case it gets out of sync somehow
  WiFiClient client = server.client();

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/plain\r\n");
  client.print("Connection: close\r\n");
  client.print("\r\n{\"ip_for_st\":\"");
  client.print(IP_FOR_ST);
  client.print("\",\r\n\"port_for_st\":\"");
  client.print(PORT_FOR_ST);
  client.print("\",\r\n\"ip_for_neohub\":\"");
  client.print(IP_FOR_NEOHUB);
  client.print("\",\r\n\"port_for_neohub\":\"");
  client.print(PORT_FOR_NEOHUB);
  client.print("\"}\r\n");
  client.stop();
}

void handleConfig() {
  //Start the response
  WiFiClient client = server.client();
  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/plain\r\n");
  client.print("Connection: close\r\n\r\n");
  client.print("{\"update_config\":\"success\",\r\n");

  //If we have received an ST IP then update as needed
  if (server.hasArg("ip_for_st")) {
    //Check if it has changed before writing to EEPROM
    if (server.arg("ip_for_st") != IP_FOR_ST) {
      server.arg("ip_for_st").toCharArray(IP_FOR_ST, sizeof(IP_FOR_ST));
      WriteEEPROMSettings(EEPROM_ST_IP_ADDR_LOC);
      client.print("\"updated_st_ip\":\"success\",\r\n");
    }
    //Respond with current value, even if not needing an update
    client.print("\"ip_for_st\":\"");
    client.print(server.arg("ip_for_st").c_str());
    client.print("\",\r\n");
  }

  //If we have received an ST Port then update as needed
  if (server.hasArg("port_for_st")) {
    //Check if it has changed before writing to EEPROM
    if (server.arg("port_for_st") != PORT_FOR_ST) {
      server.arg("port_for_st").toCharArray(PORT_FOR_ST, sizeof(PORT_FOR_ST));
      WriteEEPROMSettings(EEPROM_ST_PORT_ADDR_LOC);
      client.print("\"updated_st_port\":\"success\",\r\n");
    }
    //Respond with current value, even if not needing an update
    client.print("\"port_for_st\":\"");
    client.print(server.arg("port_for_st").c_str());
    client.print("\",\r\n");
  }

  //If we have received an NeoHub IP then update as needed
  if (server.hasArg("ip_for_neohub")) {
    //Check if it has changed before writing to EEPROM
    if (server.arg("ip_for_neohub") != IP_FOR_NEOHUB) {
      server.arg("ip_for_neohub").toCharArray(IP_FOR_NEOHUB, sizeof(IP_FOR_NEOHUB));
      WriteEEPROMSettings(EEPROM_NEOHUB_IP_ADDR_LOC);
      client.print("\"updated_neohub_ip\":\"success\",\r\n");
    }
    //Respond with current value, even if not needing an update
    client.print("\"ip_for_neohub\":\"");
    client.print(server.arg("ip_for_neohub").c_str());
    client.print("\",\r\n");
  }

  //If we have received an NeoHub Port then update as needed
  if (server.hasArg("port_for_neohub")) {
    //Check if it has changed before writing to EEPROM
    if (server.arg("port_for_neohub") != PORT_FOR_NEOHUB) {
      server.arg("port_for_neohub").toCharArray(PORT_FOR_NEOHUB, sizeof(PORT_FOR_NEOHUB));
      WriteEEPROMSettings(EEPROM_NEOHUB_PORT_ADDR_LOC);
      client.print("\"updated_neohub_port\":\"success\",\r\n");
    }
    //Respond with current value, even if not needing an update
    client.print("\"port_for_neohub\":\"");
    client.print(server.arg("port_for_neohub").c_str());
    client.print("\",\r\n");
  }

  client.print("}\r\n");
  client.stop();

}

void handleRestart() {
  //This restarts the ESP from a web command in case it is needed
  server.send(200, "text/plain", "ESP is restarting");
  server.stop();
  delay(2000);
  ESP.restart();
}

void handleReset() {
  //This resets the ESP from a web command in case it is needed
  server.send(200, "text/plain", "ESP is resetting");
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

  Serial.begin(115200); //enable Serial if we need it for debugging

  //Add the EEPROM in order to read IP and port for SmartThings/Neohub
  EEPROM.begin(512);
  ReadEEPROMSettings();


  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("NeoHubRelayBridge")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  Serial.println("WiFi Connected - Local IP:");
  Serial.println(WiFi.localIP());

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("espNeostatHubBridge")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  server.on("/", handleRoot);
  server.on("/settings", handleSettings);
  server.on("/checkrelay", handleCheck);
  server.on("/config", handleConfig);
  server.on("/neorelay", handleNeoRelay);
  server.on("/setupsmarthome", handleSetupSmartHome);
  server.on("/createchildzones", handleCreateChildZones);
  server.on("/removechildzones", handleRemoveChildZones);
  server.on("/restart", handleRestart);
  server.on("/reset", handleReset);
  server.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(server.client());
  });
  //server.on("/config", HTTP_POST, handleConfig);


#ifdef PM_ENABLE_OTA_UPDATES
  //Link the HTTP Updater with the web sesrver
  httpUpdater.setup(&server);
  //Start listening for OTA update requests
  ArduinoOTA.begin();
  //Use default password
  ArduinoOTA.setPassword((const char *)"123");
#endif

  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("WiFi Server Started");

  Serial.println("Starting SSDP");

  //Now setup variables for use in SSDP section
  char macstring[16];
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(macstring, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  //Now initialise SSDP
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName("Neostat Bridge ESP Device");
  SSDP.setSerialNumber(macstring);
  SSDP.setURL("\\");
  SSDP.setModelName("Heatmiser Neostat ESP Bridge");
  SSDP.setModelNumber(macstring);
  SSDP.setModelURL("https://community.smartthings.com/t/release-heatmiser-neostat-integration/88204");
  SSDP.setManufacturer("Chris Charles");
  SSDP.setManufacturerURL("https://community.smartthings.com/t/release-heatmiser-neostat-integration/88204");
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.begin();

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}


void loop(void) {
#ifdef PM_ENABLE_OTA_UPDATES
  ArduinoOTA.handle();
#endif
  server.handleClient();
}

void os_usleep(int microseconds)
{
  delay(microseconds / 1000);
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
