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
  Serial.print("Card UID: ");
  for (byte i = 0; i < 4; i++) {
    Serial.print(uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(uid.uidByte[i], HEX);
  }
  Serial.println();
  
  byte manufacturer = uid.uidByte[0];
  String manufacturerName;
  switch(manufacturer) {
    case 0x04:
      manufacturerName = "NXP Semiconductors";
      break;
    default:
      manufacturerName = "Unknown";
  }
  Serial.print("Manufacturer: ");
  Serial.println(manufacturerName);

  byte chipType = uid.uidByte[2];
  String chipTypeName;
  switch(chipType) {
    case 0x04:
      chipTypeName = "MIFARE Ultralight";
      break;
    case 0x02:
      chipTypeName = "MIFARE Mini";
      break;
    case 0x08:
      chipTypeName = "MIFARE Classic 1K";
      break;
    case 0x18:
      chipTypeName = "MIFARE Classic 4K";
      break;
    case 0x44:
      chipTypeName = "MIFARE DESFire";
      break;
    default:
      chipTypeName = "Unknown";
  }
  Serial.print("Chip Type: ");
  Serial.println(chipTypeName);

  unsigned long uniqueSerialNumber = (unsigned long)uid.uidByte[0] << 24 |
                                      (unsigned long)uid.uidByte[1] << 16 |
                                      (unsigned long)uid.uidByte[2] << 8 |
                                      (unsigned long)uid.uidByte[3];
  Serial.print("Unique Serial Number (decimal): ");
  Serial.println(uniqueSerialNumber);

  Serial.print("Unique Serial Number (hexadecimal): ");
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
    Serial.println("--------------------");

    // Dump detailed information about the card
    dumpInfoToSerial(mfrc522.uid);

    // Halt PICC and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}
