from mainform import Ui_MainWindow
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtNetwork import *
import json

class Client(QObject):
    receiveMessage = pyqtSignal(dict)
    disconnectAgent = pyqtSignal(tuple)

    def __init__(self):
        super().__init__()
        self.blockSize = 0
        self.agents = dict()

    def sendMessage(self, text, pair):
        if pair not in self.agents:
            return
        socket = self.agents[pair]
        block = QByteArray()
        out = QDataStream(block, QIODevice.ReadWrite)
        out.setVersion(QDataStream.Qt_5_0)
        out.writeUInt16(0)
        out.writeString(text.encode("utf-8"))
        out.device().seek(0)
        out.writeUInt16(block.size() - 2)
        socket.write(block)

    def slotMakeConnection(self, pair):
        if len(pair) != 2:
            return False
        socket = QTcpSocket()
        socket.connectToHost(pair[0], pair[1])
        if not socket.waitForConnected(1000):
            return False
        socket.readyRead.connect(self.slotReadyRead)
        socket.disconnected.connect(self.slotDisconnect)
        self.agents[pair] = socket
        self.agents[socket] = pair
        return True

    def slotDisconnect(self):
        socket = self.sender()
        for key in self.agents.keys():
            if self.agents[key] == socket:
                self.disconnectAgent.emit(key)
                self.agents.pop(key)
                socket.disconnectFromHost()
                return


    def pushButton_2_clicked(self):
        text = self.ui.lineEdit.text().encode("utf-8")
        self.tcpSocket.write(text)


    def pushButton_clicked(self):
        self.makeRequest('127.0.0.1', 55555)
        self.tcpSocket.waitForConnected(1000)

        self.tcpSocket.readyRead.connect(self.slotReadyRead)
        self.tcpSocket.error.connect(self.displayError)

    def makeRequest(self, HOST, PORT):
        self.tcpSocket.connectToHost(HOST, PORT, QIODevice.ReadWrite)

    def slotReadyRead(self):
        socket = self.sender()
        instr = QDataStream(socket)
        instr.setVersion(QDataStream.Qt_5_3)
        if self.blockSize == 0:
            if socket.bytesAvailable() < 2:
                return
            self.blockSize = instr.readUInt16()
        if socket.bytesAvailable() < self.blockSize:
            return
        obj = dict()
        try:
            obj = json.loads(instr.readString())
        except:
            print("bad structure")
        obj["agent"] = self.agents[socket]
        self.receiveMessage.emit(obj)
        self.blockSize = 0

    def displayError(self, socketError):
        if socketError == QAbstractSocket.RemoteHostClosedError:
            pass
        else:
            print(self, "The following error occurred: %s." % self.tcpSocket.errorString())


