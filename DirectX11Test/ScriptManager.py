from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from ScriptManagerWindow import *
import os
import glob
import importlib
import Main
from Window import Ui_MainWindow

#とりあえずは一つのフォルダにあるpythonファイルの列挙をする
class ScriptManager(QWidget):
    def __init__(self, parent = None):
        super(ScriptManager,self).__init__(parent)
        self.ui = Ui_ScriptManagerForm()
        self.ui.setupUi(self)
        self.setWindowFlags(Qt.Window)
        self.scriptListFile = open("Plugin/list.dat","w")
        #スクリプト管理用
        self.scriptSerachList = []
        self.scriptNameList = []
        self.scriptModuleList = {}
        self.treeItemList = []
        #連結
        self.ui.pushButton_2.clicked.connect(self.ImportScript)

    #参考サイト
    #http://note.mokuzine.net/python-dynamic-loader/
    #todo : ここはロード等のフラグを確認して読むこと
    def ImportScript(self):
        index = 0
        for item in self.treeItemList:
            #ロードにチェックが入っているかどうか
            #todo : どこかにそのスクリプトが起動するためのショートカットを置く必要がある
            if item.checkState(1) == Qt.Checked:
                fileName = self.scriptNameList[index]
                root, ext = os.path.splitext(fileName)
                self.scriptModuleList = {fileName : importlib.import_module(root)}
                self.scriptModuleList[fileName].Initialize(self.parent)   
                index += 1     
        # index = self.ui.treeWidget.currentColumn()
        # if index < 0:
        #     return
        # fileName = self.scriptNameList[index]
        # root, ext = os.path.splitext(fileName)
        # self.scriptModuleList = {fileName : importlib.import_module(root)}
        # self.scriptModuleList[fileName].Initialize()
        
    def Show(self,parent=None):
        self.parent = parent
        self.show()
        self.setWindowState(Qt.WindowActive)
        self.WindowUpdate()
        
    def WindowUpdate(self):
        self.ParseScriptList()
        index = 0
        self.ui.treeWidget.clear()
        self.treeItemList.clear()
        for path in self.scriptNameList :
            item = QTreeWidgetItem(self.ui.treeWidget)
            item.setText(0,path)
            item.setText(1,"ロード")
            item.setText(2,"自動ロード")
            #todo : ここのステートはファイルから別途読み込むこと
            #自動ロード云々のみ読み込む、自動ロードがオンのものは、ロードもオンになる
            #https://doc.qt.io/archives/qtjambi-4.5.2_01/com/trolltech/qt/core/Qt.CheckState.html
            item.setCheckState(1,Qt.Unchecked)
            #自動ロード
            item.setCheckState(2,Qt.Unchecked)
            self.treeItemList += [item]
            index += 1

    #スクリプトが格納されているフォルダ&ファイル名を代入
    def CreatePathList(self):
        #todo : ここもファイルから別途読み込むこと
        self.scriptSerachList.clear()
        self.scriptSerachList = ["Plugin."]

    def AddScript(self):
        # self.ui.listWidget.addItem()
        pass
    def PathAnalyzer(self,path):
        #それがディレクトリだった場合pythonファイル列挙
        if os.path.isdir(path):
            fileList = os.listdir(path)
            pushFileList = []
            for f in fileList:
                pushFileList.append(path + f)
            self.scriptSerachList += pushFileList
        #ファイルだった場合 pythonファイルか確認
        else:
            root, ext = os.path.splitext(path)
            if ext == ".py":
                self.scriptNameList.append(path) 

    def ParseScriptList(self):
        #スクリプト一覧初期化
        self.scriptNameList .clear()
        #パスリスト作成
        self.CreatePathList()
        for path in self.scriptSerachList:
            self.PathAnalyzer(path)

    def GetScriptList(self):
        return self.scriptNameList             