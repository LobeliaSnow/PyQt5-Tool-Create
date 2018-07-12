# import ScriptModule
# ScriptModule.SaveFileDialog()
#動的import
#https://blog.amedama.jp/entry/2016/01/28/000122
import DirectX11
import directxwidget
from Main import MainWindow
from Window import Ui_MainWindow
import Plugin.Module.testWidget
import ScriptModule
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
# import pymeshio
from PyQt5 import QtCore, QtGui, QtWidgets

#todo : ウィジェットが追加される際、それを呼び出すためのショートカットの方法を模索

class Yeah(directxwidget.RenderObject):
    def __init__(self):
        self.renderer = DirectX11.Polygon2DRenderer(8,2)
        self.renderer.Begin()
        self.renderer.Set(0,[0.0,0.0,0.0,0.5])
        self.renderer.End()
    def Render(self):
        self.renderer.Render(2,DirectX11.PrimitiveTopology.LINE_STRIP)    

class TestWidget(QWidget):
    def __init__(self, parent = None):
        super(TestWidget, self).__init__(parent)
        self.ui = Plugin.Module.testWidget.Ui_testWidget()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.Test)
        #描画オブジェクト追加
        directxwidget.DirectXWidget.renderList += [Yeah()]
        #この先Qt初期化
        menu = parent.AddMenu("yahoo")
        if menu == None:
            return
        _translate = QtCore.QCoreApplication.translate
        action = QtWidgets.QAction(parent)
        action.triggered.connect(self.Show)
        action.setText(_translate("MainWindow", "&Testを起動"))
        menu.addAction(action)
        parent.ui.menubar.addAction(menu.menuAction())

    def Test(self):
        ScriptModule.SaveFileDialog()
    
    def Show(self):
        self.setWindowState(Qt.WindowActive)
        self.show()

    
widget = TestWidget

def Initialize(parent = None):
    try:
        widget = TestWidget(parent)
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