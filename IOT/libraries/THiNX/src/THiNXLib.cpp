extern "C" {
  #include "user_interface.h"
  #include "thinx.h"
  #include <cont.h>
  #include <time.h>
  #include <stdlib.h>
  extern cont_t g_cont;
}

#include "THiNXLib.h"

#ifndef UNIT_TEST // IMPORTANT LINE FOR UNIT-TESTING!

#ifndef THINX_FIRMWARE_VERSION_SHORT
#define THINX_FIRMWARE_VERSION_SHORT VERSION
#endif

#ifndef THINX_COMMIT_ID
// any commit ID is sufficient to allow update
#define THINX_COMMIT_ID "0c48a9ab0c4f89c4b8fb72173553d3e74986632d0"
#endif

char* THiNX::thinx_api_key;
char* THiNX::thinx_owner_key;

bool THiNX::forceHTTP = false;

const char THiNX::time_format[] = "%T";
const char THiNX::date_format[] = "%Y-%m-%d";
char * THiNX::thinx_mqtt_url = strdup(THINX_MQTT_URL);

#include "thinx_root_ca.h"

#ifdef __USE_WIFI_MANAGER__
char THiNX::thx_api_key[65] = {0};
char THiNX::thx_owner_key[65] = {0};
int THiNX::should_save_config = 0;

WiFiManagerParameter * THiNX::api_key_param;
WiFiManagerParameter * THiNX::owner_param;

void THiNX::saveConfigCallback() {
  Serial.println(F("*TH: WiFiManager's saveConfigCallback called. Counfiguration should be saved now!"));
  should_save_config = true;
  strcpy(thx_api_key, api_key_param->getValue());
  strcpy(thx_owner_key, owner_param->getValue());
}
#endif

double THiNX::latitude = 0.0;
double THiNX::longitude = 0.0;
String THiNX::statusString = "Registered";
String THiNX::accessPointName = "THiNX-AP";
String THiNX::accessPointPassword = "PASSWORD";
String THiNX::lastWill = "{ \"status\" : \"disconnected\" }";

/* Constructor */

THiNX::THiNX() {

}

/* Designated Initializers */

THiNX::THiNX(const char * __apikey) {

  THiNX(__apikey, "");
}

THiNX::THiNX(const char * __apikey, const char * __owner_id) {

  thinx_phase = INIT;

  #ifdef __USE_WIFI_MANAGER__
  should_save_config = false;
  WiFiManager wifiManager;
  api_key_param = new WiFiManagerParameter("apikey", "API Key", thinx_api_key, 64);
  wifiManager.addParameter(api_key_param);
  owner_param = new WiFiManagerParameter("owner", "Owner ID", thinx_owner_key, 64);
  wifiManager.addParameter(owner_param);
  wifiManager.setTimeout(5000);
  wifiManager.setDebugOutput(true); // does some logging on mode set
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.autoConnect(accessPointName.c_str());
  #endif

  Serial.print(F("\n*TH: THiNXLib rev. "));
  Serial.print(thx_revision);
  Serial.print(" version: ");
  Serial.println(VERSION);

  Serial.println(thinx_commit_id); // returned string is "not declared in expansion of THX_CID, why?

#ifdef DEBUG
  // see lines ../hardware/cores/esp8266/Esp.cpp:80..100
  //wdt_disable(); // causes wdt reset after 8 seconds!
  //wdt_enable(60000); // must be called from wdt_disable() state!
#endif

  if (once != true) {
    once = true;
  }

  status = WL_IDLE_STATUS;
  wifi_connected = false;
  mqtt_client = NULL;
  mqtt_payload = "";
  mqtt_connected = false;
  performed_mqtt_checkin = false;
  wifi_connection_in_progress = false;
  wifi_retry = 0;

  app_version = strdup("");
  available_update_url = strdup("");
  thinx_cloud_url = strdup("thinx.cloud");

  thinx_firmware_version_short = strdup("");
  thinx_firmware_version = strdup("");
  thinx_mqtt_url = strdup("thinx.cloud");
  thinx_version_id = strdup("");
  thinx_api_key = strdup("");
  thinx_forced_update = false;
  last_checkin_timestamp = 0; // 1/1/1970

  checkin_time = millis() + checkin_interval / 4; // retry faster before first checkin
  reboot_interval = millis() + reboot_timeout;

  // will be loaded from SPIFFS/EEPROM or retrieved on Registration later
  if (strlen(__owner_id) == 0) {
    thinx_owner = strdup("");
  }

  EEPROM.begin(512); // should be SPI_FLASH_SEC_SIZE

  import_build_time_constants();
  restore_device_info();
  info_loaded = true;

  #ifdef __USE_WIFI_MANAGER__
  wifi_connected = true;
  #else
  if ((WiFi.status() == WL_CONNECTED) && (WiFi.getMode() == WIFI_STA)) {
    wifi_connected = true;
    wifi_connection_in_progress = false;
  } else {
    WiFi.mode(WIFI_STA);
  }
  #endif

  if (strlen(__apikey) > 4) {
    thinx_api_key = strdup(__apikey);
  } else {
      if (strlen(thinx_api_key) > 4) {
          Serial.print(F("*TH: With thinx.h API Key..."));
      } else {
          Serial.print(F("*TH: No API Key!"));
          return;
      }
  }

  if (strlen(__owner_id) > 4) {
    thinx_owner = strdup(__owner_id);
  } else {
      if (strlen(thinx_owner) > 4) {
          Serial.print(F("*TH: With thinx.h owner..."));
      } else {
          Serial.print(F("*TH: No API Key!"));
          return;
      }
  }

  init_with_api_key(thinx_api_key);
  wifi_connection_in_progress = false; // last
}

// Designated initializer
void THiNX::init_with_api_key(const char * __apikey) {

  #ifdef __USE_SPIFFS__
  Serial.println(F("*TH: Checking filesystem, please don't turn off or reset the device now..."));
  if (!fsck()) {
    Serial.println(F("*TH: Filesystem check failed, disabling THiNX."));
    return;
  }
  #endif

  if (info_loaded == false) {
    restore_device_info(); // loads saved apikey/ownerid
    info_loaded = true;
  }

  if (strlen(__apikey) > 4) {
    thinx_api_key = strdup(__apikey);
  } else {
    if (strlen(thinx_api_key) < 4) {
      Serial.print(F("*TH: No API Key!"));
      return;
    }
  }

  wifi_connection_in_progress = false;
  thinx_phase = CONNECT_WIFI;
}

/*
* Connection management
*/

char* THiNX::get_udid() {
  return strdup(thinx_udid);
}

void THiNX::connect() {

  if (wifi_connected) {
    Serial.println(F("*TH: connected"));
    return;
  }

  Serial.print(F("*TH: connecting: ")); Serial.println(wifi_retry);

  #ifndef __USE_WIFI_MANAGER__
  if (WiFi.SSID()) {

    if (wifi_connection_in_progress != true) {
      Serial.print(F("*TH: SSID ")); Serial.println(WiFi.SSID());
      if (WiFi.getMode() == WIFI_AP) {
        Serial.print(F("THiNX > LOOP > START() > AP SSID"));
        Serial.println(WiFi.SSID());
      } else {
        if (strlen(THINX_ENV_SSID) > 2) {
          Serial.println(F("*TH: LOOP > CONNECT > STA RECONNECT"));
          WiFi.begin(THINX_ENV_SSID, THINX_ENV_PASS);
          Serial.println(F("*TH: Enabling connection state (197)"));
        } else {
          Serial.println(F("*TH: LOOP > CONNECT > NO CUSTOM CREDS"));
        }
        wifi_connection_in_progress = true; // prevents re-entering connect_wifi(); should timeout
      }
      //
    }
  } else {
    Serial.print(F("*TH: No SSID."));
  }
  #endif

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("THiNX > LOOP > ALREADY CONNECTED"));
    wifi_connected = true; // prevents re-entering start() [this method]
    wifi_connection_in_progress = false;
  } else {
    Serial.println(F("THiNX > LOOP > CONNECTING WiFi:"));
    connect_wifi();
    Serial.println(F("*TH: Enabling connection state (237)"));
    wifi_connection_in_progress = true;
  }
}

/*
* Connection to WiFi, called from connect() [if SSID & connected]
*/

void THiNX::connect_wifi() {

  #ifdef __USE_WIFI_MANAGER__
  return;
  #else

  if (wifi_connected) {
    return;
  }

  if (wifi_connection_in_progress) {
    if (wifi_retry > 1000) {
      if (WiFi.getMode() == WIFI_STA) {
        Serial.println(F("*TH: Starting AP with PASSWORD..."));
        WiFi.mode(WIFI_AP);
        WiFi.softAP(accessPointName.c_str(), accessPointPassword.c_str()); // setup the AP on channel 1, not hidden, and allow 8 clients
        wifi_retry = 0;
        wifi_connection_in_progress = false;
        wifi_connected = true;
        return;
      } else {
        if (strlen(THINX_ENV_SSID) > 2) {
          Serial.println(F("*TH: Connecting to AP with pre-defined credentials...")); Serial.flush();
          WiFi.mode(WIFI_STA);
          WiFi.begin(THINX_ENV_SSID, THINX_ENV_PASS);
          Serial.println(F("*TH: Enabling connection state (283)"));
          wifi_connection_in_progress = true; // prevents re-entering connect_wifi()
          wifi_retry = 0; // waiting for sta...
        }
      }

    } else {
      Serial.print("*TH: WiFi retry"); Serial.println(wifi_retry); Serial.flush();
      wifi_retry++;
    }

  } else {

    if (strlen(THINX_ENV_SSID) > 2) {
      if (wifi_retry == 0) {
        Serial.println(F("*TH: Connecting to AP with pre-defined credentials..."));
        // 1st run
        if (WiFi.getMode() != WIFI_STA) {
          WiFi.mode(WIFI_STA);
        } else {
          WiFi.begin(THINX_ENV_SSID, THINX_ENV_PASS);
          Serial.println(F("*TH: Enabling connection state (272)"));
          wifi_connection_in_progress = true; // prevents re-entering connect_wifi()
        }
      }
    }
  }
  #endif
}

/*
* Registration
*/

void THiNX::checkin() {
  Serial.println(thinx_time(NULL));
  Serial.println(F("\n*TH: Contacting API..."));
  if(!wifi_connected) {
    Serial.println(F("*TH: Cannot checkin while not connected, exiting."));
  } else {
    String body = checkin_body();
    if (thx_ca_cert_len == 0 || forceHTTP) {
      senddata(body); // HTTP fallback
    } else {
      send_data(body); // HTTPS
    }
    checkin_time = millis() + checkin_interval;
  }
}

/*
* Registration - JSON body constructor
*/

String THiNX::checkin_body() {

  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& root = jsonBuffer.createObject();

  root["mac"] = thinx_mac();

  if (strlen(thinx_firmware_version) > 1) {
    root["firmware"] = thinx_firmware_version;
  } else {
    root["firmware"] = THINX_FIRMWARE_VERSION;
  }

  if (strlen(thinx_firmware_version_short) > 1) {
    root["version"] = thinx_firmware_version_short;
  }

  if (strlen(thx_commit_id) > 1) {
    root["commit"] = thx_commit_id;
  }

  if (strlen(thinx_owner) > 1) {
    root["owner"] = thinx_owner;
  }

  if (strlen(thinx_alias) > 1) {
    root["alias"] = thinx_alias;
  }

  if (strlen(thinx_udid) > 4) {
    root["udid"] = thinx_udid;
  }

  if (statusString.length() > 0) {
    root["status"] = statusString.c_str();
  }

  // Optional location data
  root["lat"] = String(latitude);
  root["lon"] = String(longitude);

  root["rssi"] = String(WiFi.RSSI());
  // root["snr"] = String(100 + WiFi.RSSI() / WiFi.RSSI()); // approximate only

  // Flag for THiNX CI
  #ifndef PLATFORMIO_IDE
  // THINX_PLATFORM is not overwritten by builder in Arduino IDE
  root["platform"] = "arduino";
  #else
  root["platform"] = strdup(THINX_PLATFORM);
  #endif

  DynamicJsonBuffer wrapperBuffer(512);
  JsonObject& wrapper = wrapperBuffer.createObject();
  wrapper["registration"] = root;

  json_output = "";
  wrapper.printTo(json_output);
  return json_output;
}


/*
* Registration - HTTPS POST
*/

void THiNX::senddata(String body) {

  if (thx_wifi_client.connect(thinx_cloud_url, 7442)) {

    thx_wifi_client.println(F("POST /device/register HTTP/1.1"));
    thx_wifi_client.print(F("Host: ")); thx_wifi_client.println(thinx_cloud_url);
    thx_wifi_client.print(F("Authentication: ")); thx_wifi_client.println(thinx_api_key);
    thx_wifi_client.println(F("Accept: application/json")); // application/json
    thx_wifi_client.println(F("Origin: device"));
    thx_wifi_client.println(F("Content-Type: application/json"));
    thx_wifi_client.println(F("User-Agent: THiNX-Client"));
    thx_wifi_client.println(F("Connection: close"));
    thx_wifi_client.print(F("Content-Length: "));
    thx_wifi_client.println(body.length());
    thx_wifi_client.println();
    thx_wifi_client.println(body);

    fetch_data();

  } else {
    Serial.println(F("*TH: API connection failed."));
    return;
  }
}

void THiNX::fetch_data() {

  Serial.println(F("*TH: Waiting for API response..."));

  char buf[512];
  int pos = 0;

  long interval = 30000;
  unsigned long currentMillis = millis(), previousMillis = millis();

  // Wait until client available or timeout...
  while(!thx_wifi_client.available()){
    delay(1);
    if( (currentMillis - previousMillis) > interval ){
      thx_wifi_client.stop();
      return;
    }
    currentMillis = millis();
  }

  // Read while connected
  bool headers_passed = false;
  while ( thx_wifi_client.connected() ) {
    String line = "    ";
    // Wait for empty line to drop headers and process only JSON data in parser...
    if (!headers_passed) {
        line = thx_wifi_client.readStringUntil('\n');
        //Serial.print("HEADERS > ");
        //Serial.println(line);
        if (line.length() < 3) {
          headers_passed = true;
        }
    } else {
      if ( thx_wifi_client.available() ) {
          buf[pos] = thx_wifi_client.read();
          pos++;
      }
    }
  }

  buf[pos] = '\0'; // add null termination for any case...

  //thx_wifi_client.stop(); // ?? may cause mqtt stop

  Serial.printf("*TH: Received %u bytes\n", pos);
  //printStackHeap("allocating string");
  String payload = String(buf);
  //printStackHeap("string allocated");
  parse(payload);

}

/* Secure version */
void THiNX::send_data(String body) {

  Serial.println(F("Secure API checkin..."));

  if (https_client.connect(thinx_cloud_url, 7443)) {

    // Load root certificate in DER format into WiFiClientSecure object
    bool res = https_client.setCACert_P(thx_ca_cert, thx_ca_cert_len);
    if (!res) {
      Serial.println(F("*TH: Failed to load root CA certificate!"));
    }

    // Verify validity of server's certificate
    if (https_client.verifyCertChain(thinx_cloud_url)) {
      Serial.println(F("*TH: Server certificate verified. Handshake will take about 120 seconds now... keep calm."));
    } else {
      Serial.println(F("*TH: ERROR: certificate verification failed!"));
      return;
    }

    https_client.println(F("POST /device/register HTTP/1.1"));
    https_client.print(F("Host: ")); https_client.println(thinx_cloud_url);
    https_client.print(F("Authentication: ")); https_client.println(thinx_api_key);
    https_client.println(F("Accept: application/json")); // application/json
    https_client.println(F("Origin: device"));
    https_client.println(F("Content-Type: application/json"));
    https_client.println(F("User-Agent: THiNX-Client"));
    https_client.print(F("Content-Length: "));
    https_client.println(body.length());
    https_client.println();
    https_client.println(body);

    fetch_data();

  } else {
    Serial.println(F("*TH: API connection failed."));
    return;
  }
}

/*
* Response Parser
*/

int strpos(char *hay, char *needle, int offset)
{
    char haystack[strlen(hay)];
    strncpy(haystack, hay+offset, strlen(hay)-offset);
    char *p = strstr(haystack, needle);
    if (p)
        return p - haystack+offset;
    return -1;
}

void THiNX::parse(String payload) {

  payload_type ptype = Unknown;

  int start_index = -1;
  int endIndex = payload.length();

  int reg_index = payload.indexOf("{\"registration\"");
  int upd_index = payload.indexOf("{\"FIRMWARE_UPDATE\"");
  int not_index = payload.indexOf("{\"notification\"");
  int cfg_index = payload.indexOf("{\"configuration\"");
  int undefined_owner = payload.indexOf("old_protocol_owner:-undefined-");

  if (upd_index > start_index) {
    start_index = upd_index;
    ptype = UPDATE;
  }

  if (reg_index > start_index) {
    start_index = reg_index;
    endIndex = payload.indexOf("}}") + 2;
    ptype = REGISTRATION;
  }

  if (not_index > start_index) {
    start_index = not_index;
    endIndex = payload.indexOf("}}") + 2; // is this still needed?
    ptype = NOTIFICATION;
  }

  if (cfg_index > start_index) {
    start_index = cfg_index;
    endIndex = payload.indexOf("}}") + 2; // is this still needed?
    ptype = CONFIGURATION;
  }

  if (ptype == Unknown) {
    Serial.println(F("ptype: Not a THiNX message."));
    return;
  }

  if (undefined_owner > start_index) {
    Serial.println(F("ERROR: Not authorized. Please copy your owner_id into thinx.h from RTM Console > User Profile."));
    return;
  }

  String body = payload.substring(start_index, endIndex);
  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& root = jsonBuffer.parseObject(body.c_str());

  if ( !root.success() ) {
    Serial.println(F("Failed parsing root node."));
    return;
  }

  switch (ptype) {

    case UPDATE: {

      Serial.println(F("ptype case UPDATE")); Serial.flush();

      JsonObject& update = root["registration"];

      Serial.println(F("TODO: Parse update payload..."));

      String mac = update["mac"];
      String this_mac = String(thinx_mac());
      Serial.println(String("mac: ") + mac);

      if (!mac.equals(this_mac)) {
        Serial.println(F("*TH: Warning: firmware is dedicated to device with different MAC."));
      }

      String udid = root["udid"];
      if ( udid.length() > 4 ) {
        thinx_udid = strdup(udid.c_str());
      }

      // Check current firmware based on commit id and store Updated state...
      String commit = update["commit"];
      Serial.println(String("commit: ") + commit);

      // Check current firmware based on version and store Updated state...
      String version = update["version"];
      Serial.println(String("version: ") + version);

      //if ((commit == thinx_commit_id) && (version == thinx_version_id)) { WHY?
      if (strlen(available_update_url) > 5) {
        Serial.println(F("*TH: firmware has same thx_commit_id as current and update availability is stored. Firmware has been installed."));
        available_update_url = strdup("");
        notify_on_successful_update();
        return;
      } else {
        Serial.println(F("*TH: Info: firmware has same thx_commit_id as current and no update is available."));
      }

      save_device_info();

      // In case automatic updates are disabled,
      // we must ask user to commence firmware update.
      if (thinx_auto_update == false) {
        if (mqtt_client != NULL) {
          Serial.println(F("*TH: Update availability notification..."));
          mqtt_client->publish(
            thinx_mqtt_channel().c_str(),
            F("{ title: \"Update Available\", body: \"There is an update available for this device. Do you want to install it now?\", type: \"actionable\", response_type: \"bool\" }")
          );
          mqtt_client->loop();
        }

      } else if (thinx_auto_update || thinx_forced_update){

        Serial.println(F("*TH: Starting update A..."));

        String type = update["type"];
        Serial.print(F("*TH: Payload type: ")); Serial.println(type);

        String files = update["files"];

        String url = update["url"]; // may be OTT URL
        available_update_url = url.c_str();

        String ott = update["ott"];
        available_update_url = ott.c_str();

        String hash = update["hash"];
        if (hash.length() > 2) {
          Serial.print(F("*TH: #")); Serial.println(hash);
          expected_hash = strdup(hash.c_str());
        }

        String md5 = update["md5"];
        if (md5.length() > 2) {
          Serial.print(F("*TH: #")); Serial.println(md5);
          expected_md5 = strdup(md5.c_str());
        }

        Serial.println(F("Saving device info before firmware update.")); Serial.flush();
        save_device_info();

        if (url) {
          mqtt_client->publish(
            mqtt_device_status_channel,
            F("{ \"status\" : \"update_started\" }")
          );

          mqtt_client->loop();
          Serial.print(F("*TH: Force update URL must not contain HTTP!!!: "));
          Serial.println(url);
          url.replace("http://", "");
          // TODO: must not contain HTTP, extend with http://thinx.cloud/"
          update_and_reboot(url);
        }
        return;
      }

    } break;

    case NOTIFICATION: {

      // Currently, this is used for update only, can be extended with request_category or similar.
      JsonObject& notification = root["notification"];

      if ( !notification.success() ) {
        Serial.println(F("*TH: Failed parsing notification node."));
        return;
      }

      String type = notification["response_type"];
      if ((type == "bool") || (type == "boolean")) {
        bool response = notification["response"];
        if (response == true) {
          Serial.println(F("*TH: User allowed update using boolean."));
          if (strlen(available_update_url) > 4) {
            update_and_reboot(available_update_url);
          }
        } else {
          Serial.println(F("*TH: User denied update using boolean."));
        }
      }

      if ((type == "string") || (type == "String")) {
        String response = notification["response"];
        if (response == "yes") {
          Serial.println(F("*TH: User allowed update using string."));
          if (strlen(available_update_url) > 4) {
            update_and_reboot(available_update_url);
          }
        } else if (response == "no") {
          Serial.println(F("*TH: User denied update using string."));
        }
      }

    } break;

    case REGISTRATION: {

      JsonObject& registration = root["registration"];

      if ( !registration.success() ) {
        Serial.println(F("*TH: Failed parsing registration node."));
        return;
      }

      // bool success = registration["success"]; unused
      String status = registration["status"];

      if (status == "OK") {

        String alias = registration["alias"];
        if ( alias.length() > 1 ) {
          thinx_alias = strdup(alias.c_str());
        }

        String owner = registration["owner"];
        if ( owner.length() > 1 ) {
          thinx_owner = strdup(owner.c_str());
        }

        String udid = registration["udid"];
        if ( udid.length() > 4 ) {
          thinx_udid = strdup(udid.c_str());
        }

        if (registration.containsKey(F("auto_update"))) {
          thinx_auto_update = (bool)registration[F("auto_update")];
        }

        if (registration.containsKey(F("forced_update"))) {
          thinx_forced_update = (bool)registration[F("forced_update")];
        }

        if (registration.containsKey(F("timestamp"))) {
          Serial.print(F("*TH: Updating THiNX time: "));
          last_checkin_timestamp = (long)registration[F("timestamp")] + timezone_offset * 3600;
          last_checkin_millis = millis();
          Serial.print(thinx_time(NULL));
          Serial.print(" ");
          Serial.println(thinx_date(NULL));
        }

        save_device_info();

      } else if (status == "FIRMWARE_UPDATE") {

        // Warning, this branch may be deprecated!

        String udid = registration["udid"];
        if ( udid.length() > 4 ) {
          thinx_udid = strdup(udid.c_str());
        }

        Serial.println(F("Saving device info for update.")); Serial.flush();
        save_device_info();

        String mac = registration["mac"];
        Serial.println(String("*TH: Update for MAC: ") + mac);
        // TODO: must be current or 'ANY'

        // commit should not be same except for forced update
        String commit = registration["commit"];
        Serial.println(String("commit: ") + commit);
        if (commit == thinx_commit_id) {
          Serial.println(F("*TH: Info: new firmware has same thx_commit_id as current."));
        }

        String version = registration["version"];
        Serial.println(String(F("*TH: version: ")) + version);

        if (thinx_auto_update == false) {
          Serial.println(String(F("*TH: Skipping auto-update (disabled).")));
          return;
        }

        String update_url;

        String url = registration["url"];
        if (url.length() > 2) {
          Serial.println(F("*TH: Starting direct update..."));
          update_url = url;
        }

        String ott = registration["ott"];
        if (ott.length() > 2) {
          Serial.println(F("*TH: Starting OTT update..."));
          update_url = "http://thinx.cloud:7442/device/firmware?ott="+ott;
        }

        String hash = registration["hash"];
        if (hash.length() > 2) {
          Serial.print(F("*TH: #")); Serial.println(hash);
          expected_hash = strdup(hash.c_str());
        }

        String md5 = registration["md5"];
        if (md5.length() > 2) {
          Serial.print(F("*TH: #")); Serial.println(md5);
          expected_md5 = strdup(md5.c_str());
        }

        Serial.println(update_url);
        update_and_reboot(update_url);
        return;

      }

    } break;

    case CONFIGURATION: {

      JsonObject& configuration = root["configuration"];

      if ( !configuration.success() ) {
        Serial.println(F("*TH: Failed parsing configuration node."));
        return;
      }


      #ifdef __ENABLE_WIFI_MIGRATION__
      //
      // Built-in support for WiFi migration
      //

      const char *ssid = configuration["THINX_ENV_SSID"];
      const char *pass = configuration["THINX_ENV_PASS"];

      // password may be empty string
      if ((strlen(ssid) > 2) && (strlen(pass) > 0)) {
        WiFi.disconnect();
        WiFi.begin(ssid, pass);
        unsigned long timeout = millis() + 20000;
        Serial.println(F("*TH: Attempting WiFi migration..."));
        while (WiFi.status() != WL_CONNECTED) {
          yield();
          if (millis() > timeout) break;
        }
        if (WiFi.status() != WL_CONNECTED) {
          Serial.println(F("*TH: WiFi migration failed."));
        } else {
          Serial.println(F("*TH: WiFi migration successful.")); // TODO: Notify using publish() to device status topic
        }
      }
      #endif
      // Forward update body to the library user
      if (_config_callback != NULL) {
        _config_callback(body);
      }

    } break;

    default:
    break;
  }

}

/*
* MQTT channel names
*/

String THiNX::thinx_mqtt_channel() {
  sprintf(mqtt_device_channel, "/%s/%s", thinx_owner, thinx_udid);
  return String(mqtt_device_channel);
}

String THiNX::thinx_mqtt_channels() {
  sprintf(mqtt_device_channels, "/%s/%s/#", thinx_owner, thinx_udid);
  return String(mqtt_device_channels);
}

String THiNX::thinx_mqtt_status_channel() {
  sprintf(mqtt_device_status_channel, "/%s/%s/status", thinx_owner, thinx_udid);
  return String(mqtt_device_status_channel);
}

unsigned long THiNX::epoch() {
  unsigned long since_last_checkin = (millis() - last_checkin_millis) / 1000;
  return last_checkin_timestamp + since_last_checkin;
}

String THiNX::thinx_time(const char* optional_format) {

  char *format = strdup(time_format);
  if (optional_format != NULL) {
    format = strdup(optional_format);
  }

  long stamp = THiNX::epoch();
  struct tm lt;
  char res[32];
  (void) localtime_r(&stamp, &lt);
  if (strftime(res, sizeof(res), format, &lt) == 0) {
      Serial.println(F("cannot format supplied time into buffer"));
  }
  return String(res);
}

String THiNX::thinx_date(const char* optional_format) {

  char *format = strdup(date_format);
  if (optional_format != NULL) {
    format = strdup(optional_format);
  }

  long stamp = THiNX::epoch();
  struct tm lt;
  char res[32];
  (void) localtime_r(&stamp, &lt);
  if (strftime(res, sizeof(res), format, &lt) == 0) {
      Serial.println(F("cannot format supplied date into buffer"));
  }
  return String(res);
}

/*
* Device MAC address
*/

const char * THiNX::thinx_mac() {
  sprintf(mac_string, "5CCF7F%06X", ESP.getChipId()); // ESP8266 only! needs alignment...
  return mac_string;
}

/*
* Sends a MQTT message on successful update (should be used after boot).
*/

void THiNX::notify_on_successful_update() {
  if (mqtt_client != NULL) {
    Serial.println(F("*TH: notify_on_successful_update()"));
    mqtt_client->publish(
      mqtt_device_status_channel,
      F("{ title: \"Update Successful\", body: \"The device has been successfully updated.\", type: \"success\" }")
    );
    mqtt_client->loop();
  } else {
    Serial.println(F("*TH: Device updated but MQTT not active to notify. TODO: Store."));
  }
}

/*
* Sends a MQTT message to Device's status topic (/owner/udid/status)
*/

void THiNX::publishStatus(String message) {
  publishStatusRetain(message, true);
}

// Old version, leaks strings, deprecated.
void THiNX::publishStatusUnretained(String message) {
  publishStatusRetain(message, false);
}

void THiNX::publish_status_unretained(char *message) {
  publish_status(message, false);
}

// Old version, leaks strings, deprecated.
void THiNX::publishStatusRetain(String message, bool retain) {
  if (mqtt_client != NULL) {
    if (retain) {
      mqtt_client->publish(
        MQTT::Publish(mqtt_device_status_channel, message.c_str()).set_retain()
      );
    } else {
      mqtt_client->publish(mqtt_device_status_channel, message.c_str());
    }
    mqtt_client->loop();
  } else {
    Serial.println(F("*TH: MQTT not active while trying to publish retained status."));
  }
}

void THiNX::publish_status(char *message, bool retain) {
  if (mqtt_client != NULL) {
    if (!mqtt_client->connected()) {
      Serial.println(F("*TH: reconnecting MQTT..."));
      start_mqtt();
      unsigned long reconnect_timeout = millis() + 10000;
      while (!mqtt_client->connected()) {
        mqtt_client->loop();
        delay(1);
        if (millis() > reconnect_timeout) break;
      }
    }
    if (retain) {
      mqtt_client->publish(
        MQTT::Publish(mqtt_device_status_channel, message).set_retain()
      );
      Serial.println(F("*TH: publish_status retained")); Serial.flush();
    } else {
      mqtt_client->publish(mqtt_device_status_channel, message); // this is weird but already tried to use object in retain version and does not consume
      Serial.println(F("*TH: publish_status unretained")); Serial.flush();
    }
    mqtt_client->loop();
  } else {
    Serial.println(F("*TH: MQTT not active while trying to publish retained status."));
  }
}

/*
* Sends a MQTT message to the Device Channel (/owner/udid)
*/

// Old version, leaks strings, deprecated.
void THiNX::publish(String message, String topic, bool retain)  {
  String channel = String(mqtt_device_channel) + String("/") + String(topic);
  if (mqtt_client != NULL) {
    if (retain == true) {
      mqtt_client->publish(
        MQTT::Publish(channel.c_str(), message.c_str()).set_retain()
      );
    } else {
      mqtt_client->publish(
        channel.c_str(), message.c_str()
      );
    }
    mqtt_client->loop();
  } else {
    Serial.println(F("*TH: MQTT not active while trying to publish message."));
  }
}

void THiNX::publish(char * message, char * topic, bool retain)  {
  char channel[256] = {0};
  sprintf(channel, "%s/%s", mqtt_device_channel, topic);
  if (mqtt_client != NULL) {
    if (retain == true) {
      mqtt_client->publish(
        MQTT::Publish(channel, message).set_retain()
      );
    } else {
      mqtt_client->publish(
        channel, message
      );
    }
    mqtt_client->loop();
  } else {
    Serial.println(F("*TH: MQTT not active while trying to publish message."));
  }
}

/*
* Starts the MQTT client and attach callback function forwarding payload to parser.
*/

void setLastWill(String nextWill) {
  THiNX::lastWill = nextWill;
}

void THiNX::setLastWill(String nextWill) {
    mqtt_client->disconnect();
    start_mqtt();
}

bool THiNX::start_mqtt() {

  if (mqtt_client != NULL) {
    Serial.println(F("*TH: start_mqtt")); Serial.flush();
    mqtt_connected = mqtt_client->connected();
    if (mqtt_connected) {
      return true;
    }
  }

  if (strlen(thinx_udid) < 4) {
    Serial.println(F("*TH: MQTT NO-UDID!")); Serial.flush();
    return false;
  }

  if (forceHTTP == true) {
    Serial.println(F("*TH: Contacting MQTT server over HTTP..."));
    mqtt_client = new PubSubClient(thx_wifi_client, thinx_mqtt_url);
  } else {
    Serial.println(F("*TH: Contacting MQTT server over HTTPS..."));
    bool res = https_client.setCACert_P(thx_ca_cert, thx_ca_cert_len);
    if (res) {
      mqtt_client = new PubSubClient(https_client, thinx_mqtt_url);
    } else {
      Serial.println(F("*TH: Failed to load root CA certificate for MQTT!"));
    }
  }

  if (strlen(thinx_api_key) < 5) {
    Serial.println(F("*TH: API Key not set, exiting."));
    return false;
  }

  if (mqtt_client->connect(MQTT::Connect(thinx_mac())
    .set_will(thinx_mqtt_status_channel().c_str(), lastWill.c_str())
    .set_auth(thinx_udid, thinx_api_key)
    .set_keepalive(120))) {

    mqtt_connected = true;
    performed_mqtt_checkin = true;

    mqtt_client->set_callback([this](const MQTT::Publish &pub){

      // Stream has been never tested so far...
      if (pub.has_stream()) {
        Serial.println(F("*TH: MQTT Type: Stream..."));
        uint32_t startTime = millis();
        uint32_t size = pub.payload_len();
        if ( ESP.updateSketch(*pub.payload_stream(), size, true, false) ) {
          // Notify on reboot for update
          mqtt_client->publish(
            mqtt_device_status_channel,
            "{ \"status\" : \"rebooting\" }"
          );
          mqtt_client->disconnect();
          pub.payload_stream()->stop();
          Serial.printf("Update Success: %lu\nRebooting...\n", millis() - startTime);
          ESP.restart();
        } else {
          Serial.println(F("*TH: ESP MQTT Stream update failed..."));
          mqtt_client->publish(
            mqtt_device_status_channel,
            "{ \"status\" : \"mqtt_update_failed\" }"
          );
        }

      } else {
        Serial.println(F("*TH: MQTT Type: String or JSON..."));
        Serial.println(pub.payload_string());
        parse(pub.payload_string());
        if (_mqtt_callback) {
            _mqtt_callback(pub.payload_string());
        }
      }
    }); // end-of-callback

    return true;

  } else {
    mqtt_connected = false;
    Serial.println(F("*TH: MQTT Not connected."));

    /*
    #ifdef __DEBUG__
          Serial.println(F("*TH: Failed to load root CA certificate for MQTT, falling back to HTTP!"));
          forceHTTP = true;
          mqtt_client = new PubSubClient(thx_wifi_client, thinx_mqtt_url);
          return start_mqtt();
    #else
          Serial.println(F("*TH: Failed to connect using MQTTS!"));
          //ESP.restart();
    #endif
    */

    return false;
  }
}

/*
* Restores Device Info. Callers (private): init_with_api_key; save_device_info()
* Provides: alias, owner, update, udid, (apikey)
*/

void THiNX::restore_device_info() {

  Serial.println(F("*TH: Restoring device info..."));

  #ifndef __USE_SPIFFS__

  int value;
  long buf_len = 512;
  long data_len = 0;

  Serial.println(F("*TH: Restoring configuration from EEPROM..."));

  for (long a = 0; a < buf_len; a++) {
    value = EEPROM.read(a);
    json_output += char(value);

    // validate at least data start
    if (a == 0) {
      if (value != '{') {
        return; // Not a JSON, nothing to do...
      }
    }
    if (value == '{') {
      json_end++;
    }
    if (value == '}') {
      json_end--;
    }
    if (value == 0) {
      json_info[a] = char(value);
      data_len++;
      Serial.print("*TH: "); Serial.print(a); Serial.println(F(" bytes read from EEPROM."));
      // Validate JSON
      break;
    } else {
      json_info[a] = char(value);
      data_len++;
    }
    // Serial.flush(); // to debug reading EEPROM bytes
  }

  Serial.print("Restore json_output: ");
  Serial.println(json_output);

  // Validating bracket count
  if (json_end != 0) {
    Serial.println(F("*TH: JSON invalid... bailing out."));
    return;
  }

  Serial.println(F("*TH: Converting data to String..."));

  #else
  if (!SPIFFS.exists("/thinx.cfg")) {
    Serial.println(F("*TH: No saved configuration."));
    return;
  }
  File f = SPIFFS.open("/thinx.cfg", "r");
  Serial.println(F("*TH: Found persistent data..."));
  if (!f) {
    Serial.println(F("*TH: No remote configuration found so far..."));
    return;
  }
  if (f.size() == 0) {
    Serial.println(F("*TH: Remote configuration file empty..."));
    return;
  }

  f.readBytesUntil('\r', json_info, sizeof(json_info));
  //Serial.print("Loaded json_info: ");
  //Serial.print("length: ");
  //Serial.print(f.size());
  //Serial.println(String(json_info));
  #endif

  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& config = jsonBuffer.parseObject((char*)json_info); // must not be String!

  if (!config.success()) {
    Serial.println(F("*TH: No JSON data to be parsed..."));
    Serial.println(json_info);
    return;

  } else {

    if (config["alias"]) {
      thinx_alias = strdup(config["alias"]);
    }

    if (config["udid"]) {
      const char *udid = config["udid"];
      Serial.println(F("*TH: Loading UDID..."));
      if ( strlen(udid) > 2 ) {
        thinx_udid = strdup(udid);
      } else {
        thinx_udid = strdup(THINX_UDID);
      }
    } else {
      thinx_udid = strdup(THINX_UDID);
    }

    if (config["apikey"]) {
      Serial.println(F("*TH: Loading Keys..."));
      thinx_api_key = strdup(config["apikey"]);
    }

    if (config["owner"]) {
      thinx_owner = strdup(config["owner"]);
      Serial.println(F("*TH: Loading Owner..."));
    }

    if (config["ott"]) {
      available_update_url = strdup(config["ott"]);
      Serial.println(F("*TH: Loading OTT..."));
    }

    #ifdef __USE_SPIFFS__
    Serial.println(F("*TH: Closing SPIFFS file."));
    f.close();
    #else
    #endif
  }
}

/*
* Stores mutable device data (alias, owner) retrieved from API
*/

void THiNX::save_device_info()
{
  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& root = jsonBuffer.createObject();

  // Mandatories

  if (strlen(thinx_owner) > 1) {
    root["owner"] = strdup(thinx_owner); // allow owner change
  }

  if (strlen(thinx_api_key) > 1) {
    root["apikey"] = strdup(thinx_api_key); // allow dynamic API Key
  }

  if (strlen(thinx_udid) > 1) {
    root["udid"] = strdup(thinx_udid); // allow setting UDID, skip 0
  }

  // Optionals
  if (strlen(available_update_url) > 1) {
    root["update"] = strdup(available_update_url); // allow update
    Serial.println(F("*TH: available_update_url..."));
  }

  #ifdef __USE_SPIFFS__
  File f = SPIFFS.open("/thinx.cfg", "w");
  if (f) {
    Serial.println(F("*TH: Saving configuration to SPIFFS..."));
    root.printTo(f);
    //root.printTo(Serial);
    f.print('\r');
    f.println();
    f.close();
    Serial.println(F("*TH: Saved configuration to SPIFFS."));
  } else {
    Serial.println(F("*TH: Saving configuration failed!"));
    delay(5000);
  }
  #else
  Serial.println(F("*TH: Saving configuration to EEPROM: "));
  for (long addr = 0; addr < strlen((const char*)json_info); addr++) {
    uint8_t byte = json_info[addr];
    EEPROM.put(addr, json_info[addr]);
    if (byte == 0) break;
  }
  EEPROM.commit();
  Serial.println(F("*TH: Saved configuration (EEPROM)."));
  #endif
}

/*
* Updates
*/

// update_file(name, data)
// update_from_url(name, url)

void THiNX::update_and_reboot(String url) {

  Serial.print("*TH: Update with URL: ");
  Serial.println(url);

  // #define __USE_STREAM_UPDATER__ ; // Warning, this is MQTT-based streamed update!
  #ifdef __USE_STREAM_UPDATER__
  Serial.println(F("*TH: Starting MQTT & reboot..."));
  uint32_t size = pub.payload_len();
  if (ESP.updateSketch(*pub.payload_stream(), size, true, false)) {
    Serial.println(F("Clearing retained message."));
    mqtt_client->publish(MQTT::Publish(pub.topic(), "").set_retain());
    mqtt_client->disconnect();

    Serial.printf("Update Success: %lu\nRebooting...\n", millis() - startTime);

    // Notify on reboot for update
    if (mqtt_client != NULL) {
      mqtt_client->publish(
        mqtt_device_status_channel,
        thx_reboot_response.c_str()
      );
      mqtt_client->disconnect();
    }
  }

  ESP.restart();
  #else

  // TODO: Download the file and check expected_hash first...

  Serial.println(F("*TH: Starting ESP8266 HTTP Update & reboot..."));
  t_httpUpdate_return ret = ESPhttpUpdate.update(url.c_str(), expected_md5);

  switch(ret) {
    case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    setDashboardStatus(ESPhttpUpdate.getLastErrorString());
    break;

    case HTTP_UPDATE_NO_UPDATES:
    Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
    break;

    case HTTP_UPDATE_OK:
    Serial.println(F("HTTP_UPDATE_OK"));
    ESP.restart();
    break;
  }
  #endif
}

/*
* Imports all required build-time values from thinx.h
*/

void THiNX::import_build_time_constants() {

  // Only if not overridden by user
  if (strlen(thinx_api_key) < 4) {
    thinx_api_key = strdup(THINX_API_KEY);
  }

  if (strlen(THINX_UDID) > 2) {
    thinx_udid = strdup(THINX_UDID);
  } else {
    thinx_udid = strdup("");
  }

  // Use commit-id from thinx.h if not given by environment
  #ifdef THX_COMMIT_ID
  thinx_commit_id = strdup(thx_commit_id);
  #else
  thinx_commit_id = strdup(THINX_COMMIT_ID);
  #endif

  thinx_mqtt_url = strdup(THINX_MQTT_URL);
  thinx_cloud_url = strdup(THINX_CLOUD_URL);
  thinx_alias = strdup(THINX_ALIAS);
  thinx_owner = strdup(THINX_OWNER);
  thinx_mqtt_port = THINX_MQTT_PORT;
  thinx_api_port = THINX_API_PORT;
  thinx_auto_update = THINX_AUTO_UPDATE;
  thinx_forced_update = THINX_FORCED_UPDATE;
  thinx_firmware_version = strdup(THINX_FIRMWARE_VERSION);
  thinx_firmware_version_short = strdup(THINX_FIRMWARE_VERSION_SHORT);
  app_version = strdup(THINX_APP_VERSION);
}

/*
* Performs the SPIFFS check and format if needed.
*/

bool THiNX::fsck() {
#if defined(ESP8266)
  String realSize = String(ESP.getFlashChipRealSize());
  String ideSize = String(ESP.getFlashChipSize());
  bool flashCorrectlyConfigured = realSize.equals(ideSize);
#else
  bool flashCorrectlyConfigured = true; //?
#endif
  bool fileSystemReady = false;
  if(flashCorrectlyConfigured) {
    #if defined(ESP8266)
      fileSystemReady = SPIFFS.begin();
    #else
      fileSystemReady = SPIFFS.begin(true); // formatOnFail=true
    #endif
    if (!fileSystemReady) {
      Serial.println(F("*TH: Formatting SPIFFS..."));
      fileSystemReady = SPIFFS.format();;
      Serial.println(F("*TH: Format complete, rebooting...")); Serial.flush();
      ESP.restart();
      return false;
    }
  }  else {
    Serial.print(F("*TH: Flash incorrectly configured, SPIFFS cannot start."));
#if defined(ESP8266)
    Serial.println(ideSize + ", real size: " + realSize);
#endif
  }
  return fileSystemReady ? true : false;
}

#ifdef __USE_WIFI_MANAGER__
/*
* API key update event
*/

void THiNX::evt_save_api_key() {
  if (should_save_config) {
    if (strlen(thx_api_key) > 4) {
      thinx_api_key = thx_api_key;
      Serial.print(F("Saving thx_api_key from Captive Portal."));
    }
    if (strlen(thx_owner_key) > 4) {
      thinx_owner_key = thx_owner_key;
      Serial.print(F("Saving thx_owner_key from Captive Portal."));
    }
    Serial.println(F("Saving device info for API key.")); Serial.flush();
    save_device_info();
    should_save_config = false;
  }
}
#endif

/*
* Final callback setter
*/

void THiNX::setPushConfigCallback( void (*func)(String) ) {
  _config_callback = func;
}

void THiNX::setFinalizeCallback( void (*func)(void) ) {
  _finalize_callback = func;
}

void THiNX::setMQTTCallback( void (*func)(String) ) {
    _mqtt_callback = func;
}

void THiNX::setMQTTBroker(char * url, int port) {
    thinx_mqtt_url = url;
    Serial.println("Port is ignored, defaults to 1883");
}

void THiNX::finalize() {
  thinx_phase = COMPLETED;
  if (_finalize_callback) {
    _finalize_callback();
  } else {
    Serial.println(F("*TH: Checkin completed (no _finalize_callback)."));
  }
}

/* This is necessary for SSL/TLS and should replace THiNX timestamp */
void THiNX::sync_sntp() {
  // THiNX API returns timezone_offset in current DST, if applicable
  configTime(timezone_offset * 3600, 0, "0.europe.pool.ntp.org", "cz.pool.ntp.org");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print(F("*TH: SNTP time: "));
  Serial.print(asctime(&timeinfo));
}

/*
* Core loop
*/

void THiNX::loop() {

  //printStackHeap("in");

  if (thinx_phase == CONNECT_WIFI) {
    // If not connected manually or using WiFiManager, start connection in progress...
    if (WiFi.status() != WL_CONNECTED) {
      wifi_connected = false;
      if (wifi_connection_in_progress != true) {
        Serial.println(F("*TH: CONNECTING »"));
        connect(); // blocking
        wifi_connection_in_progress = true;
        wifi_connection_in_progress = true;
        return;
      } else {
        return;
      }
    } else {
      wifi_connected = true;

      // Synchronize SNTP time
      Serial.println(""); // newline after "Time difference for DST"
      sync_sntp();

      // Start MDNS broadcast
#ifdef __DISABLE_PROXY__
#else
      if (!MDNS.begin(thinx_alias)) {
        Serial.println(F("*TH: Error setting up mDNS"));
      } else {
        // Query MDNS proxy
        Serial.println(F("*TH: Searching for thinx-connect on local network..."));
        int n = MDNS.queryService("thinx", "tcp"); // TODO: WARNING! may be _tcp!
        if (n > 0) {
          thinx_cloud_url = strdup(String(MDNS.hostname(0)).c_str());
          thinx_mqtt_url = strdup(String(MDNS.hostname(0)).c_str());
        }
      }
#endif

      thinx_phase = CONNECT_API;
      return;
    }
  }

  // After MQTT gets connected:
  if (thinx_phase == CHECKIN_MQTT) {
    thinx_mqtt_channel(); // initialize channel variable
    if (strlen(mqtt_device_channel) > 5) {
      if (mqtt_client->subscribe(mqtt_device_channel)) {
        Serial.println(F("*TH: MQTT connected."));
        // Publish status on status topic
        mqtt_client->publish(
          mqtt_device_status_channel,
          F("{ \"status\" : \"connected\" }")
        );
        mqtt_client->loop();
        thinx_phase = FINALIZE;
        return;
      }
    }
  }

  if ( thinx_phase == CONNECT_MQTT ) {
    if (strlen(thinx_udid) > 4) {
      if (mqtt_connected == false) {
        mqtt_connected = start_mqtt();
        mqtt_client->loop();
        if (mqtt_connected) {
            thinx_phase = CHECKIN_MQTT;
        } else {
          // tries again next time
        }
        return;
      } else {
        thinx_phase = FINALIZE;
        return;
      }
    } else {
      Serial.println(F("*TH: LOOP » FINALIZE"));
      thinx_phase = FINALIZE;
      return;
    }
  }

  /*
  if (thinx_phase > CHECKIN_MQTT) {
    Serial.println(F("*TH: start_mqtt")); Serial.flush();
    mqtt_connected = mqtt_client->connected();
    if (!mqtt_connected) {
      Serial.println(F("*TH: MQTT RECONNECT ON CONNECTION CHECK..."));
      Serial.println(thinx_time(NULL));
      mqtt_connected = start_mqtt();
      if (mqtt_connected) {
          thinx_phase = CHECKIN_MQTT;
      } else {
        // tries again next time
      }
    }
  }*/

  // CASE thinx_phase == CONNECT_API

  // Force re-checkin after specified interval
  if (thinx_phase > FINALIZE) {
    if (millis() > checkin_time) {
      if (checkin_interval > 0) {
        Serial.println(F("*TH: LOOP » Checkin interval arrived..."));
        thinx_phase = CONNECT_API;
        checkin_interval = millis() + checkin_time;
      }
    }
  }

  // If connected, perform the MQTT loop and bail out ASAP
  if (thinx_phase == CONNECT_API) {
    if (WiFi.getMode() == WIFI_AP) {
      Serial.println(F("*TH: LOOP « (AP_MODE)"));
      return;
    }
    if (strlen(thinx_api_key) > 4) {
      checkin(); // warning, this blocking and takes time, thus return...
      if (mqtt_connected == false) {
        thinx_phase = CONNECT_MQTT;
      } else {
        Serial.println(F("*TH: LOOP » FINALIZE (mqtt connected)"));
        thinx_phase = FINALIZE;
      }
    }
  }

  if ( thinx_phase == FINALIZE ) {
    finalize();
  }

  if ( thinx_phase >= FINALIZE ) {
    if (mqtt_client) {
      mqtt_client->loop();
    }
  }

  if ( (reboot_interval > 0) && (millis() > reboot_interval) ) {
    Serial.println(F("Rebooting...")); Serial.flush();
    setDashboardStatus(F("Rebooting..."));
    ESP.restart();
  }

  #ifdef __USE_WIFI_MANAGER__
    // Save API key on change
    if (should_save_config) {
      Serial.println(F("*TH: Saving API key on change..."));
      evt_save_api_key();
      should_save_config = false;
    }
  #endif

  //printStackHeap("out");
}

void THiNX::setLocation(double lat, double lon) {
  latitude = lat;
  longitude = lon;
  // uses mqtt_connected status because this happens only after first checkin
  // and thus prevents premature request to backend.
  if (wifi_connected && thinx_phase > FINALIZE) {
    Serial.println(F("*TH: LOOP » setLocation checkin"));
    checkin();
  }
}

void THiNX::setDashboardStatus(String newstatus) {
  statusString = newstatus;
  if (wifi_connected && thinx_phase > FINALIZE) {
    Serial.println(F("*TH: LOOP » setDashboardStatus checkin"));
    checkin();
    if (mqtt_client) {
      String message = String("{ \"status\" : \"") + newstatus + String("\" }");
      mqtt_client->publish(mqtt_device_status_channel, message.c_str());
    }
  }
}

// deprecated since 2.2 (3)
void THiNX::setStatus(String newstatus) {
  setDashboardStatus(newstatus);
}

void THiNX::setCheckinInterval(long interval) {
  checkin_interval = interval;
}

void THiNX::setRebootInterval(long interval) {
  reboot_interval = interval;
}

// SHA256

/* Calculates SHA-256 of a file */
bool THiNX::check_hash(char * filename, char * expected) {
  File file = SPIFFS.open(filename, "r");
  char aes_text[2 * SHA256_BLOCK_SIZE + 1];

  static uint8_t buf[512] = {0};
  static uint8_t obuf[512] = {0};
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  uint32_t fpos = 0;

  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    Sha256 *sha256Instance = new Sha256();

    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      sha256Instance->update((const unsigned char*)buf, toRead);
      fpos += toRead;
      len -= toRead;
    }

    sha256Instance->final(obuf);
    delete sha256Instance;

    for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
      sprintf(aes_text + 2 * i, "%02X", obuf[i]);
    }

    Serial.printf("AES # %s at %u\n", aes_text, fpos);
    Serial.printf("EXPECTED # %s", expected);

    end = millis() - start;
    Serial.printf("%u bytes hashed in %u ms\n\n", flen, end);

    file.close();

    return strcmp(expected, aes_text);

  } else {
    Serial.println(F("Failed to open file for reading"));
    return false;
  }
}

#ifdef __DEBUG__
/* Convenience method for debugging memory issues. */
void THiNX::printStackHeap(String tag) {
  extern cont_t g_cont;
  register uint32_t *sp asm("a1");
  unsigned long heap = system_get_free_heap_size();
  Serial.printf("[%s] STACK U=%4d ", tag.c_str(), cont_get_free_stack(&g_cont));
  Serial.printf("F=%4d ", 4 * (sp - g_cont.stack));
  Serial.print("H="); Serial.println(heap);
}
#endif

#endif // IMPORTANT LINE FOR UNIT-TESTING!
