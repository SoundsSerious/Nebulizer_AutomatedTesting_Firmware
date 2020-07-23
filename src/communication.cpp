#include <communication.h>


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

using namespace std;

portMUX_TYPE wifiMutex = portMUX_INITIALIZER_UNLOCKED;


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
      request->send(200, "text/html",html_doc());
  });

  
  //We sendout the current system info with this call
  server.on("/info", HTTP_GET, [] (AsyncWebServerRequest *request) {
      AsyncJsonResponse * response = new AsyncJsonResponse();
      response->addHeader("Server","Mister Data");
      JsonVariant& root = response->getRoot();


      root["on_time"] =  millis()/(1000.0*60.0);
      root["current_boost_voltage"] =   v_boost;
      root["current_boost_frequency"] =    BOOST_FREQUENCY;
      root["current_boost_duty"] =    (float) boost_duty;
      root["current_output_voltage_pp"] =    v_out_pp;
      root["current_output_voltage_avg"] =    v_out;
      root["current_output_duty"] =    OUT_DUTY;

      root["current_temp"] =    thermocouple_temp;
      root["current_temp_duty"] =    heat_duty;

      root["current_clear_fan_duty"] = CLR_FAN_DUTY;
      root["current_vac_fan_duty"] =    VAC_FAN_DUTY;

      root["current_boost_power"] =    estimated_boost_power;
      root["current_out_power"] =    estimated_output_power;
      root["current_efficency"] =   estimated_system_efficiency;
      root["measured_vin"] =   measured_vin ;

      root["goal_boost_voltage"] = V_BOOST_SET;
      root["set_boost_frequency"] =   BOOST_FREQUENCY ;
      root["current_output_frequency"] =    OUT_FREQUENCY;

      root["set_out_duty"] =    OUT_DUTY;
      root["goal_temperature"] =   PROCESS_TEMP_SET ;

      root["set_duty_temp_max"] =   TEMP_MAX_DUTY ;
      root["set_bst_duty_max_on"] =    BST_DUTY_MAX_ON;
      root["set_bst_duty_max_off"] =   BST_DUTY_MAX_OFF ;
      root["set_bst_duty_min_on"] =    BST_DUTY_MIN_ON;
      root["set_bst_duty_min_off"] =   BST_DUTY_MIN_OFF;
      root["current_bst_err"] =    boost_error;
      
      //root["current_bst_integral_err"] =    ;
      
      root["current_temp_err"] =   temp_error ;
      
      //root["current_temp_integral_err"] =    ;

      root["current_mode"] =   current_mode_info() ;
      root["temp_sensor_is_nominal"] =   temp_sensor_is_nominal ;
      root["thermally_ready"] =  heat_allowed  ;
      
      root["th_sensor_failure"] =  thermocouple_failure_message  ;
      root["pm_sensor_is_nominal"] = pm_sensor_is_nominal;
      root["pm_sensor_failure"] = pmsensor_failure_message;

      root["is_boost_on"] =  BOOST_ON  ;
      root["is_out_on"] =   OUT_ON ;
      root["is_heat_on"] =  HEAT_ON  ;
      root["is_clear_fan_on"] = CLEAR_FAN_ON ;
      root["is_vac_fan_on"] = VAC_FAN_ON ;

      root["last_pm_levels"] =  last_test_pm()  ;
      root["last_particle_level"] =  last_test_particle()  ;
      root["last_typical_particle_size"] =  last_typical_particle_size  ;
      root["last_mass_flow_est"] =  last_massflow  ;
      root["inst_pm_levels"] =   inst_test_pm() ;
      root["inst_particle_level"] =  inst_test_particle()  ;
      root["inst_typical_particle_size"] =  inst_typical_particle_size  ;
      root["inst_mass_flow_est"] =  inst_massflow  ;

      root["current_run_time"] =  RUN_TIME  ;
      root["current_clear_time"] =  CLEAR_TIME  ;
      root["current_vac_time"] =   VAC_TIME ;
      root["current_boost_time"] = BST_TIME   ;

      response->setLength();

      request->send(response);
  });

  server.on("/process_setting", HTTP_POST, [](AsyncWebServerRequest * request){
      // int headers = request->headers();
      // int i;
      // for(i=0;i<headers;i++){
      //   Serial.printf("HEADER[%s]: %s\n", request->headerName(i).c_str(), request->header(i).c_str());
      // }
      request -> send(200);
      },
      NULL,
      [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {

          String body = String((char *)data);
          //Serial.println(body);
          StaticJsonDocument<512> doc;
          deserializeJson(doc, body);

          portENTER_CRITICAL(&wifiMutex);

					AUTO_MODE=  bool(doc["auto_mode"]); 
					RUN_TIME =  int(doc["run_time"]); 
					CLEAR_TIME =  int(doc["clear_time"]); 
					VAC_TIME =  int(doc["vac_time"]); 
					BST_TIME =  int(doc["boost_time"]);

          portEXIT_CRITICAL(&wifiMutex);
					taskYIELD();
          portENTER_CRITICAL(&wifiMutex);

          PROCESS_TEMP_SET =  double(doc["temperature"]); 
				  TEMP_MAX_DUTY	=  int(doc["temp_duty_max"]); 
					V_BOOST_SET=  int(doc["boost_voltage"]); 
					BOOST_FREQUENCY =  int(doc["boost_frequency"]); 

          portEXIT_CRITICAL(&wifiMutex);
					taskYIELD();
          portENTER_CRITICAL(&wifiMutex);					

          BST_DUTY_MAX_ON= int(doc["bst_duty_max_on"]); 
					BST_DUTY_MAX_OFF=  int(doc["bst_duty_max_off"]); 
					BST_DUTY_MIN_ON=  int(doc["bst_duty_min_on"]); 
					BST_DUTY_MIN_OFF=  int(doc["bst_duty_min_off"]);

          portEXIT_CRITICAL(&wifiMutex);
					taskYIELD();
          portENTER_CRITICAL(&wifiMutex);					

          OUT_FREQUENCY =  int(doc["out_frequency"]); 
					OUT_DUTY=  int(doc["out_duty"]); 
					CLR_FAN_DUTY=  int(doc["clr_duty"]); 
					VAC_FAN_DUTY=  int(doc["vac_duty"]); 

          portEXIT_CRITICAL(&wifiMutex);
					taskYIELD();
          portENTER_CRITICAL(&wifiMutex);

					Kp_boost =  doc["bst_control_kp"]; 
					Ki_boost=  doc["bst_control_ki"]; 
					Kp_therm=  doc["bst_control_kp"]; 
					Ki_therm=  doc["bst_control_ki"]; 

          portEXIT_CRITICAL(&wifiMutex);
					taskYIELD();
          portENTER_CRITICAL(&wifiMutex);          
          set_boost_frequency(BOOST_FREQUENCY);
          set_output_frequency(OUT_FREQUENCY);
          portEXIT_CRITICAL(&wifiMutex);


  });

  server.on("/auto_mode_button", HTTP_GET, [](AsyncWebServerRequest *request){
    //Serial.println("Got / Request");
    portENTER_CRITICAL(&wifiMutex);
    AUTO_MODE = !AUTO_MODE;
    portEXIT_CRITICAL(&wifiMutex);    
    request->send(200, "text/html","OK");
  });

  server.on("/bst_on", HTTP_GET, [](AsyncWebServerRequest *request){
    //Serial.println("Got / Request BOOST TOGGLE");
    portENTER_CRITICAL(&wifiMutex);
    BOOST_ON = !BOOST_ON;
    if (!AUTO_MODE){ //We Will Apply Change

    }
    portEXIT_CRITICAL(&wifiMutex);    
    request->send(200, "text/html","OK");
  });

  server.on("/out_on", HTTP_GET, [](AsyncWebServerRequest *request){
    //Serial.println("Got / Request OUT TOGGLE");
    portENTER_CRITICAL(&wifiMutex);
    OUT_ON = !OUT_ON;
    if (!AUTO_MODE){ //We Will Apply Change

    }    
    portEXIT_CRITICAL(&wifiMutex);    
    request->send(200, "text/html","OK");
  });

  server.on("/clr_on", HTTP_GET, [](AsyncWebServerRequest *request){
    //Serial.println("Got / Request CLR TOGGLE");
    portENTER_CRITICAL(&wifiMutex);
    CLEAR_FAN_ON = !CLEAR_FAN_ON;
    if (!AUTO_MODE){ //We Will Apply Change

    }
    portEXIT_CRITICAL(&wifiMutex);    
    request->send(200, "text/html","OK");
  }); 

  server.on("/vac_on", HTTP_GET, [](AsyncWebServerRequest *request){
    //Serial.println("Got / Request");
    portENTER_CRITICAL(&wifiMutex);
    VAC_FAN_ON = !VAC_FAN_ON;
    if (!AUTO_MODE){ //We Will Apply Change

    }
    portEXIT_CRITICAL(&wifiMutex);    
    request->send(200, "text/html","OK");
  });    

  server.on("/heat_on", HTTP_GET, [](AsyncWebServerRequest *request){
    //Serial.println("Got / Request");
    portENTER_CRITICAL(&wifiMutex);
    HEAT_ON = !HEAT_ON;
    if (!AUTO_MODE){ //We Will Apply Change

    }
    portEXIT_CRITICAL(&wifiMutex);    
    request->send(200, "text/html","OK");

  });   

   MDNS.begin("misty");
   MDNS.addService("http", "tcp", 80);

   server.begin();
   Serial.println("Server Started");
}

  // doc += "<br><button id=\"auto_mode_button\">Auto / Input Toggle</button> ";
  // doc += "<br><button id=\"bst_on\">Boost Toggle</button> ";
  // doc += "<br><button id=\"out_on\">Out Toggle</button> ";
  // doc += "<br><button id=\"clr_on\">Clear Toggle</button> ";
  // doc += "<br><button id=\"vac_on\">Vac Toggle</button> ";
  // doc += "<br><button id=\"heat_on\">Heat Toggle</button> ";

void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
}

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, 
                    uint8_t *data, size_t len, bool final){
  //Handle upload
}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, 
          AwsEventType type, void * arg, uint8_t *data, size_t len ){
  //Handle WebSocket event
}




void print_status( void * parameters){

    for(;;){
      //print_once();
      //vTaskDelay((TickType_t) 500 / portTICK_PERIOD_MS);

      print_boost_control_paramters_hf();
      vTaskDelay((TickType_t) 100 / portTICK_PERIOD_MS);
    }
    vTaskDelete( NULL );
}



String html_doc(){
  String doc = F("<!doctype html>\n");

  vector<String> update_tags;

  vector<String> input_tags;

  doc += "<html>\n";
  doc += "<head>\n";
  doc += "<title>MONQ Testing Automator</title>\n";
  
  doc += "<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootswatch/4.5.0/slate/bootstrap.min.css\">\n";
  doc += "<script src=\"https://code.jquery.com/jquery-3.2.1.min.js\"></script>\n";
  doc += "<style> br,p,h1,h2,h3,h4,h5{ line-height:13px; !important }</style>\n";

  //Update Function 
  doc += "<script>$(function(){setInterval(requestData, 500);function requestData(){$.getJSON('/info',function(data){if(data){ console.log(data); $.each(data, function(key,value) {$(\"#\"+key).text(\":\t\"+value);});}else{console.log('bad data');}});}});</script>\n";

  doc += "</head>\n";

  doc += "<body>\n";
  doc += "<div style=\"display: table-row\">";

  doc += "<div style=\"width: 75%; display: table-cell;\">";
  doc += add_field("On Time (min)","on_time",update_tags);

  doc += "<h3>System Info </h3>";
  doc += add_field("Boost Voltage","current_boost_voltage",update_tags);
  doc += add_field("Boost Frequency","current_boost_frequency",update_tags);
  doc += add_field("Boost Duty","current_boost_duty",update_tags);

  doc += add_field("Out Voltage PP","current_output_voltage_pp",update_tags);
  doc += add_field("Out Voltage Avg","current_output_voltage_avg",update_tags);
  doc += add_field("Out Frequency","current_output_frequency",update_tags);
  doc += add_field("Out Duty","current_output_duty",update_tags);

  doc += add_field("Current Temp C","current_temp",update_tags);
  doc += add_field("Heat Duty","current_temp_duty",update_tags);

  doc += "<br><h3>Control</h3>";
  
  doc += add_field("Set Boost Voltage","goal_boost_voltage",update_tags);

  doc += add_field("Temperature Goal","goal_temperature",update_tags);
  doc += add_field("Duty Temperature Max","set_duty_temp_max",update_tags);  

  doc += add_field("Boost Duty Max ON","set_bst_duty_max_on",update_tags);
  doc += add_field("Boost Duty Max OFF","set_bst_duty_max_off",update_tags);
  doc += add_field("Boost Duty Min ON","set_bst_duty_min_on",update_tags);
  doc += add_field("Boost Duty Min OFF","set_bst_duty_min_off",update_tags);
  
  doc += add_field("Boost Error","current_bst_err",update_tags);
  // doc += add_field("Boost Integral Error","current_bst_integral_err",update_tags);

  doc += add_field("Temp Error","current_temp_err",update_tags);
  // doc += add_field("Temp Integral Error","current_temp_integral_err",update_tags);
  

  doc += "<br><h3>Info And Modes</h3>";
  doc += add_field("Current Mode:","current_mode", update_tags);
  
  //doc += add_field("TH Sensor Good","temp_sensor_is_nominal", update_tags);
  doc += add_field("TH Sensor Failure","th_sensor_failure",update_tags);
  doc += add_field("Thermally Ready","thermally_ready", update_tags);
  
  //doc += add_field("PM Sensor Good","pm_sensor_is_nominal", update_tags);
  doc += add_field("PM Sensor Failure","pm_sensor_failure",update_tags);
  

  doc += add_field("Boost ON","is_boost_on",update_tags);
  doc += add_field("Out ON","is_out_on",update_tags);
  doc += add_field("Heat ON","is_heat_on",update_tags);

  doc += add_field("Clear Fan ON","is_clear_fan_on",update_tags);
  doc += add_field("Vac Fan ON","is_vac_fan_on",update_tags);

  doc += add_field("Clear Fan Duty","current_clear_fan_duty",update_tags);
  doc += add_field("Vac Fan Duty","current_vac_fan_duty",update_tags);




  doc += "<br><h3>Metrics</h3>";
  doc += add_field("Results PM Levels ","last_pm_levels",update_tags);
  doc += add_field("Results Particle Count (10xLOG)","last_particle_level",update_tags);
  doc += add_field("Results Typical Size","last_typical_particle_size",update_tags);
  doc += add_field("Results Estimated Mass Flow","last_mass_flow_est",update_tags);

  doc += add_field("Instant PM Levels ","inst_pm_levels",update_tags);
  doc += add_field("Instant Particle Count (10xLOG)","inst_particle_level",update_tags);
  doc += add_field("Instant Typical Size","inst_typical_particle_size",update_tags);
  doc += add_field("Instant Estimated Mass Flow","inst_mass_flow_est",update_tags);

  doc += add_field("Est Boost Power","current_boost_power",update_tags);
  doc += add_field("Est Out Power","current_out_power",update_tags);  
  doc += add_field("Overall Efficiency","current_efficency",update_tags);

  doc += add_field("V In Measured","measured_vin",update_tags);    

  doc += "<br><h3>Timing</h3>";
  doc += add_field("Test Time","current_run_time",update_tags);
  doc += add_field("Clear Time","current_clear_time",update_tags);
  doc += add_field("Vac Time","current_vac_time",update_tags);
  doc += add_field("Boost Time","current_boost_time",update_tags);

  doc += "</div>";

  doc += "<div style=\"display: table-cell;\">";
  doc += "<br><h3>Input</h3>";

  doc += "<br><button id=\"cycle_once\">Cycle Once</button> ";

  doc += "<br><button id=\"auto_mode_button\">Auto / Input Toggle</button> ";
  doc += "<br><button id=\"bst_on\">Boost Toggle</button> ";
  doc += "<br><button id=\"out_on\">Out Toggle</button> ";
  doc += "<br><button id=\"clr_on\">Clear Toggle</button> ";
  doc += "<br><button id=\"vac_on\">Vac Toggle</button> ";
  doc += "<br><button id=\"heat_on\">Heat Toggle</button> ";

  doc += "<br><button id=\"sendbtn\">Send New Settings</button> ";
  
  //doc += add_input("Manual 0 OR Automode Mode 1","auto_mode",AUTO_MODE,input_tags); //BOOL
  
  doc += add_input("Goal Temp C","temperature",(float)PROCESS_TEMP_SET,input_tags);
  doc += add_input("Temp Duty MAX","temp_duty_max",TEMP_MAX_DUTY,input_tags);

  doc += add_input("Boost Voltage ","boost_voltage",(float)V_BOOST_SET,input_tags);
  doc += add_input("Boost Frequency ","boost_frequency",BOOST_FREQUENCY,input_tags);

  doc += add_input("Boost Duty Max ON","bst_duty_max_on", BST_DUTY_MAX_ON,input_tags);
  doc += add_input("Boost Duty Max OFF","bst_duty_max_off",BST_DUTY_MAX_OFF,input_tags);
  doc += add_input("Boost Duty Min ON","bst_duty_min_on",BST_DUTY_MIN_ON,input_tags);
  doc += add_input("Boost Duty Min OFF","bst_duty_min_off",BST_DUTY_MIN_OFF,input_tags);

  doc += add_input("Out Frequency ","out_frequency",OUT_FREQUENCY,input_tags);
  doc += add_input("Out Duty ","out_duty",OUT_DUTY,input_tags);

  doc += add_input("Clear Fan Duty ","clr_duty",CLR_FAN_DUTY,input_tags);
  doc += add_input("Vac Fan Duty ","vac_duty",VAC_FAN_DUTY,input_tags);

  doc += add_input("Bst Control KP","bst_control_kp",float(Kp_boost),input_tags);
  doc += add_input("Bst Control KI","bst_control_ki",float(Ki_boost),input_tags);

  doc += add_input("Temp Control KP","tmp_control_kp",float(Kp_therm),input_tags);
  doc += add_input("Temp Control KI","tmp_control_ki",float(Ki_therm),input_tags);

  doc += add_input("Test Time (ms)","run_time",RUN_TIME,input_tags);
  doc += add_input("Clear Time","clear_time",CLEAR_TIME,input_tags);
  doc += add_input("Vac Time (ms)","vac_time",VAC_TIME,input_tags);
  doc += add_input("Boost Time (ms)","boost_time",BST_TIME,input_tags);  

  




  doc += "</div>";

  doc += "</div>";
  doc += "</body>";

  doc += "<footer>";

  doc += create_json_update_function( input_tags, "sendbtn", "process_setting" );
  
  doc += clickButton(  "cycle_once", "cycle_once" );
  doc += clickButton(  "auto_mode_button", "auto_mode_button" );
  doc += clickButton(  "bst_on", "bst_on" );
  doc += clickButton(  "out_on", "out_on" );
  doc += clickButton(  "clr_on", "clr_on" );
  doc += clickButton(  "vac_on", "vac_on" );
  doc += clickButton(  "heat_on", "heat_on" );
  
  doc += "</footer>";

  doc += "</html>";


  return doc;
}

String clickButton(String button_tag, String update_url){
  String doc = F("<script>$(document).ready(function(){"); 
  
   doc += "$(\"#"+button_tag+"\").click(function(){";

    doc += "  $.ajax({   url: '/"+update_url+"', type: 'GET',   data: {'empty':'yes'}, "; //$.ajax({
    doc += " contentType: 'application/json; charset=utf-8',   dataType: 'json',   async: false,   success: function(msg) {    }    }); "; //$.ajax({

    doc += "  }); "; //ON Button
    doc += "});</script>"; //Script

    return doc;  
}

String create_json_update_function(const vector<String> &update_tags, String button_tag, String update_url){
  String doc = F("<script>$(document).ready(function(){"); 
  
  doc += "$(\"#"+button_tag+"\").click(function(){";


    if(update_tags.size() > 0){

      String update_tag;
      for (int i=0; i<update_tags.size(); i++) {
        update_tag =  update_tags[i];
        doc += assign_field( update_tag );
      }

      doc += " var data = JSON.stringify({ ";

      for (int i=0; i<update_tags.size(); i++) {
        update_tag =  update_tags[i];

        doc += json_assign_field( update_tag );
        //Commas Except For Last
        if ( i< update_tags.size()-1){ doc += ","; }
      }
      
      doc += " }); console.log(data); "; //JSON.stringify({
    }


    doc += "  $.ajax({   url: '/"+update_url+"', type: 'POST',   data: data, "; //$.ajax({
    doc += " contentType: 'application/json; charset=utf-8',   dataType: 'json',   async: false,   success: function(msg) {    }    }); "; //$.ajax({

    doc += "  }); "; //ON Button
    doc += "});</script>"; //Script

    return doc;
}

// "<script>$(document).ready(function(){ $(\"#sendbtn\").click(function(){    let closetime = $('#close_time').val(); let feed_time = $('#feed_time').val(); let eject_time = $('#eject_time').val(); 
// let piston_time = $('#piston_time').val(); let delay_time = $('#delay_time').val();  let temp = $('#process_temperature').val();  let motor_rpm = $('#motor_rpm').val(); 
// let motor_acl = $('#motor_acl').val();  let motor_dcl = $('#motor_dcl').val(); let control_kp = $('#control_kp').val();  let control_ki = $('#control_ki').val(); let smsh_pwmbias = $('#smsh_pwmbias').val();  
//  var data = JSON.stringify({ \"process_temp\": parseInt(temp), \"close_time\": parseInt(closetime), \"feed_time\": parseInt(feed_time),\"eject_time\": parseInt(eject_time),
//  \"piston_time\": parseInt(piston_time),\"delay_time\": parseInt(delay_time),\"motor_rpm\": parseInt(motor_rpm),\"motor_acl\": parseInt(motor_acl),\"motor_dcl\": parseInt(motor_dcl),
//  \"control_kp\": parseFloat(control_kp),\"control_ki\": parseFloat(control_ki), \"smsh_pwmbias\": parseInt(smsh_pwmbias) });   console.log(data);    $.ajax({   url: '/process_setting', 
//    type: 'POST',   data: data,   contentType: 'application/json; charset=utf-8',   dataType: 'json',   async: false,   success: function(msg) {    }    });   }); });</script>";

String assign_field(String update_tag){
// let closetime = $('#close_time').val();
  return " let "+update_tag+" = $('#"+update_tag+"').val();";
}
  



String json_assign_field(String update_tag){  
  //  \"process_temp\": parseInt(temp)
    return " '"+update_tag+"': Number("+update_tag+")";
} 


String add_field(String visible_name, String tag_name, vector<String> &update_tags){
  String line;
  line += "<p>";
  line += visible_name;
  line += "<span id=\"";
  line += String(tag_name);
  line += "\"></span></p>";

  update_tags.push_back(tag_name);

  return line;
}


String add_input(String visible_name, String tag_name, int defaultvalue, vector<String> &mod_tags){
  String line;
  line += "<p>";
  line += visible_name;
  line += "</p>";
  line += "<input type=\"number\" id=\"";
  line += tag_name;
  line += "\" placeholder=\"";
  line += String(defaultvalue);
  line += "\" value=\"";
  line += String(defaultvalue);
  line += "\">";

  mod_tags.push_back(tag_name);

  return line;
}


String add_input(String visible_name, String tag_name, float defaultvalue, vector<String> &mod_tags){
  String line;
  line += "<p>";
  line += visible_name;
  line += "</p>";
  line += "<input type=\"number\" id=\"";
  line += tag_name;
  line += "\" placeholder=\"";
  line += String(defaultvalue);
  line += "\" value=\"";
  line += String(defaultvalue);
  line += "\">";

  mod_tags.push_back(tag_name);

  return line;
}



String current_mode_info(){ if( AUTO_MODE){ return "Automatic"; } else{ return "Input"; } }
String last_test_pm(){
  return "";
  }
String last_test_particle(){
  return "";
  }
String inst_test_pm(){
  return String(mass_average_particle_size)+"|"+
  String(mc_bin_010)+"|"+
  String(mc_bin_025)+"|"+
  String(mc_bin_040)+"|"+
  String(mc_bin_100);
  }
String inst_test_particle(){
  return String(m.typical_particle_size)+"|"+
  String(log_nc_bin_005)+"|"+
  String(log_nc_bin_010)+"|"+
  String(log_nc_bin_025)+"|"+
  String(log_nc_bin_040)+"|"+
  String(log_nc_bin_100);
  }



void print_boost_control_paramters_hf(){
    Serial.print( v_boost_raw);
    Serial.print(" ");
    Serial.print( v_boost );
    Serial.print(" ");

    Serial.print( v_out_pp );
    Serial.print(" ");

    Serial.print( boost_duty );
    Serial.print(" ");
    // Serial.print( hv_protection*10 );
    // Serial.print(" ");
    // Serial.print( v_out_raw);
    // Serial.print(" ");
    // Serial.print( v_out );
    // Serial.print(" ");
    Serial.print(" | ");
    Serial.print( m.typical_particle_size );
    Serial.print(" | ");
    Serial.print(log_nc_bin_005);
    Serial.print(" ");
    Serial.print(log_nc_bin_010);
    Serial.print(" ");
    Serial.print(log_nc_bin_025);
    Serial.print(" ");
    Serial.print(log_nc_bin_040);
    Serial.print(" ");
    Serial.print(log_nc_bin_100);    
    Serial.println( "" );
}

void print_once(){

    Serial.print( acd_int_vboost);
    Serial.print(" ");
    Serial.print( acd_int_vout );
    Serial.print(" ");
    Serial.print( v_boost_raw);
    Serial.print(" ");
    Serial.print( v_out_raw );

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
    Serial.print(heat_allowed);
    Serial.print("|");
    Serial.print(WiFi.localIP());

    Serial.print("|");
    Serial.print(  ESP.getFreeHeap());
    Serial.print("|");
    Serial.println(  thermocouple_failure_message );
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