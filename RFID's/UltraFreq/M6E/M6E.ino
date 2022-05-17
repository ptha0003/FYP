#include <SoftwareSerial.h> //Used for transmitting to the device
#include "SparkFun_UHF_RFID_Reader.h" //Library for controlling the M6E Nano module
#include <SPI.h>
#include <RH_RF69.h>


#define RF69_FREQ 434.0
#define RFM69_INT     21  // 
#define RFM69_CS      22  //
#define RFM69_RST     24  // "A"
#define LED           13
#define RoomName "Room1"

SoftwareSerial softSerial(12, 13); //RX, TX
RFID nano; //Create instance

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  
unsigned long StartClearTimer;
unsigned long current;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 TX LoRaTxMSG!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  while (!Serial); //Wait for the serial port to come online

  if (setupNano(38400) == false) //Configure nano to run at 38400bps
  {
    Serial.println(F("Module failed to respond. Please check wiring."));
    //while (1); //Freeze!
  }

  nano.setRegion(REGION_NORTHAMERICA); //Set to North America

  nano.setReadPower(2000); //5.00 dBm. Higher values may caues USB port to brown out
  //Max Read TX Power is 27.00 dBm and may cause temperature-limit throttling

  nano.startReading(); //Begin scanning for tags
  StartClearTimer = millis();
}

String prevID = "";
void loop() {
  // put your main code here, to run repeatedly:

  String LoRaTxMSG = RoomName;
  LoRaTxMSG.concat("\\ID:");
  String ID = "";

  if(nano.check() == true)
  {
    byte responseType = nano.parseResponse(); 

    if(responseType == RESPONSE_IS_KEEPALIVE) 
    {//Serial.println("Scanning");
    }
    else if (responseType == RESPONSE_IS_TAGFOUND)
    {
       byte tagEPCBytes = nano.getTagEPCBytes(); //Get the number of bytes of EPC from response
       //When tag found concat the RFID ID into string before sending it off with LoRa
       for(int i =7; i < nano.getTagEPCBytes(); i++)
       {
        if(nano.msg[31+i] < 0x10) ID.concat("0");
        ID.concat(String(nano.msg[31 + i], HEX)); //stored EPC into one single text string for LoRa
       }
       LoRaTxMSG.concat(ID);
       int rssi = nano.getTagRSSI();
       LoRaTxMSG.concat("/");
       LoRaTxMSG.concat(rssi);

       if(!prevID.equals(ID)) //capature one UID/EPC at a time to avoid conflict at gateway
       {
          //transmit via LoRa
          char radioPacket[LoRaTxMSG.length() + 1];
          LoRaTxMSG.toCharArray(radioPacket,LoRaTxMSG.length()+1);
          itoa(0, radioPacket+LoRaTxMSG.length()+1, LoRaTxMSG.length()+1);
          Serial.println(radioPacket);
          rf69.send((uint8_t *)radioPacket, strlen(radioPacket));
          rf69.waitPacketSent(); 
       }
       prevID = ID;

       
    }
  }

  current = millis();
  if(current - StartClearTimer >= 8000)
  { //clear prevID after 8s so that user can be detected again
    prevID = "";
    StartClearTimer = current;
    
  }
  
}



boolean setupNano(long baudRate) //part of M6E nano library code (needed to set up the M6E nano reader) source below
{
  nano.begin(softSerial); //Tell the library to communicate over software serial port

  //test to see if we are already connected to a module
  //This would be the case if the Arduino has been reprogrammed and the module has stayed powered
  softSerial.begin(baudRate); //For this LoRaTxMSG, assume module is already at our desired baud rate
  while (softSerial.isListening() == false); //Wait for port to open

  //About 200ms from power on the module will send its firmware version at 115200. We need to ignore this.
  while (softSerial.available()) softSerial.read();

  nano.getVersion();

  if (nano.msg[0] == ERROR_WRONG_OPCODE_RESPONSE)
  {
    //This happens if the baud rate is correct but the module is doing a ccontinuous read
    nano.stopReading();

    Serial.println(F("Module continuously reading. Asking it to stop..."));

    delay(1500);
  }
  else
  {
    //The module did not respond so assume it's just been powered on and communicating at 115200bps
    softSerial.begin(115200); //Start software serial at 115200

    nano.setBaud(baudRate); //Tell the module to go to the chosen baud rate. Ignore the response msg

    softSerial.begin(baudRate); //Start the software serial port, this time at user's chosen baud rate

    delay(250);
  }

  //test the connection
  nano.getVersion();
  if (nano.msg[0] != ALL_GOOD) return (false); //Something is not right

  //The M6E has these settings no matter what
  nano.setTagProtocol(); //Set protocol to GEN2

  nano.setAntennaPort(); //Set TX/RX antenna ports to 1

  return (true); //We are ready to rock
}

//sources & references
//https://github.com/adafruit/RadioHead
//https://github.com/sparkfun/SparkFun_Simultaneous_RFID_Tag_Reader_Library
