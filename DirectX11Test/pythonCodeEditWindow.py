# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\2160149\Desktop\PyQt5\DirectX11Test\pythonCodeEditWindow.ui'
#
# Created by: PyQt5 UI code generator 5.10.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_python_code_editor(object):
    def setupUi(self, python_code_editor):
        python_code_editor.setObjectName("python_code_editor")
        python_code_editor.resize(999, 719)
        self.gridLayout = QtWidgets.QGridLayout(python_code_editor)
        self.gridLayout.setObjectName("gridLayout")
        self.title1 = QtWidgets.QLabel(python_code_editor)
        self.title1.setObjectName("title1")
        self.gridLayout.addWidget(self.title1, 0, 1, 1, 1)
        self.text_browser = QtWidgets.QTextBrowser(python_code_editor)
        self.text_browser.setObjectName("text_browser")
        self.gridLayout.addWidget(self.text_browser, 1, 1, 1, 1)
        self.title0 = QtWidgets.QLabel(python_code_editor)
        self.title0.setObjectName("title0")
        self.gridLayout.addWidget(self.title0, 0, 0, 1, 1)
        self.text_edit = QtWidgets.QTextEdit(python_code_editor)
        self.text_edit.setObjectName("text_edit")
        self.gridLayout.addWidget(self.text_edit, 1, 0, 1, 1)
        self.execute_button = QtWidgets.QPushButton(python_code_editor)
        self.execute_button.setObjectName("execute_button")
        self.gridLayout.addWidget(self.execute_button, 2, 0, 1, 1)
        self.save_button = QtWidgets.QPushButton(python_code_editor)
        self.save_button.setObjectName("save_button")
        self.gridLayout.addWidget(self.save_button, 2, 1, 1, 1)

        self.retranslateUi(python_code_editor)
        QtCore.QMetaObject.connectSlotsByName(python_code_editor)

    def retranslateUi(self, python_code_editor):
        _translate = QtCore.QCoreApplication.translate
        python_code_editor.setWindowTitle(_translate("python_code_editor", "Python Code Editor"))
        self.title1.setText(_translate("python_code_editor", "output window"))
        self.title0.setText(_translate("python_code_editor", "code editor"))
        self.execute_button.setText(_translate("python_code_editor", "execute"))
        self.save_button.setText(_translate("python_code_editor", "save"))

