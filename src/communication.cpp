#include <communication.h>
#include <sensors.h>


AsyncWebServer server(80);
//AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
//AsyncEventSource events("/events"); // event source (Server-Sent events)

const char * ssid     = "SpaceCase";
const char * password = "9252623793";

const char * ssid_alt = "1909";
const char * password_alt = "startnow";

const char * ssid_host = "monqmister";
const char * password_host = "dripdrip";

char * header = "";




void begin_communications(){

    Serial.begin(115200);
    delay(1000);
    while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc


    Serial.println("Communications Starting");

    WiFi.mode(WIFI_STA);


    delay( 100);
    WiFi.begin( ssid, password);

    delay( 100);
    while (WiFi.status() != WL_CONNECTED) {
        delay( 500);
        Serial.println("Connecting...");
    }


    Serial.println("Beginning Server");
    begin_server();
}

void begin_server(){

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      Serial.println("Got / Request");
      request->send(200, "text/html","Hello");
  });


   MDNS.begin("misty");
   MDNS.addService("http", "tcp", 80);

   server.begin();
}

void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
}

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  //Handle upload
}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  //Handle WebSocket event
}


void print_once(){

    // Serial.print("|");
    // Serial.print(m.mc_1p0);
    // Serial.print(" ");
    // Serial.println(m.mc_2p5);
    // Serial.print(" ");
    // Serial.println(m.mc_4p0);
    // Serial.print(" ");
    // Serial.print(m.mc_10p0);
    // Serial.print(" | ");

    Serial.print( V_Boost);
    Serial.print(" ");
    Serial.print( V_Out );

    Serial.print(" | ");
    Serial.print(m.nc_0p5);
    Serial.print(" ");
    Serial.print(m.nc_1p0);
    Serial.print(" ");
    Serial.print(m.nc_2p5);
    Serial.print(" ");
    Serial.print(m.nc_4p0);
    Serial.print(" ");
    Serial.print(m.nc_10p0);

    Serial.print(" | ");
    Serial.print(m.typical_particle_size);  
    
    Serial.print(" | ");
    Serial.print(raw_temp);
    Serial.print(" ");
    Serial.print(thermocouple_temp);
    Serial.print(" ");  
    Serial.print(temp_sensor_is_nominal);  
    Serial.print(" ");  
    Serial.print(heat_on);
    Serial.print("|");
    Serial.print(WiFi.localIP());

    Serial.print("|");
    Serial.print(  ESP.getFreeHeap());
    Serial.print("|");
    Serial.println(  failure_message );
}

void print_status( void * parameters){

    for(;;){
      print_once();
      vTaskDelay((TickType_t) 1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete( NULL );
}


    // bool success = try_connection(ssid, password);
    // Serial.println("Main Wifi Failed");

    // if (!success){
        
    //     Serial.println("Trying Alt Wifi");
    //     WiFi.disconnect();
    //     success = try_connection(ssid, password);
    //     if (!success){

    //             Serial.println("..Failed Starting AP Mode");
    //             WiFi.disconnect();
    //             WiFi.mode(WIFI_AP);
    //             WiFi.softAP(ssid_host.c_str(),password_host.c_str());
    //             Serial.print("AP Connected ");
    //             Serial.println(WiFi.status() == WL_CONNECTED);
    //         }
    //     }

// portMUX_TYPE wifiMutex = portMUX_INITIALIZER_UNLOCKED;

// bool try_connection(String ssid, String passwd, int max_tries=10){
//     int tries = 0;

//     delay( 100);
//     Serial.print("SSID:");Serial.println(ssid.c_str());
//     Serial.print("PASS:");Serial.println(passwd.c_str());
//     WiFi.begin( ssid.c_str(), passwd.c_str());

//     delay( 100);
//     while (WiFi.status() != WL_CONNECTED) {
//         delay( 500);
//         Serial.println("Connecting...");
//         tries += 1;
//         if (tries > max_tries){ break; } 
//     }

//     return (WiFi.status() == WL_CONNECTED);
// }    