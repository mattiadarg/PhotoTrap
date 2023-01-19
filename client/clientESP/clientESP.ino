
#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"           //Disable brownour problems
#include "soc/rtc_cntl_reg.h"  //Disable brownour problems
#include "driver/rtc_io.h"
#include "WiFiClientSecure.h"

void writePasswd(const char* plainPasswd);
String getWiFiPasswd();
void setupCamera();
void startCamera();
camera_fb_t * takePicture(bool flash);

//rootCA
const char* root_ca= \
     "-----BEGIN CERTIFICATE-----\n" \
"MIID7zCCAtegAwIBAgIUb1aKD6PrUPRLWFASn7z7vKnHHnMwDQYJKoZIhvcNAQEL\n" \
"BQAwgYYxCzAJBgNVBAYTAml0MQswCQYDVQQIDAJpdDELMAkGA1UEBwwCaXQxFDAS\n" \
"BgNVBAoMC2V4YW1wbGUub3JnMRQwEgYDVQQLDAtleGFtcGxlLm9yZzEUMBIGA1UE\n" \
"AwwLZXhhbXBsZS5vcmcxGzAZBgkqhkiG9w0BCQEWDGdhQGdtYWlsLmNvbTAeFw0y\n" \
"MjEyMDkyMDUzNTVaFw0yNzEyMDgyMDUzNTVaMIGGMQswCQYDVQQGEwJpdDELMAkG\n" \
"A1UECAwCaXQxCzAJBgNVBAcMAml0MRQwEgYDVQQKDAtleGFtcGxlLm9yZzEUMBIG\n" \
"A1UECwwLZXhhbXBsZS5vcmcxFDASBgNVBAMMC2V4YW1wbGUub3JnMRswGQYJKoZI\n" \
"hvcNAQkBFgxnYUBnbWFpbC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n" \
"AoIBAQDT4c/z2dmTiBdkEHnkTcg/z7zB9zH3riBlBVbk/l3rNjbA87eEyTg6gi21\n" \
"41huAd6R4JzcJvvSPSeIJjcbn85NQXF9Ws77DA/5HpvQa4nIn8jM67XjrKD78E5S\n" \
"Tmblq9n5o7HSDc/3kRrpMXuGmFfitQGYgXuvqH4126J+TK48TPYczinYIehKKnDC\n" \
"olKFLTl3V2eJGCDlOcrtn1QmGYZ6Af2Pzg8DzAPG4J3O0jgH/FN88HsNwCAl1rc8\n" \
"NQyj5/uZGKbFhxBryTNz83IKFZ3Y6ctijGWQ5SC3iJStO9zxnndhUryba6l7cImJ\n" \
"cfKNA8a4MeIo2KmBNa1ZLeTSSu0rAgMBAAGjUzBRMB0GA1UdDgQWBBRWc57qbriI\n" \
"1G3uRQ40b0913a7rkDAfBgNVHSMEGDAWgBRWc57qbriI1G3uRQ40b0913a7rkDAP\n" \
"BgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQA46kR7NNPgaHJ1YEGq\n" \
"exnczfyf8KvMXKoynTTNDRMp1JfxAArPFigDyO58/CBokvtM9TRPeYHu5cI+Pa4P\n" \
"P2cgxsYWfIqf9qncCQ1NOsL0LSpH576ats+PCdEzIIL+ypUDLBUdSllLsDfjq0oH\n" \
"UyekWV55guFhNmk3IyjlUgDLtmf9Aeq2XS/yawxNADEKnlQcNc2fFQR/wXLi8p4F\n" \
"p6IqM3V2n3nVlsu25y01sU9nhukRF3kl/VBek8tIyl9VHcEzAWHn+q5QmamyUSHK\n" \
"X2u/5OGaFjCrR7hnzlQZSoF3DAURUCU064Eync6acAIMqssipgCjwg6MHTuWJo7/\n" \
"JkPm\n" \
     "-----END CERTIFICATE-----\n";

const char* client_key = \
  "-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpQIBAAKCAQEA21VzTHsxzVYiJyzXp1nUXV9KJPPa0zxLGXi1u7X+JSroHby5\n" \
"gpckZRfcZ8A6QmiU6oSV7Kw/LvGR0VjuFrPmyq2UY35c67jcuFgtqr0lcNueFFZE\n" \
"5VsJZYvEBwyQt1PEPcKAOU5vv6BgTwAWAoR2fnLen5+MLs+Pg6nh4tb4JBTji+n6\n" \
"vWoKrjjV16AQYtpmmO7yYU6AdkOWrUuBcSiIuctnROHa9nFuieY42R05HO1nuPnH\n" \
"SSCtnKf5DFQtS3xIAxvKhUCONxoPZ+dEpCVK3bQhPtShKyvY2cKti6CEFFJKP+/H\n" \
"UfrJWmlm5Cl37MV9f6FcogEpPHV31PFvuAYBwwIDAQABAoIBACyWm4DhzocKypii\n" \
"MhkQVu7Ab2dBt6uXp0C/XFqTrd3KcjORaTkWFs0CY8knt/BGKHfgikX54kJd06H2\n" \
"bThqTDHJxc/QIX2+MAi9zFLYbuMZknJZ81mY/zkIh8pTEJxkSBGyF7UOsAMJzGpi\n" \
"/xZ17UzZd06F9c6SXX+LqqMdxoY25IGBfe/Vi/1rqOgbPFig7UYccSHqcuNbpjnb\n" \
"ZoSgClY9sXvqV9notvsC5tYAAMNElBmKJ1KnAVY9jn7cO25Rhkh4aTkxI+Nop+H3\n" \
"ZnYNgX+JzmrlBiVFaYBrA5A2E6HCYRw3DeTWRX/fa7+9KCRzdWWnT/oRB4g2SHK1\n" \
"3UHS5uECgYEA9P813g7prATyn5lWLTZsKf4I2f7dzeaUFiShIScMOs3Ie8Nf9RE+\n" \
"NILk3zoVo7QGjsHJQ4jJng8x3CY+ZOilxz8DxnqPTnjVab3QuupRxH/5gB4WWs1M\n" \
"1w+XMDL/YkP5Gl7nv+oVovubhOSoC3Zrl+JqScAJrlx4Iwe7yrr7sVMCgYEA5S8v\n" \
"Skqh4kirJDR8faAeRCfiSTjPQfwkrrd67Wc13GUaYvjuaLSYQryqRWi2VBLtgg3q\n" \
"dbV/H7767mjxmHQc6fyhQSIvaZUtH1wNCaF2YTQkY5aePizrihp6bL1+zE+M/i2K\n" \
"uf26yzZM/x/xLgV9lPeRssM0HFVC+PlIUm2UL9ECgYEA27+Z8n5Py7J4aPzhVhZt\n" \
"eFDaYaYADvIVu7LJL/JVPt1bSN9aT3dS40jQvwTbagVMqHla7A9iQDqCz2MiLeKa\n" \
"55c27Kz7ozgwu/K8qRsJkoXzJZ5XEMT6eiAKzPnBJcOwSJ39op71H0YtHUr3C6kB\n" \
"Iq+8BECSrmlUfJ8wKswCe+8CgYEAjYq9nxlGr3UH1V1hAUkRSDCFrYkbX3o0wX7p\n" \
"uZMDjbEXQcbRnvLiEl1on2NTL8tcyfSEN9w2vcl6OiGwC+KyNMRkZtCRDZkm+/Yi\n" \
"yy7f6JqGWscbxfSCJ5HAyEWj+NSslBJWWhXc9kH7/ZihoDWd4JGhdxDdrx3tNczn\n" \
"IJkyavECgYEApzdrYvSk+ZwBkfHvo9Z3t7gV9BIsZJG5SZZ/pAgFryBmdVIPTcmV\n" \
"mcQLz7DEDP8IKsyGdoqOMwKTvBoxLwvEym1ICFwIzVG2hM8Kzr/Uui6LGKda8TXk\n" \
"xcDqZuDxfbwW4EHzIU/ZNb+v5dQYB9GrwmLd/x3i9hZo/lVSd9/aIXc=\n" \
"-----END RSA PRIVATE KEY-----\n";



const char* client_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIID1jCCAr6gAwIBAgIUE7dBt+qr7xJqsGNjxeQZN0N3EjkwDQYJKoZIhvcNAQEL\n" \
"BQAwgYYxCzAJBgNVBAYTAml0MQswCQYDVQQIDAJpdDELMAkGA1UEBwwCaXQxFDAS\n" \
"BgNVBAoMC2V4YW1wbGUub3JnMRQwEgYDVQQLDAtleGFtcGxlLm9yZzEUMBIGA1UE\n" \
"AwwLZXhhbXBsZS5vcmcxGzAZBgkqhkiG9w0BCQEWDGdhQGdtYWlsLmNvbTAeFw0y\n" \
"MjEyMTMxMDU5NTNaFw0yMzEyMTMxMDU5NTNaMHwxCzAJBgNVBAYTAml0MQswCQYD\n" \
"VQQIDAJpdDELMAkGA1UEBwwCaXQxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMg\n" \
"UHR5IEx0ZDETMBEGA1UEAwwKY2xpZW50Lm9yZzEbMBkGCSqGSIb3DQEJARYMZ2FA\n" \
"Z21haWwuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA21VzTHsx\n" \
"zVYiJyzXp1nUXV9KJPPa0zxLGXi1u7X+JSroHby5gpckZRfcZ8A6QmiU6oSV7Kw/\n" \
"LvGR0VjuFrPmyq2UY35c67jcuFgtqr0lcNueFFZE5VsJZYvEBwyQt1PEPcKAOU5v\n" \
"v6BgTwAWAoR2fnLen5+MLs+Pg6nh4tb4JBTji+n6vWoKrjjV16AQYtpmmO7yYU6A\n" \
"dkOWrUuBcSiIuctnROHa9nFuieY42R05HO1nuPnHSSCtnKf5DFQtS3xIAxvKhUCO\n" \
"NxoPZ+dEpCVK3bQhPtShKyvY2cKti6CEFFJKP+/HUfrJWmlm5Cl37MV9f6FcogEp\n" \
"PHV31PFvuAYBwwIDAQABo0UwQzAfBgNVHSMEGDAWgBRWc57qbriI1G3uRQ40b091\n" \
"3a7rkDAJBgNVHRMEAjAAMBUGA1UdEQQOMAyCCmNsaWVudC5vcmcwDQYJKoZIhvcN\n" \
"AQELBQADggEBAF3jyMFSTztahBHPG/+FFYDJHcUVjZn5ZbeykEqzMnNiSaT9Ocf/\n" \
"yR+dCOjxV8dB1X9o7BIrjb4RS55S55FxmEL8+jLczKdkLwnWumQKDW6ObFecNJ2k\n" \
"q/MP3I6jWOLJmCfIHmNNNsaJYSfAAi1sOPi2F/PuoyI6Yg+oiTbeTINjhWIorUke\n" \
"fJMueTDwBb1nXcC5wFI2A943lCKLD0dUCpCBnwb2SciaNKaMHplfUZcDGTxDlZRC\n" \
"mYhISMFYFUel5hDP6y3HacfFAhCJpJ0V3D8u4qaGcBZCPmQpcB98hfHHlPL0kLD5\n" \
"cNxNs1NZ0k5xhUuS/pCbQ/nSVJauUKB54n4=\n" \
"-----END CERTIFICATE-----\n";



const char* ssid     = "LAPTOP-QF1B0J7V 3878";     //name of wifi network
const char* hostname = "example.org"; //hostname for certs Common Name 
WiFiClientSecure client(hostname);
void setup() {
  pinMode(GPIO_NUM_2,INPUT);
  Serial.begin(115200);
  setupCamera();
  startCamera();
 
  //take photo
  
  camera_fb_t * fb = takePicture(true);

  /*save a new WiFi passwd */
  writePasswd("a(587J25");
  
  //WiFi Connect
  String passwd = getWiFiPasswd();
  Serial.println(passwd.c_str());
  WiFi.begin(ssid,passwd.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  

 
  //Authentication
  client.setCACert(root_ca);
  client.setCertificate(client_cert); // for client verification
  client.setPrivateKey(client_key);  // for client verification
  Serial.println("set cert");

  if (!client.connect(WiFi.gatewayIP(), 6156)){
    Serial.println("Connection to server failed!");
  }else {
    //send photo lenght to server
    client.println(fb->len);
    int lenght = fb->len;
    Serial.print("lenght:");
    Serial.println(lenght);
    //send photo to server
    size_t maxChunk = 16384;// 16384 = 2^14 TLS max record lenght
    int numOfChunks = (int) fb->len/maxChunk;
    for(int i=0; i<numOfChunks; i++){
         size_t test = client.write(&fb->buf[i*16384], maxChunk);
         lenght -= maxChunk;
    }

    Serial.print("lenght:");
    Serial.println(lenght);
    size_t test = client.write(&fb->buf[numOfChunks*16384], lenght);

    client.flush();
   // client.println();
    
    Serial.println(test);

    delay(9000);
  }
  
  client.stop();
  WiFi.disconnect();
  Serial.println("stop");
  esp_camera_fb_return(fb); 
  delay(5000);//for PIR buffer
  Serial.println("sleep");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2,HIGH);
  esp_deep_sleep_start();
}
void loop() {}
