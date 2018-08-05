# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\2160149\Desktop\PyQt5\DirectX11Test\Src\Plugin\Module\jsonViewerWidget.ui'
#
# Created by: PyQt5 UI code generator 5.10.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_testWidget(object):
    def setupUi(self, testWidget):
        testWidget.setObjectName("testWidget")
        testWidget.resize(400, 314)
        self.gridLayout = QtWidgets.QGridLayout(testWidget)
        self.gridLayout.setObjectName("gridLayout")
        self.treeWidget = QtWidgets.QTreeWidget(testWidget)
        self.treeWidget.setObjectName("treeWidget")
        self.treeWidget.headerItem().setText(0, "Json")
        self.gridLayout.addWidget(self.treeWidget, 0, 0, 1, 1)
        self.pushButton = QtWidgets.QPushButton(testWidget)
        self.pushButton.setObjectName("pushButton")
        self.gridLayout.addWidget(self.pushButton, 1, 0, 1, 1)

        self.retranslateUi(testWidget)
        QtCore.QMetaObject.connectSlotsByName(testWidget)

    def retranslateUi(self, testWidget):
        _translate = QtCore.QCoreApplication.translate
        testWidget.setWindowTitle(_translate("testWidget", "Json Viewer"))
        self.pushButton.setText(_translate("testWidget", "Jsonを開く"))

