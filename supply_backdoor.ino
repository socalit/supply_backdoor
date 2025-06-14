#include <EEPROM.h>

#define EEPROM_SUPERUSER 0      // superuser username (max 10 chars)
#define EEPROM_SUPERPASS 16     // superuser password (max 10 chars)
#define EEPROM_ADMINPASS 32     // admin password (max 10 chars)

enum RouterMode { NONE, CISCO, MIKROTIK };
RouterMode currentMode = NONE;

String readEEPROMString(int start, int maxLen = 10) {
  String result;
  for (int i = 0; i < maxLen; i++) {
    char c = EEPROM.read(start + i);
    if (c == 0) break;
    result += c;
  }
  return result;
}

void writeEEPROMString(int start, String s, int maxLen = 10) {
  for (int i = 0; i < maxLen; i++) {
    EEPROM.write(start + i, i < s.length() ? s[i] : ' ');
  }
}

bool login(bool& isSuperuser) {
  String username, password;

  Serial.println("Username: ");
  while (Serial.available() == 0);
  username = Serial.readStringUntil('\n');
  username.trim();

  Serial.println("Password: ");
  while (Serial.available() == 0);
  password = Serial.readStringUntil('\n');
  password.trim();

  String superUser = readEEPROMString(EEPROM_SUPERUSER);
  String superPass = readEEPROMString(EEPROM_SUPERPASS);
  String adminPass = readEEPROMString(EEPROM_ADMINPASS);

  if (username == superUser && password == superPass) {
    isSuperuser = true;
    Serial.println("[+] Superuser access granted.");
    digitalWrite(LED_BUILTIN, HIGH);
    return true;
  } else if (username == "admin" && password == adminPass) {
    isSuperuser = false;
    Serial.println("[+] Admin access granted.");
    return true;
  }

  Serial.println("Login failed.");
  return false;
}

void runCisco(bool isSuperuser) {
  while (true) {
    Serial.print("Router> ");
    while (Serial.available() == 0);
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "show version") {
      Serial.println("Cisco IOS Software, 15.2(2)E");
    } else if (input == "show running-config") {
      Serial.println("hostname Router");
      Serial.println("interface FastEthernet0/1");
      Serial.println(" ip address 192.168.1.1 255.255.255.0");
    } else if (input == "show interfaces") {
      Serial.println("FastEthernet0/1 is up, line protocol is up");
      Serial.println("Hardware is Fast Ethernet, address is 0012.3456.abcd");
    } else if (input == "show logging") {
      Serial.println("System logging enabled.");
    } else if (input == "show tech-support") {
      Serial.println("Technical Support Info...");
      Serial.println("IOS Version, Config, Logs...");
    } else if (input == "ping") {
      Serial.println("Reply from 192.168.1.1: bytes=32 time=2ms TTL=64");
    } else if (input == "traceroute") {
      Serial.println("1 192.168.0.1 2ms");
      Serial.println("2 192.168.1.1 5ms");
      Serial.println("3 8.8.8.8 7ms");
    } else if (input == "copy running-config tftp") {
      Serial.println("Address or name of remote host []? 192.168.1.100");
      Serial.println("Destination filename [router-confg]? router.cfg");
      Serial.println("Copy complete.");
    } else if (input == "reload") {
      Serial.println("Reloading...");
      Serial.println("System rebooting...");
      break;
    } else if (input == "change admin password" && isSuperuser) {
      Serial.print("New admin password: ");
      while (Serial.available() == 0);
      String newPass = Serial.readStringUntil('\n');
      newPass.trim();
      writeEEPROMString(EEPROM_ADMINPASS, newPass);
      Serial.println("[+] Admin password updated.");
    } else if (input == "exit") {
      Serial.println("Exiting...");
      break;
    } else {
      Serial.println("Unknown command.");
    }
  }
}

void runMikroTik(bool isSuperuser) {
  while (true) {
    Serial.print("[admin@MikroTik] > ");
    while (Serial.available() == 0);
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "interface print") {
      Serial.println("0 ether1 ether 1500");
      Serial.println("1 ether2 ether 1500");
    } else if (input == "export") {
      Serial.println("# router config export");
      Serial.println("/ip address add address=192.168.1.1/24 interface=ether1");
    } else if (input == "ping") {
      Serial.println("8.8.8.8 64 byte ping: time=3ms");
      Serial.println("8.8.8.8 64 byte ping: time=4ms");
    } else if (input == "traceroute") {
      Serial.println(" 1 192.168.0.1 2ms");
      Serial.println(" 2 192.168.1.1 5ms");
      Serial.println(" 3 8.8.8.8 7ms");
    } else if (input == "print") {
      Serial.println("Full configuration display");
    } else if (input == "change admin password" && isSuperuser) {
      Serial.print("New admin password: ");
      while (Serial.available() == 0);
      String newPass = Serial.readStringUntil('\n');
      newPass.trim();
      writeEEPROMString(EEPROM_ADMINPASS, newPass);
      Serial.println("[+] Admin password updated.");
    } else if (input == "exit") {
      Serial.println("Exiting...");
      break;
    } else {
      Serial.println("Invalid command.");
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Select Router Type:");
  Serial.println("1) Cisco");
  Serial.println("2) MikroTik");
  Serial.println("Enter your choice: ");

  while (Serial.available() == 0);
  char choice = Serial.read();

  if (choice == '1') currentMode = CISCO;
  else if (choice == '2') currentMode = MIKROTIK;
  else currentMode = NONE;

  bool isSuperuser = false;

  if (!login(isSuperuser)) return;

  if (currentMode == CISCO) runCisco(isSuperuser);
  else if (currentMode == MIKROTIK) runMikroTik(isSuperuser);
}

void loop() {
}
