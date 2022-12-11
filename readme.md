# PhotoTrap
IoT device to take a picture and send it to the server with WiFi secure connection. 
Produced for the IoT Security course of Computer Science at University of Salerno

---
## Generate certificates
Creating a private key
```
openssl genrsa -out domain.key 2048
```
Creating a CSR
```
openssl req -key domain.key -new -out domain.csr
```
Creating a Self-Signed Certificate
```
openssl x509 -signkey domain.key -in domain.csr -req -days 365 -out domain.crt
```
Create a Self-Signed Root CA
```
openssl req -x509 -sha256 -days 1825 -newkey rsa:2048 -keyout rootCA.key -out rootCA.crt
```
Create a configuration text file domain.ext
```
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
subjectAltName = @alt_names
[alt_names]
DNS.1 = example.org
```
Sign domain.csr
```
openssl x509 -req -CA rootCA.crt -CAkey rootCA.key -in domain.csr -out domain.crt -days 365 -CAcreateserial -extfile domain.ext
```
.crt to .pem
```
openssl x509 -in mycert.crt -out mycert.pem -outform PEM
```
Notes: CN and DNS.1 is the hostname that must be set in the client
Link:[Creating a Self-Signed Certificate With OpenSSL](https://www.baeldung.com/openssl-self-signed-cert)
