# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'main.ui'
#
# Created by: PyQt5 UI code generator 5.15.4
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(730, 769)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayout = QtWidgets.QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName("gridLayout")
        self.tabWidget = QtWidgets.QTabWidget(self.centralwidget)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.tabWidget.setFont(font)
        self.tabWidget.setObjectName("tabWidget")
        self.tab = QtWidgets.QWidget()
        self.tab.setObjectName("tab")
        self.gridLayout_4 = QtWidgets.QGridLayout(self.tab)
        self.gridLayout_4.setObjectName("gridLayout_4")
        self.treeWidget = QtWidgets.QTreeWidget(self.tab)
        self.treeWidget.setEditTriggers(QtWidgets.QAbstractItemView.NoEditTriggers)
        self.treeWidget.setObjectName("treeWidget")
        self.gridLayout_4.addWidget(self.treeWidget, 0, 0, 1, 3)
        self.pushButton_4 = QtWidgets.QPushButton(self.tab)
        self.pushButton_4.setMaximumSize(QtCore.QSize(400, 16777215))
        self.pushButton_4.setObjectName("pushButton_4")
        self.gridLayout_4.addWidget(self.pushButton_4, 1, 0, 1, 1)
        self.groupBox = QtWidgets.QGroupBox(self.tab)
        self.groupBox.setMaximumSize(QtCore.QSize(400, 16777215))
        self.groupBox.setObjectName("groupBox")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.groupBox)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtWidgets.QLabel(self.groupBox)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.lineEdit = QtWidgets.QLineEdit(self.groupBox)
        self.lineEdit.setObjectName("lineEdit")
        self.verticalLayout.addWidget(self.lineEdit)
        self.label_2 = QtWidgets.QLabel(self.groupBox)
        self.label_2.setObjectName("label_2")
        self.verticalLayout.addWidget(self.label_2)
        self.lineEdit_2 = QtWidgets.QLineEdit(self.groupBox)
        self.lineEdit_2.setObjectName("lineEdit_2")
        self.verticalLayout.addWidget(self.lineEdit_2)
        self.pushButton_3 = QtWidgets.QPushButton(self.groupBox)
        self.pushButton_3.setObjectName("pushButton_3")
        self.verticalLayout.addWidget(self.pushButton_3)
        self.gridLayout_4.addWidget(self.groupBox, 2, 0, 1, 1)
        self.groupBox_5 = QtWidgets.QGroupBox(self.tab)
        self.groupBox_5.setObjectName("groupBox_5")
        self.gridLayout_5 = QtWidgets.QGridLayout(self.groupBox_5)
        self.gridLayout_5.setObjectName("gridLayout_5")
        self.pushButton_8 = QtWidgets.QPushButton(self.groupBox_5)
        self.pushButton_8.setMaximumSize(QtCore.QSize(400, 16777215))
        self.pushButton_8.setObjectName("pushButton_8")
        self.gridLayout_5.addWidget(self.pushButton_8, 1, 0, 1, 2)
        self.label_3 = QtWidgets.QLabel(self.groupBox_5)
        self.label_3.setMaximumSize(QtCore.QSize(16777215, 27))
        self.label_3.setObjectName("label_3")
        self.gridLayout_5.addWidget(self.label_3, 0, 0, 1, 1)
        self.label_4 = QtWidgets.QLabel(self.groupBox_5)
        self.label_4.setMaximumSize(QtCore.QSize(16777215, 27))
        self.label_4.setText("")
        self.label_4.setObjectName("label_4")
        self.gridLayout_5.addWidget(self.label_4, 0, 1, 1, 1)
        self.gridLayout_4.addWidget(self.groupBox_5, 2, 1, 1, 2)
        self.groupBox_6 = QtWidgets.QGroupBox(self.tab)
        self.groupBox_6.setObjectName("groupBox_6")
        self.gridLayout_6 = QtWidgets.QGridLayout(self.groupBox_6)
        self.gridLayout_6.setObjectName("gridLayout_6")
        self.label_5 = QtWidgets.QLabel(self.groupBox_6)
        self.label_5.setObjectName("label_5")
        self.gridLayout_6.addWidget(self.label_5, 0, 0, 1, 1)
        self.label_6 = QtWidgets.QLabel(self.groupBox_6)
        self.label_6.setText("")
        self.label_6.setObjectName("label_6")
        self.gridLayout_6.addWidget(self.label_6, 0, 1, 1, 1)
        self.pushButton_9 = QtWidgets.QPushButton(self.groupBox_6)
        self.pushButton_9.setMaximumSize(QtCore.QSize(400, 16777215))
        self.pushButton_9.setObjectName("pushButton_9")
        self.gridLayout_6.addWidget(self.pushButton_9, 1, 0, 1, 2)
        self.gridLayout_4.addWidget(self.groupBox_6, 1, 1, 1, 1)
        self.tabWidget.addTab(self.tab, "")
        self.tab_2 = QtWidgets.QWidget()
        self.tab_2.setObjectName("tab_2")
        self.gridLayout_3 = QtWidgets.QGridLayout(self.tab_2)
        self.gridLayout_3.setObjectName("gridLayout_3")
        self.pushButton_6 = QtWidgets.QPushButton(self.tab_2)
        self.pushButton_6.setObjectName("pushButton_6")
        self.gridLayout_3.addWidget(self.pushButton_6, 4, 2, 1, 1)
        self.pushButton_7 = QtWidgets.QPushButton(self.tab_2)
        self.pushButton_7.setObjectName("pushButton_7")
        self.gridLayout_3.addWidget(self.pushButton_7, 2, 0, 1, 3)
        self.groupBox_2 = QtWidgets.QGroupBox(self.tab_2)
        self.groupBox_2.setObjectName("groupBox_2")
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout(self.groupBox_2)
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.groupBox_3 = QtWidgets.QGroupBox(self.groupBox_2)
        self.groupBox_3.setMaximumSize(QtCore.QSize(300, 16777215))
        self.groupBox_3.setObjectName("groupBox_3")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.groupBox_3)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.checkBox = QtWidgets.QCheckBox(self.groupBox_3)
        self.checkBox.setMaximumSize(QtCore.QSize(17, 16777215))
        self.checkBox.setText("")
        self.checkBox.setObjectName("checkBox")
        self.horizontalLayout.addWidget(self.checkBox)
        self.lineEdit_3 = QtWidgets.QLineEdit(self.groupBox_3)
        self.lineEdit_3.setObjectName("lineEdit_3")
        self.horizontalLayout.addWidget(self.lineEdit_3)
        self.horizontalLayout_3.addWidget(self.groupBox_3)
        self.groupBox_4 = QtWidgets.QGroupBox(self.groupBox_2)
        self.groupBox_4.setMaximumSize(QtCore.QSize(350, 16777215))
        self.groupBox_4.setObjectName("groupBox_4")
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout(self.groupBox_4)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.checkBox_2 = QtWidgets.QCheckBox(self.groupBox_4)
        self.checkBox_2.setMaximumSize(QtCore.QSize(17, 16777215))
        self.checkBox_2.setText("")
        self.checkBox_2.setObjectName("checkBox_2")
        self.horizontalLayout_2.addWidget(self.checkBox_2)
        self.lineEdit_4 = QtWidgets.QLineEdit(self.groupBox_4)
        self.lineEdit_4.setObjectName("lineEdit_4")
        self.horizontalLayout_2.addWidget(self.lineEdit_4)
        self.horizontalLayout_3.addWidget(self.groupBox_4)
        self.gridLayout_3.addWidget(self.groupBox_2, 3, 0, 1, 3)
        self.pushButton_5 = QtWidgets.QPushButton(self.tab_2)
        self.pushButton_5.setObjectName("pushButton_5")
        self.gridLayout_3.addWidget(self.pushButton_5, 4, 0, 1, 1)
        self.tableWidget = QtWidgets.QTableWidget(self.tab_2)
        self.tableWidget.setEditTriggers(QtWidgets.QAbstractItemView.NoEditTriggers)
        self.tableWidget.setSelectionMode(QtWidgets.QAbstractItemView.SingleSelection)
        self.tableWidget.setSelectionBehavior(QtWidgets.QAbstractItemView.SelectRows)
        self.tableWidget.setObjectName("tableWidget")
        self.tableWidget.setColumnCount(0)
        self.tableWidget.setRowCount(0)
        self.gridLayout_3.addWidget(self.tableWidget, 1, 0, 1, 3)
        self.pushButton_10 = QtWidgets.QPushButton(self.tab_2)
        self.pushButton_10.setObjectName("pushButton_10")
        self.gridLayout_3.addWidget(self.pushButton_10, 0, 0, 1, 3)
        self.tabWidget.addTab(self.tab_2, "")
        self.tab_3 = QtWidgets.QWidget()
        self.tab_3.setObjectName("tab_3")
        self.gridLayout_2 = QtWidgets.QGridLayout(self.tab_3)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.textEdit = QtWidgets.QTextEdit(self.tab_3)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.textEdit.setFont(font)
        self.textEdit.setObjectName("textEdit")
        self.gridLayout_2.addWidget(self.textEdit, 1, 0, 1, 3)
        self.pushButton = QtWidgets.QPushButton(self.tab_3)
        self.pushButton.setObjectName("pushButton")
        self.gridLayout_2.addWidget(self.pushButton, 2, 0, 1, 1)
        self.pushButton_2 = QtWidgets.QPushButton(self.tab_3)
        self.pushButton_2.setObjectName("pushButton_2")
        self.gridLayout_2.addWidget(self.pushButton_2, 2, 2, 1, 1)
        self.label_7 = QtWidgets.QLabel(self.tab_3)
        self.label_7.setObjectName("label_7")
        self.gridLayout_2.addWidget(self.label_7, 0, 0, 1, 1)
        self.label_8 = QtWidgets.QLabel(self.tab_3)
        self.label_8.setText("")
        self.label_8.setObjectName("label_8")
        self.gridLayout_2.addWidget(self.label_8, 0, 1, 1, 2)
        self.tabWidget.addTab(self.tab_3, "")
        self.gridLayout.addWidget(self.tabWidget, 0, 0, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 730, 26))
        self.menubar.setObjectName("menubar")
        self.menu = QtWidgets.QMenu(self.menubar)
        self.menu.setObjectName("menu")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.action = QtWidgets.QAction(MainWindow)
        self.action.setObjectName("action")
        self.action_2 = QtWidgets.QAction(MainWindow)
        self.action_2.setObjectName("action_2")
        self.menu.addAction(self.action)
        self.menu.addAction(self.action_2)
        self.menubar.addAction(self.menu.menuAction())

        self.retranslateUi(MainWindow)
        self.tabWidget.setCurrentIndex(1)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.treeWidget.headerItem().setText(0, _translate("MainWindow", "Агент"))
        self.treeWidget.headerItem().setText(1, _translate("MainWindow", "Директория"))
        self.treeWidget.headerItem().setText(2, _translate("MainWindow", "Файл конфигурации"))
        self.pushButton_4.setText(_translate("MainWindow", "Запросить файл"))
        self.groupBox.setTitle(_translate("MainWindow", "Добавление агента:"))
        self.label.setText(_translate("MainWindow", "IP адрес"))
        self.lineEdit.setText(_translate("MainWindow", "127.0.0.1"))
        self.label_2.setText(_translate("MainWindow", "Порт"))
        self.lineEdit_2.setText(_translate("MainWindow", "55555"))
        self.pushButton_3.setText(_translate("MainWindow", "Подключить агента"))
        self.groupBox_5.setTitle(_translate("MainWindow", "Загрузка конфигурационного файла:"))
        self.pushButton_8.setText(_translate("MainWindow", "Загрузить конфигурационный файл"))
        self.label_3.setText(_translate("MainWindow", "Агент:"))
        self.groupBox_6.setTitle(_translate("MainWindow", "Установка конфигурации:"))
        self.label_5.setText(_translate("MainWindow", "Файл:"))
        self.pushButton_9.setText(_translate("MainWindow", "Сделать конфигурацию активной"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), _translate("MainWindow", "Работа с агентами"))
        self.pushButton_6.setText(_translate("MainWindow", "Сбросить"))
        self.pushButton_7.setText(_translate("MainWindow", "Открыть"))
        self.groupBox_2.setTitle(_translate("MainWindow", "Фильтрация:"))
        self.groupBox_3.setTitle(_translate("MainWindow", "По агенту"))
        self.groupBox_4.setTitle(_translate("MainWindow", "По названию файла"))
        self.pushButton_5.setText(_translate("MainWindow", "Отобразить"))
        self.pushButton_10.setText(_translate("MainWindow", "Вывести таблицу в Excel"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), _translate("MainWindow", "Просмотр данных в БД"))
        self.pushButton.setText(_translate("MainWindow", "Открыть"))
        self.pushButton_2.setText(_translate("MainWindow", "Сохранить"))
        self.label_7.setText(_translate("MainWindow", "Текущая конфигурация:"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_3), _translate("MainWindow", "Редактирование конфига"))
        self.menu.setTitle(_translate("MainWindow", "Файл"))
        self.action.setText(_translate("MainWindow", "Открыть базу данных"))
        self.action.setShortcut(_translate("MainWindow", "Ctrl+O"))
        self.action_2.setText(_translate("MainWindow", "Создать базу данных"))
        self.action_2.setShortcut(_translate("MainWindow", "Ctrl+N"))
