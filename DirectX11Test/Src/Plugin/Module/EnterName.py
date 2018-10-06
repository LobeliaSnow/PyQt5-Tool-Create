# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\2160149\Desktop\PyQt5\DirectX11Test\Src\Plugin\Module\EnterName.ui'
#
# Created by: PyQt5 UI code generator 5.10.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_EnterNameDialog(object):
    def setupUi(self, EnterNameDialog):
        EnterNameDialog.setObjectName("EnterNameDialog")
        EnterNameDialog.resize(331, 85)
        self.gridLayout = QtWidgets.QGridLayout(EnterNameDialog)
        self.gridLayout.setObjectName("gridLayout")
        self.label = QtWidgets.QLabel(EnterNameDialog)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.lineEdit = QtWidgets.QLineEdit(EnterNameDialog)
        self.lineEdit.setObjectName("lineEdit")
        self.gridLayout.addWidget(self.lineEdit, 1, 0, 1, 1)
        self.pushButton = QtWidgets.QPushButton(EnterNameDialog)
        self.pushButton.setObjectName("pushButton")
        self.gridLayout.addWidget(self.pushButton, 2, 0, 1, 1)

        self.retranslateUi(EnterNameDialog)
        QtCore.QMetaObject.connectSlotsByName(EnterNameDialog)

    def retranslateUi(self, EnterNameDialog):
        _translate = QtCore.QCoreApplication.translate
        EnterNameDialog.setWindowTitle(_translate("EnterNameDialog", "Enter Name"))
        self.label.setText(_translate("EnterNameDialog", "instance name"))
        self.pushButton.setText(_translate("EnterNameDialog", "Create"))

