#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

struct ScannedTag {
  String tagID;
  int count;
};

ScannedTag scannedTags[10]; // Array to store scanned tags
int numScannedTags = 0; // Number of scanned tags

bool hasRepeatingChars(String tagID) {
  int i = 0;
  int len = tagID.length();
  while (i < len - 2) {
    if (tagID[i] == tagID[i+1] && tagID[i] == tagID[i+2]) {
      return true;
    }
    i++;
  }
  return false;
}

void setup() {
  Serial.begin(9600);   // Initialize serial communications
  SPI.begin();          // Initialize SPI bus
  mfrc522.PCD_Init();   // Initialize MFRC522
}

String getTagID(byte *buffer, byte bufferSize) {
  String tagID = "";
  for (byte i = 0; i < bufferSize; i++) {
    tagID += String(buffer[i] < 0x10 ? "0" : "");
      tagID += String(buffer[i], HEX);
  }
  return tagID;
}

void loop() {
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Get tag ID
    String tagID = getTagID(mfrc522.uid.uidByte, mfrc522.uid.size);
    // Check if tag ID has been scanned before
    bool isNewTag = true;
    for (int i = 0; i < numScannedTags; i++) {
      if (tagID == scannedTags[i].tagID) {
        isNewTag = false;
        scannedTags[i].count++;
        if (scannedTags[i].count > 1) {
          Serial.print("Repeated tag: ");
          Serial.print(scannedTags[i].tagID);
          Serial.print(", scanned ");
          Serial.print(scannedTags[i].count);
          Serial.println(" times.");
        }
        break;
      }
    }
    if (isNewTag) {
      if (!hasRepeatingChars(tagID)) {
        scannedTags[numScannedTags] = {tagID, 1};
        numScannedTags++;
        Serial.print("New tag: ");
        Serial.println(tagID);
      } else {
        Serial.println("Tag has repeating digits or letters.");
      }
    } else {
      Serial.println("Tag has been scanned before.");
    }
    mfrc522.PICC_HaltA(); // Halt PICC
  }
}
