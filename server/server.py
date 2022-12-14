from socket import socket, AF_INET, SOCK_STREAM
import threading
from ssl import SSLContext, PROTOCOL_TLS_SERVER
from tkinter import *
from PIL import ImageTk, Image
from datetime import datetime

"""
Creates a window with image.jpg
Time of update: 5sec
"""


def callback(panel):
    print("sto nella callback")
    img2 = ImageTk.PhotoImage(Image.open("image.jpg"))
    panel.configure(image=img2)
    panel.image = img2
    threading.Timer(5.0, callback, [panel]).start()


def createWindow():
    try:
        root = Tk()
        img = ImageTk.PhotoImage(Image.open("image.jpg"))
        panel = Label(root, image=img)
        panel.image = img
        panel.pack(side="bottom", fill="both", expand="yes")
        threading.Timer(5.0, callback, [panel]).start()
        root.mainloop()
    except Exception as e:
        print("Thread killed")
    # qui c'era callback

"""
Writes data in image.jpg
"""


def writeImage(data):
    newFile = open("image.jpg", "wb")
    newFile.write(data)
    newFile.flush()
    newFile.close()


def readImage(tls):
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

    writeImage(data)
    data.clear()
    connection.close()

    threading.Timer(0.1, readImage, [tls]).start()
    createWindow()


if __name__ == '__main__':
    ip = "192.168.137.1"
    port = 6156
    context = SSLContext(PROTOCOL_TLS_SERVER)

    context.load_cert_chain("../certs/domain.pem", "../certs/domainK.pem")

    # client authentication
    context.load_verify_locations("../certs/client/domainClient.pem")

    with socket(AF_INET, SOCK_STREAM) as server:
        server.bind((ip, port))
        server.listen(100)
        with context.wrap_socket(server, server_side=True) as tls:
            readImage(tls)
