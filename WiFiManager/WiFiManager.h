/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/esp8266/hardware/esp8266com/esp8266/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' length={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>save</button></form>";
const char HTTP_IMG[] PROGMEM        = "<img src='data:image/png;base64,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' alt="" />";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Credentials Saved<br />Trying to connect ESP to network.<br />If it fails reconnect to AP to try again</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

#define WIFI_MANAGER_MAX_PARAMS 10

class WiFiManagerParameter {
  public:
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //usefully for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, usefull if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter
    void          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsucessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    WiFiManagerParameter* _params[WIFI_MANAGER_MAX_PARAMS];

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
