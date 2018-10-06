from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
import Plugin.Module.ParticleConsole
import directxwidget
import DirectX11
import ScriptModule

# テクスチャのプレビューウインドウも付けたい

# パーティクルシステムのパラメーター用インスタンス
# 値のコピーとインスタンスの削除ができるように


class ParticleInstance(QtCore.QAbstractListModel):
    def __init__(self, parent):
        super(ParticleInstance, self).__init__(parent)
        #パーティクルパラメーター
        self.particleParameter = DirectX11.GPUParticle()
        #内部用
        self.particleParameter.elapsedTime = 0.0
        #生成用パラメーター
        self.generateCount = 0
        self.sectionTime = 0.0
        self.blendMode = DirectX11.BlendMode.COPY
        self.randPos = [0.0,0.0,0.0]
        self.randMove = [0.0,0.0,0.0]
        self.randPower = [0.0,0.0,0.0]
        self.randColor = [0.0,0.0,0.0]

class ParticleEditor(directxwidget.DirectXObject, QtWidgets.QWidget):
    windowMenu = None

    def __init__(self, parent):
        super(ParticleEditor, self).__init__(parent)
        self.ui = Plugin.Module.ParticleConsole.Ui_Form()
        self.ui.setupUi(self)
        self.setWindowFlags(Qt.Window)
        self.particleSystemCopy = DirectX11.GPUParticleSystem()
        self.particleSystemAdd = DirectX11.GPUParticleSystem()
        self.particleSystemSub = DirectX11.GPUParticleSystem()
        self.particleSystemScreen = DirectX11.GPUParticleSystem()
        self.Connect()
        # 現在読み込まれるテクスチャスロット
        self.nowTextureSlot = -1

    def Connect(self):
        self.ui.openTexture0.clicked.connect(self.OpenTexture0)
        self.ui.openTexture1.clicked.connect(self.OpenTexture1)
        self.ui.openTexture2.clicked.connect(self.OpenTexture2)
        self.ui.openTexture3.clicked.connect(self.OpenTexture3)
        self.ui.openTexture4.clicked.connect(self.OpenTexture4)
        self.ui.openTexture5.clicked.connect(self.OpenTexture5)
        self.ui.openTexture6.clicked.connect(self.OpenTexture6)
        self.ui.openTexture7.clicked.connect(self.OpenTexture7)
    # テクスチャ読み込み

    def OpenTexture0(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        self.particleSystemCopy.LoadTexture(0, path[0])
        self.particleSystemAdd.LoadTexture(0, path[0])
        self.particleSystemSub.LoadTexture(0, path[0])
        self.particleSystemScreen.LoadTexture(0, path[0])
        self.ui.pathView0.setText(path[0])

    def OpenTexture1(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        self.particleSystemCopy.LoadTexture(1, path[0])
        self.particleSystemAdd.LoadTexture(1, path[0])
        self.particleSystemSub.LoadTexture(1, path[0])
        self.particleSystemScreen.LoadTexture(1, path[0])
        self.ui.pathView1.setText(path[0])

    def OpenTexture2(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        self.particleSystemCopy.LoadTexture(2, path[0])
        self.particleSystemAdd.LoadTexture(2, path[0])
        self.particleSystemSub.LoadTexture(2, path[0])
        self.particleSystemScreen.LoadTexture(2, path[0])
        self.ui.pathView2.setText(path[0])

    def OpenTexture3(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        self.particleSystemCopy.LoadTexture(3, path[0])
        self.particleSystemAdd.LoadTexture(3, path[0])
        self.particleSystemSub.LoadTexture(3, path[0])
        self.particleSystemScreen.LoadTexture(3, path[0])
        self.ui.pathView3.setText(path[0])

    def OpenTexture4(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        self.particleSystemCopy.LoadTexture(4, path[0])
        self.particleSystemAdd.LoadTexture(4, path[0])
        self.particleSystemSub.LoadTexture(4, path[0])
        self.particleSystemScreen.LoadTexture(4, path[0])
        self.ui.pathView4.setText(path[0])

    def OpenTexture5(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        self.particleSystemCopy.LoadTexture(5, path[0])
        self.particleSystemAdd.LoadTexture(5, path[0])
        self.particleSystemSub.LoadTexture(5, path[0])
        self.particleSystemScreen.LoadTexture(5, path[0])
        self.ui.pathView5.setText(path[0])

    def OpenTexture6(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        self.particleSystemCopy.LoadTexture(6, path[0])
        self.particleSystemAdd.LoadTexture(6, path[0])
        self.particleSystemSub.LoadTexture(6, path[0])
        self.particleSystemScreen.LoadTexture(6, path[0])
        self.ui.pathView6.setText(path[0])

    def OpenTexture7(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        self.particleSystemCopy.LoadTexture(7, path[0])
        self.particleSystemAdd.LoadTexture(7, path[0])
        self.particleSystemSub.LoadTexture(7, path[0])
        self.particleSystemScreen.LoadTexture(7, path[0])
        self.ui.pathView7.setText(path[0])
     #

    def RegisterMenuAction(self, parent):
        action = QtWidgets.QAction(parent)
        action.setObjectName("Particle Editor")
        ParticleEditor.windowMenu = parent.AddMenu("&Particle")
        _translate = QtCore.QCoreApplication.translate
        action.setText(_translate("MainWindow", "&Particle Editorを起動"))
        action.triggered.connect(self.Show)
        ParticleEditor.windowMenu.addAction(action)
        parent.ui.menubar.addAction(ParticleEditor.windowMenu.menuAction())

    def Show(self):
        self.setWindowState(Qt.WindowActive)
        self.show()

    def Update(self):
        pass

    def Render(self):
        pass


def Initialize(parent):
    if ParticleEditor.windowMenu != None:
        return
    editor = ParticleEditor(parent)
    editor.RegisterMenuAction(parent)
