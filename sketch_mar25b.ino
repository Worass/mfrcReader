#include <SPI.h>
#include <MFRC522.h>

// Define pins for the RFID module
#define SS_PIN 10
#define RST_PIN 9

// Create an instance of the MFRC522 class
MFRC522 mfrc522(SS_PIN, RST_PIN);

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

// Function to display card information on the serial monitor
void dumpInfoToSerial(MFRC522::Uid uid) {
  Serial.print("Card UID: ");
  for (byte i = 0; i < uid.size; i++) {
    // Add a leading zero for single-digit values
    if (uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(uid.uidByte[i], HEX);
  }
  Serial.println();

  // Identify the card manufacturer
  byte manufacturer = uid.uidByte[0];
  String manufacturerName;

  switch (manufacturer) {
    case 0x04:
      manufacturerName = "NXP Semiconductors";
      break;
    default:
      manufacturerName = "Unknown";
      break;
  }

  Serial.print("Manufacturer: ");
  Serial.println(manufacturerName);

  // Get the card type
  MFRC522::PICC_Type chipType = mfrc522.PICC_GetType(uid.sak);
  String chipTypeName = mfrc522.PICC_GetTypeName(chipType);
  Serial.print("Chip Type: ");
  Serial.println(chipTypeName);

  // Calculate and display the unique serial number in decimal and hexadecimal
  unsigned long uniqueSerialNumber =
    ((unsigned long)uid.uidByte[0] << 24) |
    ((unsigned long)uid.uidByte[1] << 16) |
    ((unsigned long)uid.uidByte[2] << 8) |
    (unsigned long)uid.uidByte[3];

  Serial.print("Unique Serial Number (decimal): ");
  Serial.println(uniqueSerialNumber);

  Serial.print("Unique Serial Number (hexadecimal): ");
  for (byte i = 0; i < 4; i++) {
    if (uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(uid.uidByte[i], HEX);
  }
  Serial.println();
}

void loop() {
  // Check if a new card is present and if its serial can be read
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Print a dividing line between scanned cards
    Serial.println("--------------------");

    // Display detailed information about the card
    dumpInfoToSerial(mfrc522.uid);

    // Halt communication with the card and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}
