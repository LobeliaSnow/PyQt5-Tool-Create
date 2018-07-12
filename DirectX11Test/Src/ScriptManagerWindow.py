# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\2160149\Desktop\PyQt5\DirectX11Test\ScriptManagerWindow.ui'
#
# Created by: PyQt5 UI code generator 5.10.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_ScriptManagerForm(object):
    def setupUi(self, ScriptManagerForm):
        ScriptManagerForm.setObjectName("ScriptManagerForm")
        ScriptManagerForm.resize(400, 300)
        self.gridLayout = QtWidgets.QGridLayout(ScriptManagerForm)
        self.gridLayout.setObjectName("gridLayout")
        self.pushButton = QtWidgets.QPushButton(ScriptManagerForm)
        self.pushButton.setObjectName("pushButton")
        self.gridLayout.addWidget(self.pushButton, 2, 0, 1, 1)
        self.pushButton_2 = QtWidgets.QPushButton(ScriptManagerForm)
        self.pushButton_2.setObjectName("pushButton_2")
        self.gridLayout.addWidget(self.pushButton_2, 2, 1, 1, 1)
        self.treeWidget = QtWidgets.QTreeWidget(ScriptManagerForm)
        self.treeWidget.setObjectName("treeWidget")
        self.treeWidget.headerItem().setTextAlignment(0, QtCore.Qt.AlignLeading|QtCore.Qt.AlignVCenter)
        self.gridLayout.addWidget(self.treeWidget, 1, 0, 1, 2)

        self.retranslateUi(ScriptManagerForm)
        QtCore.QMetaObject.connectSlotsByName(ScriptManagerForm)

    def retranslateUi(self, ScriptManagerForm):
        _translate = QtCore.QCoreApplication.translate
        ScriptManagerForm.setWindowTitle(_translate("ScriptManagerForm", "ScriptManager"))
        self.pushButton.setText(_translate("ScriptManagerForm", "Add Script"))
        self.pushButton_2.setText(_translate("ScriptManagerForm", "Load Select Script"))
        self.treeWidget.headerItem().setText(0, _translate("ScriptManagerForm", "スクリプト名"))
        self.treeWidget.headerItem().setText(1, _translate("ScriptManagerForm", "ロード"))
        self.treeWidget.headerItem().setText(2, _translate("ScriptManagerForm", "自動ロード"))

