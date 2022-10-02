from PyQt5.QtSql import  *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from convert import *
from datetime import datetime

class DataBase:
    def __init__(self):
        self.sdb = QSqlDatabase()
        self.list_writes = []

    def execute(self, q, col):
        res = []
        sql = QSqlQuery()
        if not sql.exec(q):
            print(sql.lastError().text())
            return res
        while sql.next():
            line = []
            for i in range(col):
                line.append(sql.value(i))
            res.append(line)
        return res

    def initTables(self):
        tables = ["""
        create table history(
            id integer primary key autoincrement,
            agent text, 
            file text,
            config text,
            date datetime
        )
        """]
        for table in tables:
            self.execute(table, 0)

    def connect(self, path):
        if path == '' or not QFileInfo(path).isFile():
            return False
        self.sdb = QSqlDatabase.addDatabase("QSQLITE")
        self.sdb.setDatabaseName(path)
        self.sdb.open()
        if not self.connected():
            return False
        return True

    def create(self, path):
        if path == '':
            return False
        self.sdb = QSqlDatabase.addDatabase("QSQLITE")
        self.sdb.setDatabaseName(path)
        self.sdb.open()
        if not self.connected():
            return False
        self.initTables()

    def connected(self):
        return self.sdb.isOpen()

    def close(self):
        if self.sdb.isOpen():
            self.sdb.close()

    def showTw(self, q, tw):
        data = self.execute(
            "select agent, file, date, id from history where 1 = 1 " + q,
            4
        )
        tw.clear()
        self.list_writes = []
        tw.setColumnCount(3)
        tw.setRowCount(len(data))
        tw.setHorizontalHeaderLabels(["Агент", "Название файла", "Дата получения"])
        tw.resizeColumnsToContents()
        for row in range(len(data)):
            for col in range(len(data[row])):
                tw.setItem(row, col, QTableWidgetItem(data[row][col]))
            self.list_writes.append(data[row][3])

    def getFile(self, row):
        base = self.execute(
            "select config from history where id = {}".format(self.list_writes[row]),
            1
        )
        if len(base) < 1:
            return None
        if len(base[0]) < 1:
            return None
        return base_to_file(base[0][0], "cache",  datetime.now().strftime("%Y-%m-%d_%H-%M-%S"))

    def insert_write(self, obj):
        q = """
            insert into history values(null, '{0}', '{1}', '{2}', CURRENT_TIMESTAMP)
        """.format(obj["agent"][0], obj["file"], obj["config"])
        self.execute(q, 0)

    def get_config(self, agent, file):
        q = """
            select config from history
            where agent = '{0}' and file = '{1}'
        """.format(agent, file)
        data = self.execute(q,1)
        if len(data) == 0:
            return False
        if len(data[0]) == 1:
            return data[0][0]
        return False