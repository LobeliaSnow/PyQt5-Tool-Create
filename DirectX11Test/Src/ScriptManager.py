from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from ScriptManagerWindow import *
import os
import glob
import importlib
import Main
from Window import Ui_MainWindow
import json
import ScriptModule

#とりあえずは一つのフォルダにあるpythonファイルの列挙をする
class ScriptManager(QWidget):
    def __init__(self, parent = None):
        super(ScriptManager,self).__init__(parent)
        self.ui = Ui_ScriptManagerForm()
        self.ui.setupUi(self)
        self.setWindowFlags(Qt.Window)
        self.jfile = open("Plugin/list.json","r")
        self.scriptListFile = json.load(self.jfile)
        #スクリプト管理用
        self.scriptSerachList = []
        self.scriptNameList = []
        self.scriptLoadNameList = []
        self.scriptModuleList = {}
        self.treeItemList = []
        #連結
        self.ui.pushButton_2.clicked.connect(self.Update)
        # self.ui.pushButton.clicked.connect(self.AddScript)
        # self.first = True
        self.ScriptListUpdate()
        self.hide()

    def __del__(self):
        # self.SavePathList()
        pass
    def Initialize(self,parent):
        self.parent = parent
        self.Update()
        
    #参考サイト
    #http://note.mokuzine.net/python-dynamic-loader/
    #todo : ここはロード等のフラグを確認して読むこと
    def Update(self):
        index = 0
        for item in self.treeItemList:
            #ロードにチェックが入っているかどうか
            #todo : どこかにそのスクリプトが起動するためのショートカットを置く必要がある
            fileName = self.scriptNameList[index]
            #モジュールのロード
            if item.checkState(1) == Qt.Checked:
                #保存
                root, ext = os.path.splitext(fileName)
                self.scriptModuleList = {fileName : importlib.import_module(root)}
                #拡張追加でツールが落ちないように
                try:
                    self.scriptModuleList[fileName].Initialize(self.parent)   
                except:
                    import traceback
                    traceback.print_exc()
            index += 1
            self.scriptListFile[fileName] = item.checkState(2)
        self.jfile.close()
        with open("Plugin/list.json","w") as self.jfile :
            json.dump(self.scriptListFile,self.jfile)
        self.jfile = open("Plugin/list.json","r")

    def Show(self,parent=None):
        self.parent = parent
        self.show()
        self.setWindowState(Qt.WindowActive)
        
    def ScriptListUpdate(self):
        self.ParseScriptList()
        index = 0
        self.ui.treeWidget.clear()
        self.treeItemList.clear()
        for path in self.scriptNameList:
            item = QTreeWidgetItem(self.ui.treeWidget)
            item.setText(0,path)
            item.setText(1,"ロード")
            item.setText(2,"自動ロード")
            #todo : ここのステートはファイルから別途読み込むこと
            #自動ロード云々のみ読み込む、自動ロードがオンのものは、ロードもオンになる
            #https://doc.qt.io/archives/qtjambi-4.5.2_01/com/trolltech/qt/core/Qt.CheckState.html
            #自動ロード
            # item.setCheckState(1,Qt.Unchecked)
            # item.setCheckState(2,Qt.Unchecked)
            # if self.first:
            if len(self.scriptListFile) and path in self.scriptListFile and self.scriptListFile[path] != 0:
                item.setCheckState(1,Qt.Checked)
                item.setCheckState(2,Qt.Checked)
            else:
                item.setCheckState(1,Qt.Unchecked)
                item.setCheckState(2,Qt.Unchecked)
            self.treeItemList += [item]
            index += 1       
        # self.first = False
        
    #スクリプトが格納されているフォルダ&ファイル名を代入
    def CreatePathList(self):
        #todo : ここもファイルから別途読み込むこと
        self.scriptSerachList.clear()
        with open("Plugin/pathList.dat","r") as pathList:
            for line in pathList:            
                line = line.strip()
                self.scriptSerachList.append(line)
                self.scriptLoadNameList.append(line)
        # self.scriptSerachList = ["Plugin."]
    #パスリストの保存    
    def SavePathList(self):
        with open("Plugin/pathList.dat","w") as pathList:
            for line in self.scriptLoadNameList:
                pathList.writelines(line + "\n")

    # def AddScript(self):
    #     path = ScriptModule.OpenFileDialog()
    #     if path[0] != "":
    #         self.scriptLoadNameList.append(path[0])
    #         self.scriptSerachList.append(path[0])
    #         item = QTreeWidgetItem(self.ui.treeWidget)
    #         item.setText(0,path[0])
    #         item.setText(1,"ロード")
    #         item.setText(2,"自動ロード")
    #         self.treeItemList += [item]
            # self.Update()      
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
        self.scriptNameList.clear()
        #パスリスト作成
        self.CreatePathList()
        for path in self.scriptSerachList:
            self.PathAnalyzer(path)

    def GetScriptList(self):
        return self.scriptNameList             