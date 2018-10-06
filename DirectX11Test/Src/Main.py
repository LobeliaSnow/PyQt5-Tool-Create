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
import random

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

#個人製作 第一弾

class MainWindow(QMainWindow):
    def __init__(self, parent = None):
        super(MainWindow, self).__init__(parent)
        DirectX11.Device.Create(0)
        DirectX11.DirectInput.Initialize()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.outputBrowser = StdOutputHooker()
        #必須!デフォルトのQtのGLを切る
        self.ui.directX11Widget.setAttribute(Qt.WA_PaintOnScreen)
        self.ui.directX11Widget.setAttribute(Qt.WA_NativeWindow)
        #この先各イベント設定
        self.ui.execute_button.clicked.connect(self.ScriptExecute)
        self.ui.actionScriptManager.triggered.connect(self.OpenScriptManager)
        self.menuEnum = {"Script":1}
        # object2d = DX2DObject("test.jpg")
        # object2d.pos = [-0.5,0.5]
        # object2d.scale = [1.0,-1.0]
        # directxwidget.DirectXWidget.renderList += [object2d]
        DirectX11.Mouse.Initialize(self.winId().ascapsule(),True,False)
        #ScriptManagerウインドウのインスタンス生成
        self.ScriptManagerWindow = ScriptManager(self)
        self.ScriptManagerWindow.Initialize(self)
        self.ui.maya_connect.clicked.connect(self.ConnectMaya)
        
    def __del__(self):
        DirectX11.Device.Destroy()

    def ConnectMaya(self):
        print(self.ui.directX11Widget)
        self.ui.directX11Widget.ConnectMaya()
        
    def AddMenu(self,menu_name):
        if menu_name in self.menuEnum:
            return self.menuEnum[menu_name]
        menu = QtWidgets.QMenu(self.ui.menubar)
        self.menuEnum[menu_name] = menu
        menu.setObjectName(menu_name)
        _translate = QtCore.QCoreApplication.translate
        menu.setTitle(_translate("MainWindow", menu_name))
        return menu

    #その名前のメニューが有り無しのBooleanを返す
    def FindMenu(self,name):
        return name in self.menuEnum
        
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
    try:
        sys.exit(app.exec_())
    except:
        return
        
if __name__ == '__main__':
    main()
