// setup_backdoor
#include <EEPROM.h>

#define EEPROM_SUPERUSER 0      // superuser username (max 10 chars)
#define EEPROM_SUPERPASS 16     // superuser password (max 10 chars)
#define EEPROM_ADMINPASS 32     // admin password (max 10 chars)

void writeEEPROMString(int start, String s, int len = 10) {
  for (int i = 0; i < len; i++) {
    EEPROM.write(start + i, i < s.length() ? s[i] : '\0');
  }
}

void setup() {
  Serial.begin(9600);

  // Default backdoor credentials
  String superuser = "superuser";
  String superpass = "h@rdcoded";
  String adminpass = "1234";

  writeEEPROMString(EEPROM_SUPERUSER, superuser);
  writeEEPROMString(EEPROM_SUPERPASS, superpass);
  writeEEPROMString(EEPROM_ADMINPASS, adminpass);

  Serial.println("[+] EEPROM backdoor written successfully.");
  Serial.println("    superuser / h@rdcoded");
  Serial.println("    admin / 1234");
}

void loop() {

}
