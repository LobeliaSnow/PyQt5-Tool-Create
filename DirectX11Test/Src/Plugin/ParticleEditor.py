from PyQt5 import QtCore
from PyQt5 import QtWidgets
from PyQt5 import QtGui
import Plugin.Module.ParticleEditorWindow
import directxwidget
import DirectX11
import ScriptModule
import sys
import random
import copy

# テクスチャのプレビューウインドウも付けたい

# パーティクルシステムのパラメーター用インスタンス
# 値のコピーができるように
# 回転のパラメーター忘れてる

#現状インスタンスを移動してから戻るとなぜか消える

# パーティクルコンソールのuiファイルから生成されたpyファイルの
# 一番下のimportを以下のように変更すること
# from Plugin.Module.Instancelist import InstanceList


class ParticleEditor(directxwidget.DirectXObject, QtWidgets.QMainWindow):
    windowMenu = None

    def __init__(self, parent):
        super(ParticleEditor, self).__init__(parent)
        self.ui = Plugin.Module.ParticleEditorWindow.Ui_ParticleEditor()
        self.ui.setupUi(self)
        self.setWindowFlags(QtCore.Qt.Window)
        self.particleSystem = []
        for i in range(4):
            self.particleSystem += [DirectX11.GPUParticleSystem()]
        self.Connect()
        # 現在読み込まれるテクスチャスロット
        self.nowTextureSlot = -1
        self.ui.instanceListWidget.SetParentEditor(self)
        self.DeactivateWidget()
        self.active = False
        self.colorRed = 1.0
        self.colorGreen = 1.0
        self.colorBlue = 1.0

    def Connect(self):
        self.ui.openTexture0.clicked.connect(self.OpenTexture0)
        self.ui.openTexture1.clicked.connect(self.OpenTexture1)
        self.ui.openTexture2.clicked.connect(self.OpenTexture2)
        self.ui.openTexture3.clicked.connect(self.OpenTexture3)
        self.ui.openTexture4.clicked.connect(self.OpenTexture4)
        self.ui.openTexture5.clicked.connect(self.OpenTexture5)
        self.ui.openTexture6.clicked.connect(self.OpenTexture6)
        self.ui.openTexture7.clicked.connect(self.OpenTexture7)
        self.ui.colorPickerButton.clicked.connect(self.ColorPick)

    def DeactivateWidget(self):
        self.ui.generateParameters.setDisabled(True)
        self.ui.particleParameters.setDisabled(True)
        self.ui.activeInstance.setText("")
        self.active = False

    def ActivateWidget(self):
        self.ui.generateParameters.setEnabled(True)
        self.ui.particleParameters.setEnabled(True)
        self.active = True

    # 入力値のリセット
    def ResetGeneratorParameters(self):
        self.ui.spinGenerateFrame.setValue(0)
        self.ui.spinSectionTime.setValue(0.0)
        self.ui.spinRandPosX.setValue(0.0)
        self.ui.spinRandPosY.setValue(0.0)
        self.ui.spinRandPosZ.setValue(0.0)
        self.ui.spinRandMoveX.setValue(0.0)
        self.ui.spinRandMoveY.setValue(0.0)
        self.ui.spinRandMoveZ.setValue(0.0)
        self.ui.spinRandPowerX.setValue(0.0)
        self.ui.spinRandPowerY.setValue(0.0)
        self.ui.spinRandPowerZ.setValue(0.0)
        self.ui.spinRandColorRed.setValue(0.0)
        self.ui.spinRandColorGreen.setValue(0.0)
        self.ui.spinRandColorBlue.setValue(0.0)

    def ResetParticleParameters(self):
        self.ui.spinPosX.setValue(0.0)
        self.ui.spinPosY.setValue(0.0)
        self.ui.spinPosZ.setValue(0.0)
        self.ui.spinMoveX.setValue(0.0)
        self.ui.spinMoveY.setValue(0.0)
        self.ui.spinMoveZ.setValue(0.0)
        self.ui.spinUVPosX.setValue(0.0)
        self.ui.spinUVPosY.setValue(0.0)
        self.ui.spinUVSizeX.setValue(0.0)
        self.ui.spinAliveTime.setValue(0.0)
        self.ui.spinFadeInTime.setValue(0.0)
        self.ui.spinFadeOutTime.setValue(0.0)
        self.ui.spinStartScale.setValue(0.0)
        self.ui.spinEndScale.setValue(0.0)
        self.ui.spinTextureIndex.setValue(-1)
        self.ui.colorRed = 1.0
        self.ui.colorGreen = 1.0
        self.ui.colorBlue = 1.0

    def ResetParameters(self):
        self.ResetGeneratorParameters()
        self.ResetParticleParameters()
        self.ui.blendCopy.setChecked(True)

    def ColorPick(self):
        color = QtWidgets.QColorDialog.getColor()
        self.colorRed = color.redF()
        self.colorGreen = color.greenF()
        self.colorBlue = color.blueF()
        
    # テクスチャ読み込み
    def OpenTexture0(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        for system in self.particleSystem:
            system.LoadTexture(0, path[0])
        self.ui.pathView0.setText(path[0])

    def OpenTexture1(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        for system in self.particleSystem:
            system.LoadTexture(1, path[0])
        self.ui.pathView1.setText(path[0])

    def OpenTexture2(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        for system in self.particleSystem:
            system.LoadTexture(2, path[0])
        self.ui.pathView2.setText(path[0])

    def OpenTexture3(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        for system in self.particleSystem:
            system.LoadTexture(3, path[0])
        self.ui.pathView3.setText(path[0])

    def OpenTexture4(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        for system in self.particleSystem:
            system.LoadTexture(4, path[0])
        self.ui.pathView4.setText(path[0])

    def OpenTexture5(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        for system in self.particleSystem:
            system.LoadTexture(5, path[0])
        self.ui.pathView5.setText(path[0])

    def OpenTexture6(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        for system in self.particleSystem:
            system.LoadTexture(6, path[0])
        self.ui.pathView6.setText(path[0])

    def OpenTexture7(self):
        path = ScriptModule.OpenFileDialog(self)
        if path[0] == "":
            return
        for system in self.particleSystem:
            system.LoadTexture(7, path[0])
        self.ui.pathView7.setText(path[0])

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
        self.setWindowState(QtCore.Qt.WindowActive)
        self.show()
    def SetParticles(self):
        # 各経過時間
        for i in range(self.ui.instanceListWidget.count()):
            instance = self.ui.instanceListWidget.item(i)
            instance.elapsedTime += self.particleSystem[int(instance.blendMode)].GetElapsedTime()
            for generate in range(instance.generateCount):
                # ベースを入れる
                particle = DirectX11.GPUParticle(instance.particleParameter)
                # particle = instance.particleParameter
                particle.aliveTime = particle.elapsedTime
                # ランダム要素反映
                particle.posX += random.uniform(-instance.randPos[0],instance.randPos[0])
                particle.posY += random.uniform(-instance.randPos[1],instance.randPos[1])
                particle.posZ += random.uniform(-instance.randPos[2],instance.randPos[2])
                particle.moveX += random.uniform(-instance.randMove[0],instance.randMove[0])
                particle.moveY += random.uniform(-instance.randMove[1],instance.randMove[1])
                particle.moveZ += random.uniform(-instance.randMove[2],instance.randMove[2])
                particle.powerX += random.uniform(-instance.randPower[0],instance.randPower[0])
                particle.powerY += random.uniform(-instance.randPower[1],instance.randPower[1])
                particle.powerZ += random.uniform(-instance.randPower[2],instance.randPower[2])
                particle.colorRed += random.uniform(-instance.randColor[0], instance.randColor[0])
                if particle.colorRed > 1.0:
                    particle.colorRed = 1.0
                particle.colorGreen += random.uniform(-instance.randColor[1], instance.randColor[1])
                if particle.colorGreen > 1.0:
                    particle.colorGreen = 1.0
                particle.colorBlue += random.uniform(-instance.randColor[2], instance.randColor[2])
                if particle.colorBlue > 1.0:
                    particle.colorBlue = 1.0
                self.particleSystem[int(instance.blendMode)].Append(particle)
    def Update(self):
        if self.ui.instanceListWidget.count() > 0:
            if not self.active:
                self.ActivateWidget()
        else:
            self.DeactivateWidget()
        self.ui.instanceListWidget.Update()
        self.SetParticles()
        for system in self.particleSystem:
            system.Update(1.0)

    def Render(self):
        index = 0
        self.particleSystem[0].Render(DirectX11.ParticleBlendMode(index))
        for system in self.particleSystem:
            system.Render(DirectX11.ParticleBlendMode(index))
            index += 1


def Initialize(parent):
    if ParticleEditor.windowMenu != None:
        return
    editor = ParticleEditor(parent)
    editor.RegisterMenuAction(parent)
    directxwidget.DirectXWidget.objectList += [editor]
    # デバッグ用
    editor.Show()
