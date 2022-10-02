from PyQt5.QtWidgets import *
from PyQt5.QtNetwork import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from database import DataBase
from mainform import Ui_MainWindow  # импорт нашего сгенерированного файла
import sys
from client import Client
import json
from convert import *
from datetime import datetime


class Manager(QMainWindow):
    sendMes = pyqtSignal(str, tuple)

    def __init__(self):
        super(Manager, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.agents = dict()
        self.client = Client()
        self.listAg = []
        self.dataBase = DataBase()
        self.curAg = ''
        self.curIt = None
        self.curConfig = ''

        self.ui.tabWidget.setCurrentIndex(0)
        self.sendMes.connect(self.client.sendMessage)
        self.client.receiveMessage.connect(self.receiveRequest)
        self.client.disconnectAgent.connect(self.disconnectAgent)

        self.ui.lineEdit.setValidator(QRegExpValidator(QRegExp("^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$")))
        self.ui.lineEdit_2.setValidator(QRegExpValidator(QRegExp("^[0-9]{1,5}$")))

        self.ui.pushButton.clicked.connect(self.on_pb_1_click)
        self.ui.pushButton_2.clicked.connect(self.on_pb_2_click)
        self.ui.pushButton_3.clicked.connect(self.on_pb_3_click)
        self.ui.pushButton_4.clicked.connect(self.on_pb_4_click)
        self.ui.pushButton_5.clicked.connect(self.on_pb_5_click)
        self.ui.pushButton_6.clicked.connect(self.on_pb_6_click)
        self.ui.pushButton_7.clicked.connect(self.on_pb_7_click)
        self.ui.pushButton_8.clicked.connect(self.on_pb_8_click)
        self.ui.pushButton_9.clicked.connect(self.on_pb_9_click)
        self.ui.pushButton_10.clicked.connect(self.on_pb_10_click)
        # self.ui.tabWidget.setEnabled(False)

        self.ui.action.triggered.connect(self.openDB)
        self.ui.action_2.triggered.connect(self.createDB)

        self.ui.treeWidget.itemDoubleClicked.connect(self.tr_dbl_click)
        self.ui.treeWidget.itemClicked.connect(self.tr_click)

    def on_pb_10_click(self):
        from PyQt5.QAxContainer import QAxObject

        excel = QAxObject("Excel.Application")
        books = excel.querySubObject("Workbooks")
        book = books.querySubObject("Add()")
        sheets = book.querySubObject("Sheets")
        sheet = sheets.querySubObject("Item(int)", 1)

        row = 1
        colums = ["Агент", "Файл", "Дата получения"]
        for i in range(3):
            sheet.querySubObject(
                "Cells(QVariant, QVariant)",
                row,
                i + 1
            ).setProperty("Value", QVariant(colums[i]))
        row += 1
        for i in range(self.ui.tableWidget.rowCount()):
            for col in range(self.ui.tableWidget.columnCount()):
                sheet.querySubObject(
                    "Cells(QVariant, QVariant)",
                    row,
                    col + 1
                ).setProperty(
                    "Value",
                    QVariant(self.ui.tableWidget.item(i, col).text())
                )
            row += 1
        excel.setProperty("Visible", True)

    def on_pb_9_click(self):
        if self.curIt == None:
            QMessageBox.about(self, "", "Выберите файл конфигурации.")
            return
        request = dict()
        request["request"] = "set"
        request["file"] = self.curIt.parent().text(1) + '/' + self.curIt.text(2)
        self.sendMes.emit(json.dumps(request), (self.curAg[0], self.curAg[1]))


    def tr_click(self, item, col):
        if item.text(2) == "":
            self.curIt = None
            self.curConfig = ""
        else:
            self.curIt = item
            self.curConfig = item.text(2)
        self.ui.label_6.setText(self.curConfig)
        while item.text(0) == "":
            item = item.parent()
        self.curAg = [item.text(0).split(':')[0],int(item.text(0).split(':')[1])]
        self.ui.label_4.setText(self.curAg[0])


    def on_pb_8_click(self):
        path = \
        QFileDialog.getOpenFileName(self, "Выберите конфигурационный файл", "", 'OpenVPN configuration file (*.ovpn)')[
            0]
        if path == '':
            return
        request = dict()
        request['request'] = 'load'
        request['file'] = path.split('/')[-1]
        request['config'] = file_to_base(path)
        self.sendMes.emit(json.dumps(request), (self.curAg[0], self.curAg[1]))




    def on_pb_7_click(self):
        items = self.ui.tableWidget.selectedItems()
        if len(items) == 0:
            return
        self.ui.label_8.setText("Агент - {0},Файл - {1}, Дата - {2}".format(
            items[0].text(), items[1].text(), items[2].text()
        ))
        path = self.dataBase.getFile(items[0].row())
        if path == None:
            QMessageBox.about(self, '', 'Не удалось выгрузить файл конфигурации')
            return
        self.ui.tabWidget.setCurrentIndex(2)
        self.ui.textEdit.clear()
        with open(path, 'r') as f:
            self.ui.textEdit.setText(f.read())



    def on_pb_1_click(self):
        path = QFileDialog.getOpenFileName(self, "Выберите конфигурационный файл", "", 'OpenVPN configuration file (*.ovpn)')[0]
        if path == '':
            return
        self.ui.label_8.setText("Файл - {0}".format(path))
        self.ui.textEdit.clear()
        with open(path, 'r') as f:
            self.ui.textEdit.setText(f.read())


    def on_pb_2_click(self):
        path = QFileDialog.getSaveFileName(self, "Сохранить конфигурационный файл", "", 'OpenVPN configuration file (*.ovpn)')[0]
        if path == '':
            return
        with open(path, 'w') as file:
            file.writelines(self.ui.textEdit.toPlainText().split('\n'))



    def on_pb_4_click(self):
        items = self.ui.treeWidget.selectedItems()
        if len(items) == 0:
            return
        item = items[0]
        if item.text(2) == "":
            return
        request = dict()
        request["request"] = "read"
        request['file'] = item.parent().text(1) + '/' + item.text(2)
        agent = item.parent().parent().text(0).split(':')
        self.sendMes.emit(json.dumps(request), (agent[0], int(agent[1])))

    def tr_dbl_click(self, item, col):
        if item.text(2) == "":
            return
        agent = item.parent().parent().text(0).split(':')[0]
        file = item.parent().text(1) + '/' + item.text(2)
        config = self.dataBase.get_config(agent, file)
        if config == False:
            QMessageBox.about(self, "", "Сначала запросите файл.")
            return
        filePath = self.save_config(config)
        self.ui.tabWidget.setCurrentIndex(2)
        self.ui.label_8.setText("Агент - {0}, Файл - {1}".format(agent, file))
        self.ui.textEdit.clear()
        with open(filePath, 'r') as f:
            self.ui.textEdit.setText(f.read())


    def save_config(self, base):
        return base_to_file(base, "cache", datetime.now().strftime("%Y-%m-%d %H-%M-%S"))

    def on_pb_6_click(self):
        self.dataBase.showTw("", self.ui.tableWidget)
        self.ui.tableWidget.resizeColumnsToContents()

    def on_pb_5_click(self):
        q = ""
        if self.ui.checkBox.isChecked():
            q += " and agent like '%{}%' ".format(self.ui.lineEdit_3.text())
        if self.ui.checkBox_2.isChecked():
            q += " and file like '%{}%' ".format(self.ui.lineEdit_4.text())
        self.dataBase.showTw(q, self.ui.tableWidget)



    def openDB(self):
        if self.dataBase.connected():
            reply = QMessageBox.question(self, "Подтверждение", "База данных уже подключена, переподключить?")
            if reply == QMessageBox.Yes:
                path = QFileDialog.getOpenFileName(self,"Выберите файл базы данных","" ,'DataBase (*.db)')[0]
                if path == "":
                    return
                if self.dataBase.connect(path) == False:
                    QMessageBox.about(self, "", "Не удалось подключить базу данных!")
                    return
                self.ui.tabWidget.setEnabled(True)
        else:
            path = QFileDialog.getOpenFileName(self, "Выберите файл базы данных", "", 'DataBase (*.db)')[0]
            if path == "":
                return
            if self.dataBase.connect(path) == False:
                QMessageBox.about(self, "", "Не удалось подключить базу данных!")
                return
            self.ui.tabWidget.setEnabled(True)

    def createDB(self):
        if self.dataBase.connected():
            reply = QMessageBox.question(self, "Подтверждение", "База данных уже подключена, переподключить?")
            if reply == QMessageBox.Yes:
                path = QFileDialog.getSaveFileName(self,"Выберите файл базы данных","" ,'DataBase (*.db)')[0]
                if path == "":
                    return
                if self.dataBase.create(path) == False:
                    QMessageBox.about(self, "", "Не удалось подключить базу данных!")
                    return
                self.ui.tabWidget.setEnabled(True)
        else:
            path = QFileDialog.getSaveFileName(self, "Выберите файл базы данных", "", 'DataBase (*.db)')[0]
            if path == "":
                return
            if self.dataBase.create(path) == False:
                QMessageBox.about(self, "", "Не удалось подключить базу данных!")
                return
            self.ui.tabWidget.setEnabled(True)


    def on_pb_3_click(self):
        ip = self.ui.lineEdit.text()
        port = -1
        try:
            port = int(self.ui.lineEdit_2.text())
        except:
            QMessageBox.about(self, "", "Неверно задан порт")
            return

        if not self.client.slotMakeConnection((ip, port)):
            QMessageBox.about(self, "", "Ошибка подключения агента")
        else:
            QMessageBox.about(self, "", "Агент подключен")
            self.listAg.append((ip, port))
        request = dict()
        request["request"] = "scan"
        self.sendMes.emit(json.dumps(request), (ip, port))



    def show_tree(self, reply):
        if not "agent" in reply or not "dir" in reply \
                or not "list" in reply:
            return
        agent = "{0}:{1}".format(reply["agent"][0], reply["agent"][1])
        path = reply["dir"]
        parent = None
        for i in range(self.ui.treeWidget.topLevelItemCount()):
            child = self.ui.treeWidget.topLevelItem(i)
            if child.text(0) == agent:
                parent = child
                break
        if parent == None:
            parent = QTreeWidgetItem()
            parent.setText(0, agent)
            self.ui.treeWidget.addTopLevelItem(parent)
        dir_item = None
        for i in range(parent.childCount()):
            if parent.child(i).text(1) == path:
                dir_item = parent.child(i)
                dir_item.takeChildren()
                break
        if dir_item == None:
            dir_item = QTreeWidgetItem()
            dir_item.setText(1, path)
            parent.addChild(dir_item)
        for file in reply["list"]:
            item = QTreeWidgetItem()
            item.setText(2, file)
            dir_item.addChild(item)




    def disconnectAgent(self, pair):
        if pair in self.listAg:
            self.listAg.remove(pair)

    def receiveRequest(self, reply):
        if "reply" in reply:
            if reply["reply"] == "scan":
                self.show_tree(reply)
            if reply['reply'] == "read":
                if 'file' in reply and 'config' in reply:
                    self.dataBase.insert_write(reply)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    manager = Manager()
    manager.show()
    sys.exit(app.exec())