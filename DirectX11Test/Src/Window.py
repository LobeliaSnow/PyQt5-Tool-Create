# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\2160149\Desktop\PyQt5\DirectX11Test\Src\Window.ui'
#
# Created by: PyQt5 UI code generator 5.10.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(1068, 718)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayout = QtWidgets.QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName("gridLayout")
        self.execute_button = QtWidgets.QPushButton(self.centralwidget)
        self.execute_button.setObjectName("execute_button")
        self.gridLayout.addWidget(self.execute_button, 1, 3, 1, 1)
        self.lineEdit_2 = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit_2.setReadOnly(True)
        self.lineEdit_2.setObjectName("lineEdit_2")
        self.gridLayout.addWidget(self.lineEdit_2, 1, 5, 1, 1)
        self.python_button = PythonEditorTrigger(self.centralwidget)
        self.python_button.setObjectName("python_button")
        self.gridLayout.addWidget(self.python_button, 1, 0, 1, 1)
        self.python_editor = QtWidgets.QLineEdit(self.centralwidget)
        self.python_editor.setObjectName("python_editor")
        self.gridLayout.addWidget(self.python_editor, 1, 1, 1, 1)
        self.output_label = QtWidgets.QLabel(self.centralwidget)
        self.output_label.setObjectName("output_label")
        self.gridLayout.addWidget(self.output_label, 1, 4, 1, 1)
        self.maya_connect = QtWidgets.QPushButton(self.centralwidget)
        self.maya_connect.setObjectName("maya_connect")
        self.gridLayout.addWidget(self.maya_connect, 1, 7, 1, 1)
        self.directX11Widget = DirectXWidget(self.centralwidget)
        self.directX11Widget.setObjectName("directX11Widget")
        self.gridLayout.addWidget(self.directX11Widget, 0, 0, 1, 8)
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 1, 6, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1068, 21))
        self.menubar.setObjectName("menubar")
        self.menuScript = QtWidgets.QMenu(self.menubar)
        self.menuScript.setObjectName("menuScript")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.actionScriptManager = QtWidgets.QAction(MainWindow)
        self.actionScriptManager.setObjectName("actionScriptManager")
        self.menuScript.addAction(self.actionScriptManager)
        self.menubar.addAction(self.menuScript.menuAction())

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.execute_button.setText(_translate("MainWindow", "Execute"))
        self.python_button.setText(_translate("MainWindow", "Python"))
        self.output_label.setText(_translate("MainWindow", "output"))
        self.maya_connect.setText(_translate("MainWindow", "Connect"))
        self.label.setText(_translate("MainWindow", "Mayaと接続"))
        self.menuScript.setTitle(_translate("MainWindow", "&Script"))
        self.actionScriptManager.setText(_translate("MainWindow", "&ScriptManagerを起動"))

from directxwidget import DirectXWidget
from pythoneditortrigger import PythonEditorTrigger
