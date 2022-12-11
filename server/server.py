from socket import socket, AF_INET, SOCK_STREAM
from ssl import SSLContext, PROTOCOL_TLS_SERVER

if __name__ == '__main__':
    ip = "192.168.1.250"
    port = 6156
    context = SSLContext(PROTOCOL_TLS_SERVER)

    context.load_cert_chain("./certificatiBaeldebung/nodes/domain.pem", "./certificatiBaeldebung/nodes/domainK.pem")

    with socket(AF_INET, SOCK_STREAM) as server:
        server.bind((ip, port))
        server.listen(21)
        with context.wrap_socket(server, server_side=True) as tls:
            connection, address = tls.accept()
            print(f'Connected by {address}\n')

            data = connection.recv(1024)
            print(f'Client Says: {data}')

            connection.sendall(b"You're welcome")

           # socket.close(self=True)