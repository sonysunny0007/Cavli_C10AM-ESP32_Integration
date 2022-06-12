/*#Connection
 *ESP32 TX(IO26) - C10AM RX 
 *ESP32 RX(IO27) - C10AM TX
 *ESP32 IO13 - RESET
 *ESP32 IO12 - PWRKEY
 *ESP32 GND  - C10AM GND
 */

#include "C10AMCore.h"
Network Network;
bool modemWait = false;
bool networkWait = false;
bool dataWait = false;
bool simWait = false;
int ledPin = 2;
int wait = 0;
void setup()
{
  Serial.begin(115200);
  Serial.println("Testing......!");
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin, LOW);
  Network.SerialInit();
  Network.modemReset();
  Network.modemWakeUp();
  while (!Network.isModemAvailable())
  {
    if (!modemWait)
    {
      Serial.print("Waiting for modem.");
      modemWait = true;
    }
    else
    {
      Serial.print(".");
    }
  }
  
  Serial.println("Modem Ready");
  digitalWrite(ledPin, HIGH);
  Network.setSIMMode("0");
  delay(5000);
  Serial.println(Network.getModemInfo());
  Serial.println(Network.getIMEI());
  Serial.println(Network.getICCID());
  Network.isSIMCheck();
//  Network.setNetworkAuto();

  while (!Network.isNetworkAttached())
  {
    if (!networkWait)
    {
      Serial.print("Waiting for Network.");
      networkWait = true;
    }
    else
    {
      delay(500);
      Serial.print(".");
//      wait++;
//      if(wait == 30)
//      Network.setNetworkManual("40419");//Change to preferred network code
    }
  }
  if (Network.setPDN("IP", "hubblethings.io"))
  {
    Serial.print("Default PDN: ");
    Serial.println(Network.getDefaultPDN());
  }
  while (!Network.enablePacketData(true))
  {
    if (!dataWait)
    {
      Serial.print("Waiting for GPRS.");
      dataWait = true;
    }
    else
    {
      delay(500);
      Serial.print(".");
    }
  }
  
  delay(5000);
  if (Network.getPacketDataStatus())
  {
    Serial.println("Packet Data Attached");
    Serial.println(Network.getIPAddr());
//    Serial.println("Ping IP:"+Network.getPingStatus("google.com").addr+Network.getPingStatus("google.com").stats);
 }
  Serial.println("RF Strength=" + Network.getRadioQuality().csq + ", RSSI=" + Network.getRadioQuality().rssi + ", BER=" + Network.getRadioQuality().ber);

//  //=============================================================== SMS Functions ==================================================================================================
//        String readsms = Network.readSMS("5","ME");
//        Serial.println(readsms);
//        if((Network.sendSMS("9745899513","Its me cavli Wireless"))==1)
//        Serial.println("SMS Send Successfull");

//  //=============================================================== MQTT Functions ==================================================================================================
//
  if ((Network.createMQTT("broker.mqttdashboard.com", "1883", "1211", "90", "0", "test_123", "123__4")) == 1)

    Serial.println("MQTT Connection Successful!!");
  else
    Serial.println("MQTT Connection Unsuccessful!!");
//  if ((Network.publishMQTT("testvia", "Cavli-R&D-KOCHI", "0", "0", "0")) == 1)
//    Serial.println("MQTT Message Published Successfully");
//  else
//    Serial.println("MQTT Message Published Unsuccessful");
//  if(Network.publishMQTTLM("testvia", "{name: \"John\", age: 31, city: \"New York\"}", "0", "0", "0")==1)
//      Serial.println("MQTT large message and topic published successfully");
//  else
//    Serial.println("MQTT arge message and topic published successfully");
//  if(Network.publishMQTTLT("test", "Testing from CAVLI", "1", "0", "0")==1)
//        Serial.println("MQTT large topic and message published successfully");
//  else
//    Serial.println("MQTT large topic and message published unsuccessful");
  
//  
//  // while (!(Network.subscribeMQTT("Hubble", "0", true)))
//  // {
//  // }
//  // Serial.println("MQTT Subscription Successfull");
//  // while (1)
//  // {
//  //   Serial.println(Network.callbackMQTT());
//  // }
//  //  while (!(Network.disconnectMQTT())) {
//  //  } Serial.println("MQTT Connection Disconnected Successfully");
//

}

void loop()
{
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(700);
}
