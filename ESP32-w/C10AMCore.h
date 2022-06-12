#ifndef t1
#define t1

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

struct CS_MODEM_RES {
	unsigned char status;
	String data;
	String temp;
};

struct radio {
	String csq;
	String rssi;
	String ber;
};

struct ping {
	bool status;
	String addr;
	String stats;
};

class Network {
  public:
  // Public API Functions starts here.
    void SerialInit(); // Modem Serial Comm Initiate. Mandatory function to be called before all other API functions.
    String readstringin();
  // Network Function Method
    bool radioEnable (bool enable); // True - Enable Cellular Radio, False - Disable Cellular Radio. 
    bool networkAttach (bool enable); // True - Attach to Cellular Network, False - Dettach from Cellular Service.
    bool setNetworkAuto (); 
    bool setSIMMode (String simmode);// SIM Mode 1 - External ESIM, 0 - eSIM
    bool setNetworkManual (String networkcode);
    bool setPDN(String ipType, String apn); // IP Type: 1 - IPV4, 2 - IPV6, 3 - IPV4V6, 5 - No IP (NB-IoT Paging).
    bool enablePacketData (bool enable); // True - Attach to Data Service, False - Dettach from Data Service.
    bool reboot(); // Reboots the Modem. This is a cold reboot.
    bool shutdown(); // Modem will be powered down. Toggle RESET PIN externally to Switch ON.
    void modemReset(); // Modem will go for Hard Reset. The same function can be used to Switch ON Modem after a Shutdown or as warm reboot.
    bool modemWakeUp(); // Wake Up the modem from Sleep Mode.

  // Get Info/Status methods.
    bool isModemAvailable(); // Returns true if a valid modem response is received.
    bool isESIMReady(); // Returns true if the eSIM is ready for registration.
    bool isSIMCheck(); // Returns true if the eSIM is ready for registration.
    bool isNetworkAttached(); // Returns true if the modem is registered to the network.
    radio getRadioQuality(); // Displays the Radio Signal Strength.
    String getModemInfo(); // Displays details Modem Information.
    String getIMEI(); // Displays IMEI Number.
    String getICCID(); // Displays ICCID of the active SIM Card.
    String getIMSI(); // Displays IMSI of the active SIM Card.
    String getNetTime(); // Dsipalys the time received from the network.
    String getDefaultPDN(); // Dsiplays default PDN configured.
    bool getPacketDataStatus(); // Displays Packet Data status.
    String getIPAddr(); // Get the current modem IP address.
    ping getPingStatus(String hostname); // Ping to a hostname and get the ping quality report.
    String getSMSCenterNumber(); // Displays the SMS Center Number from the SIM Card.
    String getNetworkOperator(); // Displays the currently attached Service Provider Name.
    
  // SMS methods
    String readSMS(String index, String storageType); // Prints the SMS received and stored in a specific strorage (SIM or Modem Flash) with the index number.
    bool sendSMS(String destNumber, String message); // Sends SMS to the given Destination Number.

  // MQTT methods
    bool createMQTT(String mqttserver, String port, String clientID, String keepalive, String cleansession, String username, String password); // Create a new MQTT connection to the specified hostname and credintials.
    bool publishMQTT(String topic, String message, String qos, String duplicate, String retain); // Publish MQTT Message to a specified topic unding the previously created MQTT connection.
    bool subscribeMQTT(String topic, String qos, bool enable); // Subcribe to a MQTT Topic. If enable is false, the topic will be unsubscribed from the connection.
    bool publishMQTTLM(String topic, String message, String qos, String duplicate, String retain);
    bool publishMQTTLT(String topic, String message, String qos, String duplicate, String retain);
    String callbackMQTT();
    bool disconnectMQTT(); // Disconnect from the created MQTT server.

  
  private:
    CS_MODEM_RES serial_res(long timeout, String chk_string);
    CS_MODEM_RES serial_readin();
};

#endif
