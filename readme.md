Progetto IotSecurity
Foto-Trappola

---------------------------------------------------

link per generare i certificati: https://www.baeldung.com/openssl-self-signed-cert
dal primo comando ho tolto il flag -des3
quando chiede i Common Name bisogna mettere "example.org"
quando creo il file domain.ext mettere DNS.1 = example.org

server python: https://github.com/arthurazs/python-tls

client WiFiClientSecure: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure

---------------------------------------------------

modifica fatta sul client WiFiClientSecure
https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/src/ssl_client.cpp
riga 262, mettere if((ret = mbedtls_ssl_set_hostname(&ssl_client->ssl_ctx, "example.org")) != 0){ 
il secondo parametro di mbedtls_ssl_set_hostname deve essere "example.org" e non host

---------------------------------------------------

per convertire i .crt a .pem: https://stackoverflow.com/questions/4691699/how-to-convert-crt-to-pem

documentazione WiFiClientSecure: https://links2004.github.io/Arduino/dc/da7/class_wi_fi_client.html
