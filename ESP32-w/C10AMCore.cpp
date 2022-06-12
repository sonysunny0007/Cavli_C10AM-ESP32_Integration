#include "C10AMCore.h"

//HardwareSerial Serial2(2);
String comma = ",";
String quotes = "\"";
String MQTT_client_id;

void Network::SerialInit()
{ // Modem Serial Comm Initiate. Mandatory function to be called before all other API functions.
	Serial2.begin(115200, SERIAL_8N1, 27, 26); //27,26 RX, TX
}

bool Network::radioEnable(bool enable)
{ // True - Enable Cellular Radio, False - Disable Cellular Radio.
	if (isModemAvailable())
	{
		Serial2.print("AT+CFUN=");
		if (enable)
		{
			Serial2.println("1");
		}
		else
		{
			Serial2.println("0");
		}
		CS_MODEM_RES res = serial_res(500, F("OK"));
		return (res.status);
	}
	else
	{
		Serial.println("Modem is Unavailable or Busy");
		return (false);
	}
}

bool Network::networkAttach(bool enable)
{ // True - Attach to Cellular Network, False - Dettach from Cellular Service.
	if (isModemAvailable())
	{
		Serial2.print("AT+CREG=");
		if (enable)
		{
			Serial2.println("1");
		}
		else
		{
			Serial2.println("0");
		}
		CS_MODEM_RES res = serial_res(500, F("OK"));
		return (res.status);
	}
	else
	{
		Serial.println("Modem is Unavailable or Busy");
		return (false);
	}
}


bool Network::setPDN(String ipType, String apn)
{ // IP Type: 1 - IPV4, 2 - IPV6, 3 - IPV4V6, 5 - No IP (NB-IoT Paging).
	Serial2.print("AT+CGDCONT=1,");
	Serial2.println(ipType + comma + apn);
	CS_MODEM_RES res = serial_res(500, F("OK"));
	return (res.status);
}

bool Network::enablePacketData(bool enable)
{ // True - Attach to Data Service, False - Dettach from Data Service.
	if (!getPacketDataStatus())
	{
		if (isModemAvailable())
		{
			if (isNetworkAttached())
			{
				if (getDefaultPDN())
				{
					Serial2.print("AT+CGACT=");
					if (enable)
					{
						Serial2.println("1,1");
					}
					else
					{
						Serial2.println("0");
					}
					while (1)
					{
						CS_MODEM_RES res = serial_res(500, F("OK"));
            Serial.println(res.data);
						return (res.status);
						res = serial_res(500, F("OK"));
						break;
					}
				}
				else
				{
					Serial.println("No default PDN found");
					return (false);
				}
			}
			else
			{
				Serial.println("No Network service available");
				return (false);
			}
		}
		else
		{
			Serial.println("Modem is Unavailable or Busy");
			return (false);
		}
	}
}



bool Network::reboot()
{ // Reboots the Modem. This is cold reboot.
	Serial2.println("AT+TRB");
	CS_MODEM_RES res = serial_res(500, F("REBOOTING"));
	return (res.status);
}

bool Network::shutdown()
{ // Modem will be powered down. Toggle RESET PIN externally to Switch ON.
	Serial2.println("AT+CPOF");
	CS_MODEM_RES res = serial_res(500, F("OFF"));
	return (res.status);
}

void Network::modemReset()
{ // Modem will go for Hard Reset. The same function can be used to Switch ON Modem after a Shutdown or as warm reboot.
	int resetPin = 13;
	pinMode(resetPin, OUTPUT);
    
	digitalWrite(resetPin, LOW);
  delay(5000);
  digitalWrite(resetPin, HIGH);

	Serial.println("Modem Reset Complete.");
}

bool Network::modemWakeUp()
{ // Wake Up the modem from Sleep Mode.
	int wakePin = 12;
	pinMode(wakePin, OUTPUT);

	digitalWrite(wakePin, LOW);
	delay(1000);
  digitalWrite(wakePin, HIGH);

	Serial.println("Modem Wake up Initiated.");
}

// Get Functions.

bool Network::isModemAvailable()
{ // Returns true if a valid modem response is received.
	String reponse, tmp;
  bool resp;
	Serial2.println("AT");
	CS_MODEM_RES res = serial_res(500, F("OK"));
//  Serial.println(res.data);
	return (res.status);
}

 bool Network::isSIMCheck()
 {
  String response, tmp;
  Serial2.println("AT^SIMSWAP?");
  CS_MODEM_RES res;
  res = serial_res(500, F("^SIMSWAP"));
  response = res.temp;
  int info_first = response.indexOf(F(":"));
  int info_mid = response.indexOf(F("O"));
  tmp = response.substring(info_first+2, info_mid);
  Serial.print("SIM Status: ");
  Serial.println(tmp);
  if (tmp == "1")
  {
  Serial.println("External SIM Ready");
  }
  if(tmp == "0")
  {
  Serial.println("ESIM Ready");
  }
  return(res.status);
 }


bool Network::isESIMReady()
{ // Returns true if the eSIM is ready for registration.
	Serial2.print("AT+SIM=");
	Serial2.println("0");
	CS_MODEM_RES res = serial_res(500, F("+USIM"));
	String data;
	String tmp;
	bool simStat;
	if (res.status)
	{
		data = res.data;
		int esim_first = data.indexOf(F(":"));
		tmp = data.substring(esim_first + 2, esim_first + 8);
		if (tmp == "NORMAL")
		{
			simStat = true;
		}
		else
		{
			simStat = false;
		}
	}
	else
	{
		simStat = false;
	}
	return (simStat);
}

bool Network::setNetworkAuto()
{
	Serial2.print("AT+COPS=");
	Serial2.println("0");
	delay(1000);
	CS_MODEM_RES res = serial_res(500, F("OK"));
	return (res.status);
}

bool Network::setSIMMode(String simmode)
{
  Serial2.print("AT^SIMSWAP=");
  Serial2.println(simmode);
  CS_MODEM_RES res = serial_res(500, F("^SIMSWAP"));
  return (res.status);
}


bool Network::setNetworkManual(String networkcode)
{
	CS_MODEM_RES res = serial_res(500, F("OK"));
	String out;
	int wait = 0;
	Serial2.println("AT+COPS=1,2," + quotes + networkcode + quotes + comma + "7");
	delay(5000);
	while (1)
	{
		res = serial_readin();
		String out = res.data;
		int start = out.indexOf("O");
		out = out.substring(start, start + 2);
		delay(1000);
		wait++;
		if (out == "OK")
		{
			break;
		}
		else
		{ 
      if (wait == 1)
      Serial.print("Switching to preffered network");
      if (wait != 1)
      Serial.print(".");
			if (wait == 30)
				break;
		}
	}
	while (1)
	{
		Serial2.println("AT+COPS?");
		res = serial_res(500, F("+COPS:"));
		out = res.data;
		int type_first = out.indexOf(F("\""));
		int type_second = out.indexOf(F("\""), type_first + 1);
		out = out.substring(type_first + 1, type_second);
		if (out == networkcode)
		{
			Serial.print("Switched network to:");
			Serial.println(out);
			res.status = true;
			break;
		}
		else
		{
			Serial.println("Manual network switching failed");
			res.status = false;
			break;
		}
	}
	return (res.status);
}

bool Network::isNetworkAttached()
{ // Returns true if the modem is registered to the network.
	Serial2.println("AT+CREG?");
	CS_MODEM_RES res = serial_res(1000, F("+CREG"));
//  Serial.println(res.data);
	String tmp;
	bool netStat;
	netStat = false;
	String data;
	if (res.status)
	{
		data = res.data;
		if (res.data.indexOf(F("+CREG")) != -1)
		{
			int first = res.data.indexOf(F(":"));
			int last = res.data.indexOf(F(","));
			tmp = res.data.substring(first + 2, last+3 );
		}
//		if (tmp == F("3,1,") || tmp == F("3,5,"))
    if (tmp == F("3,1,")) 
		{
      Serial.println("Modem registered to HOME network");
			netStat = true;
		}
    else if (tmp == F("3,5,"))
      {
        Serial.println("Modem registered to ROAMING network");
        netStat = true;
      }
		else
		{
			netStat = false;
		}
	}
//  Serial.print("Network Status: ");
//  Serial.print(tmp);
//  Serial.println(netStat);
	res = serial_res(500, F("OK"));
	return (netStat);

}

String Network::getModemInfo()
{ // Displays the Modem Manufacturer and Firmware information.
	CS_MODEM_RES res = serial_res(500, F("OK"));
	Serial2.println(F("ATI"));
	res = serial_res(500, F("OK"));
	String out = res.temp;
	int info_first = out.indexOf(F("Manufacturer:"));
	int info_last = out.indexOf(F("Build Date:"));
	out = out.substring(info_first, info_last + 20);
	res = serial_res(500, F("OK"));
	return (out);
}

String Network::getIMEI()
{ // Displays IMEI Number.
	Serial2.println("AT+CGSN");
	CS_MODEM_RES res = serial_res(1000, F("OK"));
	String out = res.temp;
	out = out.substring(1, 16);
	res = serial_res(500, F("OK"));
	return (out);
}

String Network::getICCID()
{ // Displays ICCID of the active SIM Card.
	Serial2.println("AT+ICCID");
	CS_MODEM_RES res = serial_res(1000, F("+ICCID"));
	String out = res.temp;
//  Serial.println(out);
	int iccid_start = out.indexOf(F(":")) + 2;
	out = out.substring(iccid_start, iccid_start + 19);
	res = serial_res(500, F("OK"));
	return (out);
}

String Network::getIMSI()
{ // Displays IMSI of the active SIM Card.
	Serial2.println("AT+CIMI");
	CS_MODEM_RES res = serial_res(500, F("OK"));
	String out = res.temp;
	out = out.substring(1, 16);
	res = serial_res(500, F("OK"));
	return (out);
}

String Network::getNetTime()
{   // Dsipalys the time received from the network.
	// To be reviewed.
}

radio Network::getRadioQuality()
{
	Serial2.println(F("AT+CSQ"));
	CS_MODEM_RES res = serial_res(800, F("+CSQ"));
  delay(200);
	radio sig;
	int x = 0;
	String tmp;
	if (res.status)
	{
		if (res.data.indexOf(F("+CSQ")) != -1)
		{
			int index = res.data.indexOf(F(":"));
			int index2 = res.data.indexOf(F(","));
			tmp = res.data.substring(index + 2, index2);
			if (tmp == F("99"))
			{
				sig.csq = F("N/A");
				sig.rssi = F("N/A");
			}
			else
			{
				sig.csq = tmp;
				x = tmp.toInt();
				x = (2 * x) - 113;
				sig.rssi = String(x);
			}
			sig.ber = res.data.substring(index2 + 1);
		}
	}
	res = serial_res(500, F("OK"));
	return (sig);
}

String Network::getDefaultPDN()
{ // Dsiplays default PDN configured.
  String response, tmp;
	Serial2.println(F("AT+CGDCONT?"));
	CS_MODEM_RES res = serial_res(500, F("+CGDCONT"));
//	Serial.println(res.data);
  response = res.data;
  int info_first = response.indexOf(F("1"));
  int info_mid = response.indexOf(F("0"));
  tmp = response.substring(info_first+8, info_mid-3);
//  Serial.println(tmp);
	return (tmp);
}

bool Network::getPacketDataStatus()
{ // Displays Packet Data status.
	Serial2.println("AT+CGACT?");
	CS_MODEM_RES res = serial_res(500, F("+CGACT"));
	String tmp;
	bool netStat;
	netStat = false;
	String data;
	if (res.status)
	{
		data = res.data;
//    Serial.println(data);
		if (res.data.indexOf(F("+CGACT")) != -1)
		{
			int first = res.data.indexOf(F(":"));
			int last = res.data.indexOf(F(","));
			tmp = res.data.substring(first + 2, last + 2);
		}
		if (tmp == F("1,1"))
		{
			netStat = true;
		}
		else
		{
			netStat = false;
		}
	}
	res = serial_res(500, F("OK"));
	return (netStat);
}

String Network::getIPAddr()
{ // Get the current modem IP address.
	CS_MODEM_RES res = serial_res(500, F("OK"));
	Serial2.println("AT+CGPADDR");
	res = serial_res(500, F("+CGPADDR"));
	String out = res.temp;
	int addr_start = out.indexOf(F(":")) + 5;
	int addr_end = out.indexOf(F("\","));
	out = out.substring(addr_start, addr_end);
	res = serial_res(500, F("OK"));
	return (out);
}


ping Network::getPingStatus(String hostname)
{ // Ping to a hostname and get the ping quality report.
	ping pingr;
	String data = "";
	Serial2.print(F("AT+PING="));
	Serial2.println(quotes + hostname + quotes);
	CS_MODEM_RES res = serial_res(10000, F("Ping"));
	if (res.status)
	{
		data = res.data;
		int ip_start = data.indexOf(F("for")) + 4;
		int ip_end = data.indexOf(F("\n"), ip_start);
		pingr.addr = data.substring(ip_start, ip_end - 2);
		res = serial_res(500, F("Packets:"));
		data = res.data;
		int stats_start = data.indexOf(F("Packets:")) + 9;
		int stats_end = data.indexOf(F("\n"), stats_start);
		pingr.stats = data.substring(stats_start, stats_end - 2);
		pingr.status = true;
	}
	else
	{
		Serial.println("# Ping Failed");
		pingr.status = false;
	}
	res = serial_res(500, F("+CSCON: 0"));
	return pingr;
}

String Network::getSMSCenterNumber()
{ // Displays the SMS Center Number from the SIM Card.
	Serial2.println("AT+CSCA?");
	CS_MODEM_RES res = serial_res(500, F("+CSCA"));
	String sca = res.data;
	int sca_start = sca.indexOf(F(":")) + 3;
	int sca_end = sca.indexOf(F(",\"")) - 2;
	sca = sca.substring(sca_start, sca_end);
	res = serial_res(500, F("OK"));
	return (sca);
}

String Network::getNetworkOperator()
{ // Displays the currently attached Service Provider Name.
	if (isNetworkAttached())
	{
		Serial2.println("AT+QSPN?");
		CS_MODEM_RES res = serial_res(500, F("+QSPN"));
		String spn = res.data;
    Serial.println(spn);
		int spn_start = spn.indexOf(F(":")) + 5;
		int spn_end = spn.indexOf(F("\n")) - 2;
		spn = spn.substring(spn_start, spn_end);
		res = serial_res(500, F("OK"));
		return (spn);
	}
	else
	{
		return ("No Service");
	}
}

bool Network::sendSMS(String destNumber, String message)
{ // Sends SMS to the given Destination Number.
	Serial2.println("AT+CMGF=1");
	CS_MODEM_RES res = serial_res(500, F("OK"));
	Serial2.print(F("AT+CMGS="));
	Serial2.println(quotes + destNumber + quotes);
	delay(100);
	Serial2.println(message + "\x1a");
	delay(100);
	res = serial_res(5000, F("OK"));
	return res.status;
}

String Network::readSMS(String index, String storageType)
{ // Prints the SMS received and stored in a specific strorage (SIM or Modem Flash) with the index number.
	Serial2.print(F("AT+CPMS="));
	Serial2.println(quotes + storageType + quotes);
	CS_MODEM_RES res = serial_res(10000, F("OK"));
	Serial2.print(F("AT+CMGR="));
	Serial2.println(index);
	res = serial_res(10000, F("OK"));
	String out = res.temp;
	out.replace(F("OK"), "");
	out = out.substring(0, out.length());
	return out;
}

bool Network::createMQTT(String mqttserver, String port, String clientID, String keepalive, String cleansession, String username, String password)
{ // Create a new MQTT connection to the specified hostname and credintials.
  CS_MODEM_RES res;
  bool  value;
  value = disconnectMQTT();
//+MQTTCREATE: <hostname>,<port>,<clientid>,<keepalive>,<cleansession>,[username],[password],[topic_size (1024~20480)],[message_size(1024~20480)],[version(3~4)]<CR><LF>
//AT+MQTTCREATE="broker.mqttdashboard.com",1883,"MQTT_FX_Client",90,0<CR>
//  Serial.println(value);
  Serial.println("Creating MQTT connection");
	Serial2.print(F("AT+MQTTCREATE="));
	Serial2.println(quotes + mqttserver + quotes + comma + port + comma + quotes + clientID + quotes + comma + keepalive + comma + cleansession + comma + quotes + username + quotes + comma + quotes + password + quotes);
	res = serial_res(10000, F("+MQTTCREATE"));
  String out = res.data;
  Serial.print("CREATE STATUS: ");
  Serial.println(out);
  int info_first = out.indexOf(F(":"));
  MQTT_client_id = out.substring(info_first + 2, info_first + 3);
  Serial.println(MQTT_client_id);
  if(port == "1883")
  {
  Serial2.print(F("AT+MQTTCONN="));
  Serial2.println(MQTT_client_id);
  res = serial_res(10000, F("CONNECTION SUCCESS"));
  Serial.print("CONNECTION STATUS: ");
  Serial.println(res.data); 
  }
  else
  {
  Serial2.print(F("AT+MQTTSCONN="));
  Serial2.println(MQTT_client_id);
  res = serial_res(10000, F("CONNECTION SUCCESS"));
  Serial.print("CONNECTION STATUS: ");
  Serial.println(res.data);
  }
	return res.status;
}

bool Network::publishMQTT(String topic, String message, String qos, String duplicate, String retain)
{ // Publish MQTT Message to a specified topic unding the previously created MQTT connection.
	Serial2.print(F("AT+MQTTPUB="));
	Serial2.println(MQTT_client_id + comma + quotes + topic + quotes + comma + quotes + message + quotes + comma + qos + comma + duplicate + comma + retain);
	CS_MODEM_RES res = serial_res(10000, F("OK"));
	return res.status;
}

bool Network::publishMQTTLM(String topic, String message, String qos, String duplicate, String retain)
{ // Publish MQTT Message to a specified topic unding the previously created MQTT connection.
  Serial2.print(F("AT+MQTTPUBLM="));
  Serial2.println(MQTT_client_id + comma + quotes + topic + quotes + comma + qos + comma + duplicate + comma + retain);
  Serial2.print(message + "\x1a");
  CS_MODEM_RES res = serial_res(10000, F("OK"));
  return res.status;
}

bool Network::publishMQTTLT(String topic, String message, String qos, String duplicate, String retain)
{ // Publish MQTT Message to a specified topic unding the previously created MQTT connection.
  Serial2.print(F("AT+MQTTPUBLT="));
  Serial2.println(MQTT_client_id + comma + quotes + message + quotes + comma + qos + comma + duplicate + comma + retain);
  Serial2.print(topic + "\x1a");
  CS_MODEM_RES res = serial_res(10000, F("OK"));
  return res.status;
}




bool Network::subscribeMQTT(String topic, String qos, bool enable)
{ // Subcribe to a MQTT Topic. If enable is false, the topic will be unsubscribed from the connection.
	Serial2.print(F("AT+MQTTSUBUNSUB="));
	if (enable)
	{
		Serial2.println(MQTT_client_id+ comma + quotes + topic + quotes + comma + "1" + comma + qos);
	}
	else
	{
		Serial2.println(MQTT_client_id + quotes + topic + quotes + comma + "0");
	}
	CS_MODEM_RES res = serial_res(10000, F("OK"));
	return res.status;
}

String Network::callbackMQTT()
{
	String mqttdata;
	while (1)
	{
		CS_MODEM_RES res;
		res = serial_res(100, F("+MQTTPUBLISH"));
		String out = res.data;
		int addr_start = out.indexOf(F("+M"));
		int addr_end = out.indexOf(F("H"));
		String temp = out.substring(addr_start, addr_end + 1);
		int addr_start_first = out.indexOf(",");
		int addr_start_second = out.indexOf(",", addr_start_first + 1);
		int addr_start_third = out.indexOf(",", addr_start_second + 1);
		mqttdata = out.substring(addr_start_second + 1, addr_start_third);
		int len = mqttdata.toInt();
		mqttdata = out.substring(addr_start_third + 1, addr_start_third + len + 1);
		if (temp == "+MQTTPUBLISH")
		{
			break;
		}
	}
	return mqttdata;
}

bool Network::disconnectMQTT()
{ // Disconnect from the created MQTT server.
	Serial2.println("AT+MQTTDISCONN=3");
	CS_MODEM_RES res = serial_res(500, F("OK"));
  Serial2.println("AT+MQTTDELETE=3");
  res = serial_res(500, F("OK"));
  Serial2.println("AT+MQTTDISCONN=4");
  res = serial_res(500, F("OK"));
  Serial2.println("AT+MQTTDELETE=4");
  res = serial_res(500, F("OK"));
  Serial.println(res.data);
	return res.status;
}


CS_MODEM_RES Network::serial_readin()
{
	int flag = 0, num = 0;
	;
	CS_MODEM_RES resp_;
	resp_.data = "";
	String input;
	for (;;)
	{
		if (Serial2.available())
		{
			for (num = 0; num < 1; num++)
			{
				input = Serial2.readString();
				resp_.data = input;
			}
			flag = 1;
			break;
		}
		return (resp_);
		if (flag == 1)
			break;
	}
}

CS_MODEM_RES Network::serial_res(long timeout, String chk_string)
{
	unsigned long pv_ok = millis();
	unsigned long current_ok = millis();
	String input;
	unsigned char until = 1;
	unsigned char res = -1;
	CS_MODEM_RES res_;
	res_.temp = "";
	res_.data = "";

	while (until)
	{
		if (Serial2.available())
		{
			input = Serial2.readStringUntil('\n');
			res_.temp += input;
			if (input.indexOf(chk_string) != -1)
			{
				res = 1;
				until = 0;
			}
			else if (input.indexOf(F("ERROR")) != -1)
			{
				res = 0;
				until = 0;
			}
		}
		current_ok = millis();
		if (current_ok - pv_ok >= timeout)
		{
			until = 0;
			res = 0;
			pv_ok = current_ok;
		}
	}
	res_.status = res;
	res_.data = input;
	return (res_);
}
