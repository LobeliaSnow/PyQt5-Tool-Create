# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\black\Desktop\PyQt5-Tool-Create\DirectX11Test\Src\Plugin\Module\GltfConsole.ui'
#
# Created by: PyQt5 UI code generator 5.11.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_GLTFConsole(object):
    def setupUi(self, GLTFConsole):
        GLTFConsole.setObjectName("GLTFConsole")
        GLTFConsole.resize(400, 89)
        self.formLayout = QtWidgets.QFormLayout(GLTFConsole)
        self.formLayout.setObjectName("formLayout")
        self.label = QtWidgets.QLabel(GLTFConsole)
        self.label.setObjectName("label")
        self.formLayout.setWidget(1, QtWidgets.QFormLayout.LabelRole, self.label)
        self.doubleSpinBox = QtWidgets.QDoubleSpinBox(GLTFConsole)
        self.doubleSpinBox.setMinimum(0.1)
        self.doubleSpinBox.setMaximum(1000.0)
        self.doubleSpinBox.setProperty("value", 1.0)
        self.doubleSpinBox.setObjectName("doubleSpinBox")
        self.formLayout.setWidget(1, QtWidgets.QFormLayout.FieldRole, self.doubleSpinBox)
        self.pushButton = QtWidgets.QPushButton(GLTFConsole)
        self.pushButton.setObjectName("pushButton")
        self.formLayout.setWidget(0, QtWidgets.QFormLayout.SpanningRole, self.pushButton)
        self.checkBox = QtWidgets.QCheckBox(GLTFConsole)
        self.checkBox.setObjectName("checkBox")
        self.formLayout.setWidget(2, QtWidgets.QFormLayout.SpanningRole, self.checkBox)

        self.retranslateUi(GLTFConsole)
        QtCore.QMetaObject.connectSlotsByName(GLTFConsole)

    def retranslateUi(self, GLTFConsole):
        _translate = QtCore.QCoreApplication.translate
        GLTFConsole.setWindowTitle(_translate("GLTFConsole", "glTF Console"))
        self.label.setText(_translate("GLTFConsole", "scale"))
        self.pushButton.setText(_translate("GLTFConsole", "glTFを開く"))
        self.checkBox.setText(_translate("GLTFConsole", "wire frame"))

