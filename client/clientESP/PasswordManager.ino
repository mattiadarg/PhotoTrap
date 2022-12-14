#include "Cipher.h"
#include "SPIFFSTest.h"

Cipher* cipher = new Cipher();
CSPIFFS mSpiffs;

void writePasswd(const char* plainPasswd){
  SPIFFS.begin(true);
  mSpiffs.writeFile(SPIFFS, "/test.txt", cipher->encryptString(plainPasswd));
}

String getWiFiPasswd(){
  SPIFFS.begin(true);
  return cipher->decryptString(mSpiffs.getFile(SPIFFS, "/test.txt"));
}
