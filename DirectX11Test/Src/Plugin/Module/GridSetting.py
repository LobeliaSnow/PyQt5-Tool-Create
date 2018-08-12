# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\black\Desktop\PyQt5-Tool-Create\DirectX11Test\Src\Plugin\Module\GridSetting.ui'
#
# Created by: PyQt5 UI code generator 5.11.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_GridSetting(object):
    def setupUi(self, GridSetting):
        GridSetting.setObjectName("GridSetting")
        GridSetting.resize(400, 78)
        self.formLayout = QtWidgets.QFormLayout(GridSetting)
        self.formLayout.setObjectName("formLayout")
        self.label = QtWidgets.QLabel(GridSetting)
        self.label.setObjectName("label")
        self.formLayout.setWidget(0, QtWidgets.QFormLayout.LabelRole, self.label)
        self.doubleSpinBox = QtWidgets.QDoubleSpinBox(GridSetting)
        self.doubleSpinBox.setMinimum(10.0)
        self.doubleSpinBox.setMaximum(10000.0)
        self.doubleSpinBox.setProperty("value", 100.0)
        self.doubleSpinBox.setObjectName("doubleSpinBox")
        self.formLayout.setWidget(0, QtWidgets.QFormLayout.FieldRole, self.doubleSpinBox)
        self.pushButton = QtWidgets.QPushButton(GridSetting)
        self.pushButton.setObjectName("pushButton")
        self.formLayout.setWidget(1, QtWidgets.QFormLayout.SpanningRole, self.pushButton)

        self.retranslateUi(GridSetting)
        QtCore.QMetaObject.connectSlotsByName(GridSetting)

    def retranslateUi(self, GridSetting):
        _translate = QtCore.QCoreApplication.translate
        GridSetting.setWindowTitle(_translate("GridSetting", "Grid Setting"))
        self.label.setText(_translate("GridSetting", "grid scale"))
        self.pushButton.setText(_translate("GridSetting", "更新"))

