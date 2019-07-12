/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
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

const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"ru\"><head><meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>body{background-color:#fde3e8;}.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:2px solid #fff;border-radius:0.3rem;background-color:#f26e8a;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Настройка WiFi</button></form><br/><form action=\"/i\" method=\"get\"><button>Информация</button></form><br/><form action=\"/r\" method=\"post\"><button>Сброс</button></form><br/><br/><center><img src=\" data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAARgAAAC6CAMAAABC86LvAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAyZpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADw/eHBhY2tldCBiZWdpbj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlkIj8+IDx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IkFkb2JlIFhNUCBDb3JlIDUuNi1jMTM4IDc5LjE1OTgyNCwgMjAxNi8wOS8xNC0wMTowOTowMSAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIiB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyIgeG1sbnM6c3RSZWY9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZVJlZiMiIHhtcDpDcmVhdG9yVG9vbD0iQWRvYmUgUGhvdG9zaG9wIENDIDIwMTcgKFdpbmRvd3MpIiB4bXBNTTpJbnN0YW5jZUlEPSJ4bXAuaWlkOkJEOThDMjc5QTRCMTExRTk4N0JBRDcwNzM5RjQ3Q0RBIiB4bXBNTTpEb2N1bWVudElEPSJ4bXAuZGlkOkJEOThDMjdBQTRCMTExRTk4N0JBRDcwNzM5RjQ3Q0RBIj4gPHhtcE1NOkRlcml2ZWRGcm9tIHN0UmVmOmluc3RhbmNlSUQ9InhtcC5paWQ6QkQ5OEMyNzdBNEIxMTFFOTg3QkFENzA3MzlGNDdDREEiIHN0UmVmOmRvY3VtZW50SUQ9InhtcC5kaWQ6QkQ5OEMyNzhBNEIxMTFFOTg3QkFENzA3MzlGNDdDREEiLz4gPC9yZGY6RGVzY3JpcHRpb24+IDwvcmRmOlJERj4gPC94OnhtcG1ldGE+IDw/eHBhY2tldCBlbmQ9InIiPz4friApAAAAwFBMVEXs8Ppvcm5hiS9LcRWJcWSzlofK0dTQtKWyqJHKq5tQTEWRhnAfHx0sSVXZxq50aFqpinp7g3+cpKZANiyTmZqkqaqDiYluVEmMlJc5UBW0ubuemYestLZeamvT2uG+xsm5yK6eqYvg5u4bNkKWnqGLqG5ze3qZiYKmrbG7paO2vcPk0MHAno9aXlaNkoOGjpFFWWC/tpyOfHWpuZqnnZy2rrLOvLzs5d1nYF6Zj4/Z4N94mlNtNSbcvK3///////88nDVlAAAAQHRSTlP///////////////////////////////////////////////////////////////////////////////////8AwnuxRAAAP4BJREFUeNrsnQlD4kgXrkk3ayUxZCVNCIgwAZEsRFSw1fn//+qe91QlBLW3mV7m3vuFbntTOzy8Z61TRevvf3G9fOH6+/+Bq/UzgfwJQL/qHlq/DstvQPML76L1L2+k9di926/Kst0e7nv3N4+t30Tn5cevXwfmDMj+4NIl8BBiNBpZ7Xa5Gg6HHb72vdavRPPqKT9M+9FK1/WyLJOyTR/p93rUv3n4F2ha/+RmHu6Ee7oIzUgwmBMX+nl98fCL0JxDue9HuADGJuEmeOh8ZVkUdR/+IZrWD97Lwx2hMNzziyXDiul0tvvt9Xa7v95e4+pd/GQ4TdHu+v19vz/vM5kMZPSkrdGDfpQSjSLUb/3wXbR+6HZafVCRjyYXInNgxRAYhQTXgK7e888k01BKv9MBF8BhxWQwJSiG0dhtvYGmLLPWD95F60duZsZiMd6wqcCsjmRBDS5MZvDTVNM0ILLVqN/pq6uypbJkueCRSM3wZeswrx9TTeu7b8f6xEZkvNaL4kJgVquai2Iy6NED1/PDvyZzMqGtdGQnLorMSi8T4mJZFosmOZGB39GSJNt//120vvNuLtNP7icDjwrN4uR76SIwZbnq7LeSyvXgbnDdG/RqMoPBv1PN6asH2zdY+v2OMiYdarGYTQNNorOFJYmdzb73LlrfdTsfP9HlMhX+WF+mYC7p4WCRJREY9rh3PXWdkdne/HMylVh6g2119c8vaUzkZZgL0LAX1ttkRsq8tDKx7cBrfddttL7jdrofPqkLkoEdLaRUTKGu9ECvEIE5diAY4iJ/EJQTF3pK1/+MTP0iP/fk91FkBvTo75qagWTabcZC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\"></center>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='Пароль'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>Сохранить</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Сканировать</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Данные сохранены<br />Попытка подключения ESP к сети.<br />При неудаче переподключитесь и попробуйте снова</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter {
  public:
    /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~WiFiManagerParameter();

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
    ~WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
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
    //adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
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
    boolean       configPortalHasTimeout();

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

    int                    _max_params;
    WiFiManagerParameter** _params;

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
