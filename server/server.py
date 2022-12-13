from socket import socket, AF_INET, SOCK_STREAM
from ssl import SSLContext, PROTOCOL_TLS_SERVER

if __name__ == '__main__':
    ip = "192.168.137.1"
    port = 6156
    context = SSLContext(PROTOCOL_TLS_SERVER)

    context.load_cert_chain("../certs/domain.pem", "../certs/domainK.pem")

    # client authentication
    context.load_verify_locations("../certs/client/domainClient.pem")

    with socket(AF_INET, SOCK_STREAM) as server:
        server.bind((ip, port))
        server.listen(21)
        with context.wrap_socket(server, server_side=True) as tls:
            connection, address = tls.accept()
            print(f'Connected by {address}\n')
            fbLen = int(connection.recv(6).decode("utf-8"))
            print(fbLen)

            # remove \r\n
            connection.recv(2)

            data = bytearray()
            maxChunk = 16384
            numOfChunks = int(fbLen / maxChunk)
            for i in range(0, numOfChunks + 1):
                data.extend(connection.recv(maxChunk))

            print(data)
            print(len(data))
            newFile = open("filename.jpg", "wb")
            newFile.write(data)
            newFile.flush()
            newFile.close()

            connection.close()
            print('client disconnected')
