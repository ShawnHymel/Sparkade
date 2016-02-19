/**
 * Sparkade
 * Authors: Nick Poole, Shawn Hymel
 * Date: February 14, 2016
 * 
 * Serves a captive portal access point on the SparkFun ESP8266
 * Thing board. Note that SSL does not work, so HTTPS sites will
 * not be redirected.
 * 
 * Hardware:
 *  - SparkFun ESP8266 Thing (or Thing Dev Board)
 *  - SD Card Breakout
 *  - SD Card
 *  - Jumper wires
 *  
 * Connections:
 *  Thing | SD Card Breakout
 *  ------|-----------------
 *    3V3 | VCC
 *    GND | GND
 *     14 | CLK
 *     13 | CMD (MOSI)
 *     12 | D0 (MISO)
 *     15 | D3 (CS)
 *     
 * Libraries:
 * 
 * To Use:
 *  Place a web page "index.html" file on a FAT-formatted SD card
 *  conencted to the Thing (via SPI). The server, on boot, will
 *  create an access point and serve the index.html file to HTTP
 *  requests. See README.md for more detailed instructions.
 * 
 * Based on the ESP8266WebServer library by Ivan Grokhotkov (2014)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later 
 * version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General 
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <SPI.h>
#include <SdFat.h>

// Constants
#define DEBUG_SERIAL      1           // Print info to serial
#define DEBUG_LOG         1           // Save debug info to file
#define SD_SPEED          SPI_HALF_SPEED
#define LED_ON            0
#define LED_OFF           1

// Parameters
const char* AP_SSID = "Sparkade";     // Name of access point (AP)
const char* AP_PSK = "";              // Password for AP
const String DEFAULT_PAGE = "index.html";
const String LOG_FILE_BASE = "debug"; // Name of the log file
const IPAddress AP_IP(10, 10, 10, 1);
const IPAddress SUBNET_MASK(255, 255, 255, 0);
const byte DNS_PORT = 53;             // Port for DNS
const byte SD_CS = SS;                // Chip select for SD card
const int LED_PIN = 5;                // LED for R/W operations

// Globals
IPAddress ap_ip(192, 168, 1, 1);
DNSServer dns_server;
ESP8266WebServer server(80);
static bool has_sd = false;
SdFat sd;
SdFile log_file;
char *log_file_name;

/****************************************************************
 * Functions
 ***************************************************************/

// Send debugging message to the serial console and log file
void debug(char s[]) {

  // Print to console
  if ( DEBUG_SERIAL > 0 ) {
    Serial.println(s);
  }

  // Append to log file
  if ( DEBUG_LOG > 0 ) {
    digitalWrite(LED_PIN, LED_ON);
    if ( !log_file.open(log_file_name, 
                        O_CREAT | O_WRITE | O_AT_END) ) {
      if ( DEBUG_SERIAL > 0 ) {
        Serial.println("Could not write to log file");
      }
    }
    log_file.println(s);
    log_file.close();
    digitalWrite(LED_PIN, LED_OFF);
  }
}

// Configure access point
void setupAP() {

  // Configure and start access point
  WiFi.softAPConfig(AP_IP, AP_IP, SUBNET_MASK);
  WiFi.softAP(AP_SSID, AP_PSK);

  // Print out AP's IP address
  IPAddress ip = WiFi.softAPIP();
  char msg[25];
  sprintf(msg, "AP at: %i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
  debug(msg);

  // Start running DNS server (captive portal)
  dns_server.setErrorReplyCode(DNSReplyCode::NoError);
  dns_server.start(DNS_PORT, "*", AP_IP);
}

// Create a new log file
void createLogFile() {

  uint32_t cnt = 0;
  String file_name;
  uint32_t name_len;
  bool exists = true;

  // Keep incrementing until we find a file that doesn't exist
  while ( exists ) {

    // Construct char array of log file name
    file_name = LOG_FILE_BASE + cnt + ".log";
    name_len = file_name.length() + 1;
    log_file_name = (char *)malloc(name_len * sizeof(char));
    memset(log_file_name, 0, name_len);
    file_name.toCharArray(log_file_name, name_len);

    // Check if we've reached the max log files
    if ( cnt == UINT32_MAX ) {
      exists = false;
      debug("Max logs reached. Using the last one.");
      break;
    }

    // Check if the log file exists
    if ( sd.exists(log_file_name) ) {
      cnt++;
      free(log_file_name);
    } else {
      exists = false;
    }
  }
}

// Serve a file from the SD card
bool loadFromSdCard(String path) {

  String data_type = "text/plain";

  // Default load index.htm
  if ( (path == "") || path.endsWith("/") ) {
    path += DEFAULT_PAGE;
    data_type = "text/html";
  }

  // Look in certain directories depending on type of data
  if ( path.endsWith(".src") ) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if(path.endsWith(".htm")) {
    data_type = "text/html";
  } else if(path.endsWith(".css")) {
    data_type = "text/css";
  } else if(path.endsWith(".js")) {
    data_type = "application/javascript";
  } else if(path.endsWith(".png")) {
    data_type = "image/png";
  } else if(path.endsWith(".gif")) {
    data_type = "image/gif";
  } else if(path.endsWith(".jpg")) {
    data_type = "image/jpeg";
  } else if(path.endsWith(".ico")) {
    data_type = "image/x-icon";
  } else if(path.endsWith(".xml")) {
    data_type = "text/xml";
  } else if(path.endsWith(".pdf")) {
    data_type = "application/pdf";
  } else if(path.endsWith(".zip")) {
    data_type = "application/zip";
  }

  // Look for file in SD card
  File data_file = sd.open(path.c_str());
  if ( data_file.isDirectory() ) {
    path += "/";
    path += DEFAULT_PAGE;
    data_type = "text/html";
    data_file = sd.open(path.c_str());
  }

  // If we were unable to open the file, return
  if ( !data_file ) {
    debug("Could not read file from SD card");
    return false;
  }

  // Handle binary data type
  if ( server.hasArg("download") ) {
    data_type = "application/octet-stream";
  }

  // Serve file. Do nothing if it fails.
  if ( server.streamFile(data_file, data_type) != 
                                      data_file.size() ) {
                                        
  }

  // Close the file
  data_file.close();
  
  return true;
}

/****************************************************************
 * HTTP Handlers (callbacks)
 ***************************************************************/

void handleRoot() {
  loadFromSdCard("");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

/****************************************************************
 * Execution starts here
 ***************************************************************/

void setup() {

  // Enable debugging
  if ( DEBUG_SERIAL > 0 ) {
    Serial.begin(9600);
    Serial.println("Thing Captive Portal");
  }

  // Config LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  // Enable SD card
  if ( sd.begin(SD_CS, SD_SPEED) ) {
    if ( DEBUG_SERIAL > 0 ) {
      Serial.println("SD card initialized");
    }
    has_sd = true;
  } else {
    if ( DEBUG_SERIAL > 0 ) {
      Serial.println("SD card could not be initialized");
    }
    while(1);
  }

  // Check if log file exists
  if ( DEBUG_LOG > 0 ) {
    createLogFile();
  }

  // Enable access point
  setupAP();

  // Set HTTP callbacks
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  // Start web server
  server.begin();
}

void loop() {

  // Captive portal - process DNS requests
  dns_server.processNextRequest();

  // Process client HTTP requests
  server.handleClient();
}

