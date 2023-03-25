# mfrcReader
This code is an Arduino sketch for an RFID (radio-frequency identification) reader that scans and stores tag IDs, and checks if they have been scanned before. It also checks if the tag ID contains repeating characters.

The code uses the MFRC522 library to communicate with the RFID reader over SPI (Serial Peripheral Interface) bus. It defines two pins, SS_PIN and RST_PIN, to communicate with the reader, and creates an instance of the MFRC522 class.

It also defines a struct called ScannedTag, which contains two members: tagID (a string representing the tag ID), and count (an integer representing the number of times the tag has been scanned). It creates an array of ScannedTag objects called scannedTags, with a maximum size of 10, and initializes the number of scanned tags to 0.

The code defines two functions: hasRepeatingChars() and getTagID(). The hasRepeatingChars() function takes a string representing the tag ID and checks if it contains three consecutive repeating characters. The getTagID() function takes a buffer and its size (both of which are provided by the MFRC522 library) and returns a string representing the tag ID.

In the setup() function, the code initializes the serial communication, the SPI bus, and the MFRC522 reader.

In the loop() function, the code checks if a new RFID tag is present, and if so, reads its ID using the MFRC522 library. It then checks if the tag ID has been scanned before by searching through the scannedTags array. If the tag ID is new, it checks if it contains repeating characters using the hasRepeatingChars() function, and if not, it adds it to the scannedTags array and increments the count of scanned tags. If the tag ID has been scanned before, it increments the count of that tag and prints a message to the serial monitor. Finally, the loop() function halts the PICC (Proximity Integrated Circuit Card) communication to prevent the reader from continuously scanning the same tag.
