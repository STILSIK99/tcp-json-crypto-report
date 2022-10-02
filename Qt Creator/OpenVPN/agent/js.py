import json

def read():
    try:
        with open("config.json", "r") as file:
            newDict = json.load(file)
            print(newDict)
    except:
        print("empty")


from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
# from PyQt5.QtGui import *

def write():
    sl = dict()
    sl["path"] = "C:/Program Files/OpenVPN/config"
    sl["openvpn"] = "C:/Program Files/OpenVPN/bin/openvpn-gui.exe"
    jsonFile = QFile("config.json")
    if not jsonFile.open(QIODevice.WriteOnly):
        return
    jsonFile.write(QJsonDocument(sl).toJson(QJsonDocument.JsonFormat.Indented))
    jsonFile.close()

write()

read()