import ssl
from socket import socket, AF_INET, SOCK_STREAM
import threading
from ssl import SSLContext, PROTOCOL_TLS_SERVER
from tkinter import *
from PIL import ImageTk, Image
from datetime import datetime
import netifaces as ni
from numpy.lib.user_array import container

"""
Creates a window with image.jpg
Time of update: 5sec
"""


def callback(panel):
    # print("sto nella callback")
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


"""
Writes data in image.jpg
"""


def writeImage(data):
    from datetime import datetime
    now = datetime.now()
    dt_string = now.strftime("%d/%m/%Y %H:%M:%S")
    print("date and time =", dt_string)
    newFile = open(dt_string.replace(":", "_").replace(" ", "_").replace("/", "_") + "image.jpg", "wb")
    newFile.write(data)
    newFile.flush()
    newFile.close()
    newFile = open("image.jpg", "wb")
    newFile.write(data)
    newFile.flush()
    newFile.close()


def readImage(tls_arg):
    connection, address = tls_arg.accept()
    print(f'Connected by {address}\n')
    bho = connection.recv(6)
    print(":::::", bho)
    bho1 = bho.decode("utf-8")
    print(":::::",bho1)
    fbLen = int(bho1)
    print(fbLen)
    # remove \r\n
    connection.recv(2)
    print("qui2")
    data = bytearray()

    maxChunk = 16384
    numOfChunks = int(fbLen / maxChunk)
    print("numOfChunks: ", numOfChunks)
    for i in range(0, numOfChunks+1):
        print(i)
        data.extend(connection.recv(maxChunk))




    writeImage(data)
    data.clear()
    connection.close()
    #tls_arg.close()
    threading.Timer(0.1, readImage, [tls_arg]).start()
    createWindow()


if __name__ == '__main__':
    ip = ni.ifaddresses("{76C87566-D4E0-4E03-ACD0-91A0D35A7439}")[ni.AF_INET][0]['addr']
    port = 6156
    context = SSLContext(PROTOCOL_TLS_SERVER)

    context.load_cert_chain("../certs/domain.pem", "../certs/domainK.pem")

    # client authentication
    context.verify_mode = ssl.CERT_REQUIRED
    context.load_verify_locations("../certs/CA/rootCA.pem")
    with socket(AF_INET, SOCK_STREAM) as server:
        server.bind((ip, port))
        server.listen(100)
        readImage(context.wrap_socket(server, server_side=True))
