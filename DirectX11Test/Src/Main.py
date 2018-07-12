import sys
import os
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

from Window import Ui_MainWindow
from pythoneditortrigger import *
from StdOutputHooker import *
import DirectX11
from ScriptManager import *
from ScriptModule import *

# todo : クラス間連携の取り方を模索すること
# todo : PyQtのウインドウのメニューを動的に増減するやり方検索
# todo : Jsonについて調べる&Jsonをバイナリに変換するプログラム作成
#        具体的には、Jsonのヘッダー情報の順番を外部ファイルから定義して
#        それをもとにバイナリファイル作成するプログラム

#Qt URL List
#いろいろ
#https://qiita.com/dais_k/items/c3682818b28801585ad9
#ツールバー
#http://see-ku.com/wiki/wiki.cgi?page=Qt%A4%CEGUI%A5%E1%A5%E2%A1%CA%B1%FE%CD%D1%CA%D4%A1%CB
#QListWidgetItem
#https://qiita.com/inon3135/items/35ec7c897d0e194040f5
#https://qiita.com/tatsuteb/items/959700c1d7b42c7c8b5a
# from threading import Lock

# class Singleton:
#     uniqueInstance = None
#     lock = Lock()  # クラスロック
#     @staticmethod
#     def GetInstance(cls):
#         if not cls.uniqueInstance:
#             cls.uniqueInstance = cls()
#         return cls.uniqueInstance

class MainWindow(QMainWindow):
    def __init__(self, parent = None):
        super(MainWindow, self).__init__(parent)
        DirectX11.Device.Create(0)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.outputBrowser = StdOutputHooker()
        #必須!デフォルトのQtのGLを切る
        self.ui.directX11Widget.setAttribute(Qt.WA_PaintOnScreen)
        self.ui.directX11Widget.setAttribute(Qt.WA_NativeWindow)
        #ScriptManagerウインドウのインスタンス生成
        self.ScriptManagerWindow = ScriptManager(self)
        self.ScriptManagerWindow.GetScriptList()
        #非表示にする
        self.ScriptManagerWindow.hide()
        #この先各イベント設定
        self.ui.execute_button.clicked.connect(self.ScriptExecute)
        self.ui.actionScriptManager.triggered.connect(self.OpenScriptManager)
        self.menuEnum = {"Script":1}
        
    def __del__(self):
        DirectX11.Device.Destroy()

    def AddMenu(self,menu_name):
        if menu_name in self.menuEnum:
            return
        self.menuEnum[menu_name] = 1
        menu = QtWidgets.QMenu(self.ui.menubar)
        menu.setObjectName(menu_name)
        _translate = QtCore.QCoreApplication.translate
        menu.setTitle(_translate("MainWindow", "&yahoo"))
        return menu

    def GetMenuEnum(self):
        return self.menuEnum

    #コマンド ScriptManagerを起動に接続されている
    def OpenScriptManager(self):
        self.ScriptManagerWindow.Show(self)

    def ScriptExecute(self):
        self.ui.lineEdit_2.setText(ExecutePythonCode(self.ui.python_editor.text()))
        self.ui.python_editor.setText("")

def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()