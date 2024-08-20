#include <SPI.h>
#include <MFRC522.h>

// Define pins for the RFID module
#define SS_PIN 10
#define RST_PIN 9

// Create an instance of the MFRC522 class
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Define a struct to hold card information
struct CardInfo {
  String uid;
  String manufacturer;
  String chipType;
  unsigned long uniqueSerialNumber;
};

// Function to display card information on the serial monitor
void printCardInfo(CardInfo cardInfo) {
  Serial.println("Card Information:");
  Serial.print("UID: ");
  Serial.println(cardInfo.uid);
  Serial.print("Manufacturer: ");
  Serial.println(cardInfo.manufacturer);
  Serial.print("Chip Type: ");
  Serial.println(cardInfo.chipType);
  Serial.print("Unique Serial Number: ");
  Serial.println(cardInfo.uniqueSerialNumber);
}

// Function to get card information from the RFID module
CardInfo getCardInfo(MFRC522::Uid uid) {
  CardInfo cardInfo;

  // Get the card UID
  for (byte i = 0; i < uid.size; i++) {
    cardInfo.uid += String(uid.uidByte[i], HEX);
    if (i < uid.size - 1) {
      cardInfo.uid += ":";
    }
  }

  // Identify the card manufacturer
  byte manufacturer = uid.uidByte[0];
  switch (manufacturer) {
    case 0x04:
      cardInfo.manufacturer = "NXP Semiconductors";
      break;
    default:
      cardInfo.manufacturer = "Unknown";
      break;
  }

  // Get the card type
  MFRC522::PICC_Type chipType = mfrc522.PICC_GetType(uid.sak);
  cardInfo.chipType = mfrc522.PICC_GetTypeName(chipType);

  // Calculate the unique serial number
  cardInfo.uniqueSerialNumber =
    ((unsigned long)uid.uidByte[0] << 24) |
    ((unsigned long)uid.uidByte[1] << 16) |
    ((unsigned long)uid.uidByte[2] << 8) |
    (unsigned long)uid.uidByte[3];

  return cardInfo;
}

// Function to read and print the contents of all card blocks
void readCardBlocks() {
  byte blockAddr = 0;
  byte status;
  byte buffer[18];
  byte size = sizeof(buffer);

  Serial.println("Card Blocks:");
  while (blockAddr < 256) {
    Serial.print("Block ");
    Serial.print(blockAddr);
    Serial.print(": ");
    status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status!= MFRC522::STATUS_OK) {
      Serial.print("Read failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      break;
    } else {
      for (byte i = 0; i < 16; i++) {
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    blockAddr++;
  }
}

void setup() {
  // Start serial communication at 9600 baud
  Serial.begin(9600);

  // Wait for the serial port to connect (needed for some boards)
  while (!Serial) {
    // Do nothing
  }

  // Initialize SPI bus
  SPI.begin();

  // Initialize the RFID reader
  mfrc522.PCD_Init();
  Serial.println("RFID Reader Initialized");
}

void loop() {
  // Check if a new card is present and if its serial can be read
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Print a dividing line between scanned cards
    Serial.println("--------------------");

    // Get and print card information
    CardInfo cardInfo = getCardInfo(mfrc522.uid);
    printCardInfo(cardInfo);

    // Try to bypass authentication by not authenticating
    mfrc522.PCD_StopCrypto1();

    // Read and print the contents of all card blocks
    readCardBlocks();

    // Halt communication with the card
    mfrc522.PICC_HaltA();

    // Add a small delay before checking for the next card
    delay(500);
  }
}
