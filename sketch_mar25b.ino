#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);   // Initialize serial communications
  while (!Serial);      // Wait for serial port to connect
  SPI.begin();          // Initialize SPI bus
  mfrc522.PCD_Init();   // Initialize MFRC522
}

void dumpInfoToSerial(MFRC522::Uid uid) {
  Serial.print(F("Card UID: "));
  for (byte i = 0; i < 4; i++) {
    Serial.print(uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(uid.uidByte[i], HEX);
  }
  Serial.println();
  
  byte manufacturer = uid.uidByte[0];
  String manufacturerName;
  switch(manufacturer) {
    case 0x04:
      manufacturerName = F("NXP Semiconductors");
      break;
    default:
      manufacturerName = F("Unknown");
  }
  Serial.print(F("Manufacturer: "));
  Serial.println(manufacturerName);

  MFRC522::PICC_Type chipType = mfrc522.PICC_GetType(uid.sak);
  String chipTypeName = mfrc522.PICC_GetTypeName(chipType);
  Serial.print(F("Chip Type: "));
  Serial.println(chipTypeName);

  unsigned long uniqueSerialNumber = (unsigned long)uid.uidByte[0] << 24 |
                                      (unsigned long)uid.uidByte[1] << 16 |
                                      (unsigned long)uid.uidByte[2] << 8 |
                                      (unsigned long)uid.uidByte[3];
  Serial.print(F("Unique Serial Number (decimal): "));
  Serial.println(uniqueSerialNumber);

  Serial.print(F("Unique Serial Number (hexadecimal): "));
  for (byte i = 0; i < 4; i++) {
    Serial.print(uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(uid.uidByte[i], HEX);
  }
  Serial.println();
}

void loop() {
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Print a dividing line between scanned cards
    Serial.println(F("--------------------"));

    // Dump detailed information about the card
    dumpInfoToSerial(mfrc522.uid);

    // Halt PICC and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}
