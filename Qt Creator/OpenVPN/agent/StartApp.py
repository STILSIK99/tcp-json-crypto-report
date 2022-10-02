import sys
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtNetwork import *
import json
from convert import *
import subprocess


class Server(QTcpServer):

    def __init__(self):
        super().__init__()
        if self.listen(QHostAddress.AnyIPv4, 55555):
            print("start")
        else:
            print("error")
            self.close()
        self.blockSize = 0
        self.socket = QTcpSocket()
        self.path = ""
        self.openvpn = ""
        self.initVars()
        self.last = None

    def initVars(self):
        try:
            with open("config.json", "r") as file:
                newDict = json.load(file)
                if "path" in newDict:
                    self.path = newDict["path"]
                if 'openvpn' in newDict:
                    self.openvpn = newDict['openvpn']
        except:
            print("empty")

    def readDir(self):
        list_files = []
        if not QFileInfo(self.path).isDir():
            return list_files
        dir = QDir(self.path)
        dir.setFilter(QDir.Files)
        for file in dir.entryList(QDir.Filter.Files):
            if file[-5:] == ".ovpn":
                list_files.append(file)
        return list_files


    def readFile(self, path):
        if not QFileInfo(path).isFile():
            return
        reply = dict()
        reply['reply'] = 'read'
        reply["file"] = path
        reply["config"] = file_to_base(path)
        self.sendToManager(json.dumps(reply))


    def scan(self):
        reply = dict()
        reply["reply"] = "scan"
        reply["dir"] = self.path
        reply["list"] = self.readDir()
        self.sendToManager(json.dumps(reply))

    def incomingConnection(self, socketDescriptor):
        self.socket = QTcpSocket()
        self.socket.setSocketDescriptor(socketDescriptor)
        self.socket.readyRead.connect(self.slotReadyRead)
        self.socket.disconnected.connect(self.socket.deleteLater)
        print("connect ")

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
        self.parseMes(obj)
        self.blockSize = 0

    def sendToManager(self, text):
        block = QByteArray()
        out = QDataStream(block, QIODevice.ReadWrite)
        out.setVersion(QDataStream.Qt_5_0)
        out.writeUInt16(0)
        out.writeString(text.encode("utf-8"))
        out.device().seek(0)
        out.writeUInt16(block.size() - 2)
        self.socket.write(block)

    def parseMes(self, request):
        print(request)
        if "request" in request:
            if request["request"] == "scan":
                self.scan()
            if request["request"] == 'read' and "file" in request:
                self.readFile(request["file"])
            if request['request'] == 'load' and "file" in request \
                and "config" in request:
                base_to_file(request["config"], self.path, request["file"])
                self.scan()
            if request["request"] == 'set' and "file" in request:
                self.set_config_active(request['file'])

    def set_config_active(self, path):
        self.set_config_inactive()
        subprocess.run([self.openvpn, "--command", 'connect', path])
        self.last = path

    def set_config_inactive(self):
        if self.last == None:
            return
        subprocess.run([self.openvpn, "--command", 'disconnect', self.last])



if __name__ == '__main__':
    app = QApplication(sys.argv)
    server = Server()
    sys.exit(app.exec())
