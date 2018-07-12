# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\2160149\Desktop\PyQt5\DirectX11Test\Plugin\Module\testWidget.ui'
#
# Created by: PyQt5 UI code generator 5.10.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_testWidget(object):
    def setupUi(self, testWidget):
        testWidget.setObjectName("testWidget")
        testWidget.resize(400, 300)
        self.formLayout = QtWidgets.QFormLayout(testWidget)
        self.formLayout.setObjectName("formLayout")
        self.checkBox = QtWidgets.QCheckBox(testWidget)
        self.checkBox.setObjectName("checkBox")
        self.formLayout.setWidget(0, QtWidgets.QFormLayout.LabelRole, self.checkBox)
        self.pushButton = QtWidgets.QPushButton(testWidget)
        self.pushButton.setObjectName("pushButton")
        self.formLayout.setWidget(1, QtWidgets.QFormLayout.LabelRole, self.pushButton)

        self.retranslateUi(testWidget)
        QtCore.QMetaObject.connectSlotsByName(testWidget)

    def retranslateUi(self, testWidget):
        _translate = QtCore.QCoreApplication.translate
        testWidget.setWindowTitle(_translate("testWidget", "testWidget"))
        self.checkBox.setText(_translate("testWidget", "CheckBox"))
        self.pushButton.setText(_translate("testWidget", "PushButton"))

