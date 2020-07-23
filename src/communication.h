


#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include <common.h>
#include <config.h>
#include <control.h>
#include <sensors.h>
#include <signals.h>

using namespace std;

//#include "ArduinoJson.h"

// Set web server port number to 80
//extern WiFiServer server;
extern char * header;

extern const char * ssid;
extern const char * password;

extern const char * ssid_alt;
extern const char * password_alt;

extern const char * ssid_host;
extern const char * password_host;



void begin_communications();

void begin_server();
void print_thermal_status_wifi();
void wifi_status_task(void * parameter);
void start_mdns_service();


void onRequest(AsyncWebServerRequest *request);

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

void print_boost_control_paramters_hf();

void print_status( void *parameters);

String create_json_update_function(const vector<String> &update_tags, String button_tag, String update_url);

String clickButton(String button_tag, String update_url);

String html_doc();
String add_input(String visible_name, String tag_name, int defaultvalue, vector<String> &mod_tags);
String add_field(String visible_name, String tag_name, vector<String> &update_tags);
String json_assign_field(String update_tag);
String assign_field(String update_tag);
String add_input(String visible_name, String tag_name, float defaultvalue, vector<String> &mod_tags);


//Informational String Formatting
String current_mode_info();
String last_test_pm();
String last_test_particle();
String inst_test_pm();
String inst_test_particle();

#endif
