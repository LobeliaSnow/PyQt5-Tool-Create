# import ScriptModule
# ScriptModule.SaveFileDialog()
#動的import
#https://blog.amedama.jp/entry/2016/01/28/000122
import DirectX11
import directxwidget
from Main import MainWindow
from Window import Ui_MainWindow
import Plugin.Module.jsonViewerWidget
import ScriptModule
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
# import pymeshio
from PyQt5 import QtCore, QtGui, QtWidgets
import json
import random
#todo : ウィジェットが追加される際、それを呼び出すためのショートカットの方法を模索

#http://www7a.biglobe.ne.jp/~thor/novel/column/10.html
#http://hareoff.blogspot.com/2013/03/pyqtqtreeviewwidget.html
#http://3plus.hatenablog.com/entry/2014/09/12/222738
#https://github.com/ashwin/json-viewer/blob/master/json_viewer.py
#http://w3log.hatenablog.com/entry/2012/03/13/162805

#Json(dict)をQTreeItemのリストにして返す
def ParseJson(jdict):
    items = []    
    if not isinstance(jdict,dict) and not isinstance(jdict,list):
        item = QtWidgets.QTreeWidgetItem([str(jdict)])
        items.append(item)
        return items    
    keys = jdict.keys()
    for key in keys:
        if not isinstance(jdict[key],dict) and not isinstance(jdict[key],list):
            item = QtWidgets.QTreeWidgetItem([key])
            child = QtWidgets.QTreeWidgetItem([key])
            child.setData(0,Qt.EditRole,str(jdict[key]))
            item.addChild(child)
            items.append(item)
        else:
            item = QtWidgets.QTreeWidgetItem([key])
            items.append(item)
            if isinstance(jdict[key],dict):
                items[-1].addChildren(ParseJson(jdict[key]))
            else:
                for element in jdict[key]:
                    items[-1].addChildren(ParseJson(element))
    return items

# def CreateTreeItems(jdict):
#     keys = jdict.keys()
#     index = 0
#     items = []
#     for key in keys:
#         group = QtWidgets.QTreeWidgetItem([str(index)])
#         items.append(group)
#         item = QtWidgets.QTreeWidgetItem([key])
#         item.addChildren(ParseJson(jdict))
#         group.addChild(item)
#         index += 1
#     return items

class JsonViewerWidget(QWidget):
    def __init__(self, parent = None):
        super(JsonViewerWidget, self).__init__(parent)
        self.ui = Plugin.Module.jsonViewerWidget.Ui_testWidget()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.OpenJsonToDialog)
        #ソートを無効化
        # self.ui.treeWidget.setSortingEnabled(False)
        # jfile = open("test.json")
        # jdict = json.load(jfile)
        # items = ParseJson(jdict)
        # self.ui.treeWidget.addTopLevelItems(items)
        #この先Qt初期化
        menu = parent.AddMenu("&Window")
        if menu == None:
            return
        _translate = QtCore.QCoreApplication.translate
        action = QtWidgets.QAction(parent)
        action.triggered.connect(self.Show)
        action.setText(_translate("MainWindow", "&Json Viewer"))
        menu.addAction(action)
        parent.ui.menubar.addAction(menu.menuAction())
        parent.jsonViewer = self
        # directxwidget.DirectXWidget.renderList += [ScriptModule.DX2DObject()]
        
    def OpenJsonToDialog(self):
        #ext_list="全ての形式(*.*)", filter = "", view_path=""
        filePath = ScriptModule.OpenFileDialog(self,"ファイルを読み込み","Json glTF(*.json *gltf);;全ての形式(*.*)","","")
        if filePath[0] != "":
            self.OpenJsonToFilePath(filePath[0])            
    
    def OpenJsonToFilePath(self, file_path):
        jfile = open(file_path)
        jdict = json.load(jfile)
        self.OpenJsonDict(jdict)

    def OpenJsonDict(self,jdict):
        self.ui.treeWidget.clear()
        items = ParseJson(jdict)
        self.ui.treeWidget.addTopLevelItems(items)

    def Show(self):
        self.setWindowState(Qt.WindowActive)
        self.show()

    
widget = None

def Initialize(parent = None):
    try:
        widget = JsonViewerWidget(parent)
        widget.setWindowFlags(Qt.Window)        
    except:
        import traceback
        traceback.print_exc()

    
    # testMenu = QMenu(menuBar)
    # testMenu.setObjectName("testMenu")
    # testMenu.setTitle(_translate("MainWindow", "&Tsts"))

#todo : 動的実行についてよく調べる
#このサイトすげー使えそう(小並感)
#https://qiita.com/tag1216/items/486ff0737fcb8cd0b7ac
#http://note.mokuzine.net/python-dynamic-loader/
#https://blog.amedama.jp/entry/2016/01/28/000122
#https://qiita.com/ajitama/items/2ab42e4c7354dcd79266
#https://www.yoheim.net/blog.php?q=20171004