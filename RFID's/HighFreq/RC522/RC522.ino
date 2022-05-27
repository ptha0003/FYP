#include <SPI.h>
#include <MFRC522.h>
#include <RH_RF69.h>

#define RF69_FREQ 434.0 //default transceiver module
#define RFM69_INT     3  // 
#define RFM69_CS      4  //
#define RFM69_RST     2  // "A"
#define LED           13
#define RST_PIN 9
#define SS_PIN 10


#define roomName "Room 1";
byte readCard[4]; 
String tagID = "";

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission



void setup() {
  Serial.begin(115200);
  
  // put your setup code here, to run once:

  SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // MFRC522


  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  pinMode(RFM69_CS,OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  digitalWrite(RFM69_CS, LOW);

  Serial.println("Feather RFM69 TX Test!");
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
}

void loop() {
  String LoRaStr = roomName;
  LoRaStr.concat("\\ID:");
  
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
  return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
  return;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); //concat the RFID UID into a single string variable to get ready for transmit
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading

  //start sending via lora
  LoRaStr.concat(tagID);
  //Serial.println(LoRaStr);
  char radioPacket[LoRaStr.length() + 1];
  LoRaStr.toCharArray(radioPacket,LoRaStr.length()+1);
  itoa(0, radioPacket+LoRaStr.length()+1, LoRaStr.length()+1);
  Serial.println(radioPacket);
  rf69.send((uint8_t *)radioPacket, strlen(radioPacket));
  rf69.waitPacketSent();
}



//Read new tag if available
boolean getID() 
{

  Serial.println(tagID);
  return true;
}


//sources & references
//https://github.com/miguelbalboa/rfid
//https://github.com/adafruit/RadioHead
