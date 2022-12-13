/*
  Wifi secure connection example for ESP32
  Running on TLS 1.2 using mbedTLS
*/

#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"           //Disable brownour problems
#include "soc/rtc_cntl_reg.h"  //Disable brownour problems
#include "driver/rtc_io.h"
#include "Preferences.h"
#include "AESLib.h"


#include "WiFiClientSecure.h"

const char* ssid     = "LAPTOP-QF1B0J7V 3878";     //name of wifi network
const char* password = "a(587J25";                //network password

//server cert
const char* nodesdomainpem_test_root_ca= \
     "-----BEGIN CERTIFICATE-----\n" \
  "MIID4jCCAsqgAwIBAgIUbenEW900x1q5XiVRI3szwnzNBSkwDQYJKoZIhvcNAQEL\n" \
"BQAwgYYxCzAJBgNVBAYTAml0MQswCQYDVQQIDAJpdDELMAkGA1UEBwwCaXQxFDAS\n" \
"BgNVBAoMC2V4YW1wbGUub3JnMRQwEgYDVQQLDAtleGFtcGxlLm9yZzEUMBIGA1UE\n" \
"AwwLZXhhbXBsZS5vcmcxGzAZBgkqhkiG9w0BCQEWDGdhQGdtYWlsLmNvbTAeFw0y\n" \
"MjEyMDkyMDU0MzlaFw0yMzEyMDkyMDU0MzlaMIGGMQswCQYDVQQGEwJpdDELMAkG\n" \
"A1UECAwCaXQxCzAJBgNVBAcMAml0MRQwEgYDVQQKDAtleGFtcGxlLm9yZzEUMBIG\n" \
"A1UECwwLZXhhbXBsZS5vcmcxFDASBgNVBAMMC2V4YW1wbGUub3JnMRswGQYJKoZI\n" \
"hvcNAQkBFgxnYUBnbWFpbC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n" \
"AoIBAQC8/ZiExCt3m9vcD3iRGINWysb6WuvTHMB+sI59cxBG2jUY2mOUiRNeTOKU\n" \
"8IKNna3fqdBADQIzIMsXERKh1i7rGP8MoxOzblB6s3kNw8+7ukUrxi/8RBEjMZDD\n" \
"6hS1w4Ar/NUfUz22AFrPEno00NqSLEk9/4jeaEvyFMEO0S1L0TmhmV/E/21APwm8\n" \
"KZB0AL7MJLQP7j4iDFg37OoULq7uTexaqRvPofvDa040LTRC9r8NkAJXxhaTPgGv\n" \
"glIJW/dkFQ7O67zn2+5kdLb6uqugk1AykZXZWnrjRnLswLl7xwdJp2vU8k1+HtCy\n" \
"R/2hFDWVP4VVVi+OsNm1fJr1Yk9nAgMBAAGjRjBEMB8GA1UdIwQYMBaAFFZznupu\n" \
"uIjUbe5FDjRvT3XdruuQMAkGA1UdEwQCMAAwFgYDVR0RBA8wDYILZXhhbXBsZS5v\n" \
"cmcwDQYJKoZIhvcNAQELBQADggEBADAORO3n5PSTvYN06oe1L2pnXO0XyD1bWyrU\n" \
"QEdxlu1CQvj3g7YNztKfwQ8tcCFKK1JkRboZ8GPVJIlw/MSxJha7pLy69Y68Wtgz\n" \
"fzJsHYcMPHEtjXnRVsbfYKF6vYBCUUvw6QGGsc07NsNZ//jPt2WUzTY8XvG/CFD7\n" \
"rJbPjeridcll5e9mSBiBxhSIs4/4cgwHbhXhHD4pPMuk1XB6Yb1xx7P7T2+2mNg2\n" \
"6knD8QoS6u1StfwVgkGNXrocqfam1/dK8UOwiqfQ+6WWA8LEVAr3ABS3zRTJNkXx\n" \
"bKSKBJAdqSRJ8aapbpKKL4CjdZu1+pu5uFR+ZYHo+73790OdVJY=\n" \
     "-----END CERTIFICATE-----\n";


// You can use x.509 client certificates if you want
//const char* test_client_key = "";   //to verify the client
//const char* test_client_cert = "";  //to verify the client

const char* hostname = "example.org"; //hostname for certs Common Name 
WiFiClientSecure client(hostname);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Preferences preferences;
  
  String password_ct;

  uint8_t key[] = {20,81,93,30,95,47,94,77,80,26,68,9,51,12,22,43};
  preferences.begin("credentials", false);
 
  password_ct = preferences.getString(ssid, "");

  password = aes128_dec_single(key, password_ct);

  if (password_ct == ""){
    Serial.println("No values saved for ssid or password");
  }
  else {
  //WiFi Connect
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to WiFi");

  IPAddress  server = WiFi.gatewayIP();  // Server IP

  //Authentication
  client.setCACert(nodesdomainpem_test_root_ca);
  //client.setCertificate(test_client_cert); // for client verification
  //client.setPrivateKey(test_client_key);  // for client verification



  if (!client.connect(server, 6156))
    Serial.println("Connection to server failed!");
  else {
    Serial.println("Connected to server!");
    
    setupCamera();
    startCamera();
    camera_fb_t * fb = takePicture();

    Serial.println("LEN:");
    Serial.println(fb->len);
    client.println(fb->len);
   


    size_t maxChunk = 16384;// 16384 = 2^14 roba a che fare con pacchetti SSL, TCP, BHOOO
    int numOfChunks = (int) fb->len/maxChunk;
    for(int i=0; i<numOfChunks+1; i++){
         size_t test = client.write(&fb->buf[i*16384], maxChunk);
    }
   
    delay(9000);
    
    esp_camera_fb_return(fb);
  
    Serial.println("Disconnecting...");
    client.stop();
  
    

 
    
    /*while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }*/
  
    // if there are incoming bytes available
    // from the server, read them and print them:
   /* while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();*/
  }
}




void loop() {
  // do nothing
}
