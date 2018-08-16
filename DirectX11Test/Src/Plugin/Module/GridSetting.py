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
        GridSetting.resize(400, 166)
        self.formLayout = QtWidgets.QFormLayout(GridSetting)
        self.formLayout.setObjectName("formLayout")
        self.label = QtWidgets.QLabel(GridSetting)
        self.label.setObjectName("label")
        self.formLayout.setWidget(0, QtWidgets.QFormLayout.LabelRole, self.label)
        self.label_2 = QtWidgets.QLabel(GridSetting)
        self.label_2.setObjectName("label_2")
        self.formLayout.setWidget(1, QtWidgets.QFormLayout.LabelRole, self.label_2)
        self.pushButton_2 = QtWidgets.QPushButton(GridSetting)
        self.pushButton_2.setObjectName("pushButton_2")
        self.formLayout.setWidget(1, QtWidgets.QFormLayout.FieldRole, self.pushButton_2)
        self.label_3 = QtWidgets.QLabel(GridSetting)
        self.label_3.setObjectName("label_3")
        self.formLayout.setWidget(2, QtWidgets.QFormLayout.LabelRole, self.label_3)
        self.pushButton_3 = QtWidgets.QPushButton(GridSetting)
        self.pushButton_3.setObjectName("pushButton_3")
        self.formLayout.setWidget(2, QtWidgets.QFormLayout.FieldRole, self.pushButton_3)
        self.label_4 = QtWidgets.QLabel(GridSetting)
        self.label_4.setObjectName("label_4")
        self.formLayout.setWidget(3, QtWidgets.QFormLayout.LabelRole, self.label_4)
        self.pushButton_4 = QtWidgets.QPushButton(GridSetting)
        self.pushButton_4.setObjectName("pushButton_4")
        self.formLayout.setWidget(3, QtWidgets.QFormLayout.FieldRole, self.pushButton_4)
        self.checkBox = QtWidgets.QCheckBox(GridSetting)
        self.checkBox.setObjectName("checkBox")
        self.formLayout.setWidget(5, QtWidgets.QFormLayout.LabelRole, self.checkBox)
        self.pushButton = QtWidgets.QPushButton(GridSetting)
        self.pushButton.setObjectName("pushButton")
        self.formLayout.setWidget(5, QtWidgets.QFormLayout.FieldRole, self.pushButton)
        self.spinBox = QtWidgets.QSpinBox(GridSetting)
        self.spinBox.setMinimum(10)
        self.spinBox.setMaximum(1000)
        self.spinBox.setProperty("value", 100)
        self.spinBox.setObjectName("spinBox")
        self.formLayout.setWidget(0, QtWidgets.QFormLayout.FieldRole, self.spinBox)

        self.retranslateUi(GridSetting)
        QtCore.QMetaObject.connectSlotsByName(GridSetting)

    def retranslateUi(self, GridSetting):
        _translate = QtCore.QCoreApplication.translate
        GridSetting.setWindowTitle(_translate("GridSetting", "Grid Setting"))
        self.label.setText(_translate("GridSetting", "grid scale"))
        self.label_2.setText(_translate("GridSetting", "X color"))
        self.pushButton_2.setText(_translate("GridSetting", "Color Picker"))
        self.label_3.setText(_translate("GridSetting", "Y color"))
        self.pushButton_3.setText(_translate("GridSetting", "Color Picker"))
        self.label_4.setText(_translate("GridSetting", "Z Color"))
        self.pushButton_4.setText(_translate("GridSetting", "Color Picker"))
        self.checkBox.setText(_translate("GridSetting", "自動更新"))
        self.pushButton.setText(_translate("GridSetting", "更新"))

