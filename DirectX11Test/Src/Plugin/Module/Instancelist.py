from PyQt5 import QtCore
from PyQt5 import QtWidgets
from PyQt5 import QtGui
import DirectX11
from Plugin.Module.EnterName import Ui_EnterNameDialog
import copy

class EnterName(QtWidgets.QDialog):
    def __init__(self, parent):
        super(EnterName, self).__init__(parent)
        self.ui = Ui_EnterNameDialog()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.Close)
        self.ret = ""

    def Close(self):
        self.ret = str(self.ui.lineEdit.text())
        if self.ret == "":
            return
        self.close()

    def Exec(self):
        self.ret = ""
        self.ui.lineEdit.setText("")
        self.exec()
        return self.ret

class ParticleInstance(QtWidgets.QListWidgetItem):
    def __init__(self, text, parent):
        super(ParticleInstance, self).__init__(text, parent)
        # パーティクルパラメーター
        self.particleParameter = DirectX11.GPUParticle()
        # 内部用
        self.particleParameter.elapsedTime = 0.0
        self.elapsedTime = 0.0
        # 生成用パラメーター
        self.generateCount = 0
        self.sectionTime = 0.0
        self.playTime = 0.0
        self.blendMode = DirectX11.ParticleBlendMode.COPY
        self.randPos = [0.0, 0.0, 0.0]
        self.randMove = [0.0, 0.0, 0.0]
        self.randPower = [0.0, 0.0, 0.0]
        self.randColor = [0.0, 0.0, 0.0]

class InstanceList(QtWidgets.QListWidget):
    def __init__(self, *args, **kwargs):
        super(InstanceList, self).__init__(*args, **kwargs)
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.customContextMenuRequested.connect(self.ContextMenu)
        # self.instanceList = []
        # インスタンス名入力用ダイアログ
        self.enterName = EnterName(self)
        self.itemDoubleClicked.connect(self.ChangeInstance)
        self.index = 0
        self.copy = None
        self.copyValue = None
        self.renameInstance = None
        self.deleteInstance = None
        self.pasteValue = None
        self.resetValue = None

    def keyPressEvent(self, event):
        # アイテムの削除
        if event.key() == QtCore.Qt.Key_Delete:
            self.DeleteInstance()
        # アイテムのリネーム
        elif event.key() == QtCore.Qt.Key_F2:
            self.RenameInstance()

    def SetGenerateParameters(self, item):
        self.parent.ui.spinGenerateFrame.setValue(item.generateCount)
        self.parent.ui.spinSectionTime.setValue(item.sectionTime)
        self.parent.ui.spinPlayTime.setValue(item.playTime)
        self.parent.ui.spinRandPosX.setValue(item.randPos[0])
        self.parent.ui.spinRandPosY.setValue(item.randPos[1])
        self.parent.ui.spinRandPosZ.setValue(item.randPos[2])
        self.parent.ui.spinRandMoveX.setValue(item.randMove[0])
        self.parent.ui.spinRandMoveY.setValue(item.randMove[1])
        self.parent.ui.spinRandMoveZ.setValue(item.randMove[2])
        self.parent.ui.spinRandPowerX.setValue(item.randPower[0])
        self.parent.ui.spinRandPowerY.setValue(item.randPower[1])
        self.parent.ui.spinRandPowerZ.setValue(item.randPower[2])
        self.parent.ui.spinRandColorRed.setValue(item.randColor[0])
        self.parent.ui.spinRandColorGreen.setValue(item.randColor[1])
        self.parent.ui.spinRandColorBlue.setValue(item.randColor[2])
        if item.blendMode == DirectX11.ParticleBlendMode.COPY:
            self.parent.ui.blendCopy.setChecked(True)
        if item.blendMode == DirectX11.ParticleBlendMode.ADD:
            self.parent.ui.blendAdd.setChecked(True)
        if item.blendMode == DirectX11.ParticleBlendMode.SUB:
            self.parent.ui.blendSub.setChecked(True)
        if item.blendMode == DirectX11.ParticleBlendMode.SCREEN:
            self.parent.ui.blendScreen.setChecked(True)

    def SetParticleParameters(self, item):
        self.parent.ui.spinPosX.setValue(item.particleParameter.posX)
        self.parent.ui.spinPosY.setValue(item.particleParameter.posY)
        self.parent.ui.spinPosZ.setValue(item.particleParameter.posZ)
        self.parent.ui.spinMoveX.setValue(item.particleParameter.moveX)
        self.parent.ui.spinMoveY.setValue(item.particleParameter.moveY)
        self.parent.ui.spinMoveZ.setValue(item.particleParameter.moveZ)
        self.parent.ui.spinUVPosX.setValue(item.particleParameter.uvPosX)
        self.parent.ui.spinUVPosY.setValue(item.particleParameter.uvPosY)
        self.parent.ui.spinUVSizeX.setValue(item.particleParameter.uvSizeX)
        self.parent.ui.spinUVSizeY.setValue(item.particleParameter.uvSizeY)
        self.parent.ui.spinAliveTime.setValue(item.particleParameter.aliveTime)
        self.parent.ui.spinFadeInTime.setValue(item.particleParameter.fadeInTime)
        self.parent.ui.spinFadeOutTime.setValue(item.particleParameter.fadeOutTime)
        self.parent.ui.spinStartScale.setValue(item.particleParameter.startScale)
        self.parent.ui.spinEndScale.setValue(item.particleParameter.endScale)
        self.parent.ui.spinStartRad.setValue(item.particleParameter.startRad)
        self.parent.ui.spinEndRad.setValue(item.particleParameter.endRad)
        self.parent.ui.spinTextureIndex.setValue(item.particleParameter.textureIndex)
        self.parent.colorRed = item.particleParameter.colorRed
        self.parent.colorGreen = item.particleParameter.colorGreen
        self.parent.colorBlue = item.particleParameter.colorBlue

    def UpdateGenerateParameters(self, item):
        item.generateCount = self.parent.ui.spinGenerateFrame.value()
        item.sectionTime = self.parent.ui.spinSectionTime.value()
        item.playTime = self.parent.ui.spinPlayTime.value()
        item.randPos[0] = self.parent.ui.spinRandPosX.value()
        item.randPos[1] = self.parent.ui.spinRandPosY.value()
        item.randPos[2] = self.parent.ui.spinRandPosZ.value()
        item.randMove[0] = self.parent.ui.spinRandMoveX.value()
        item.randMove[1] = self.parent.ui.spinRandMoveY.value()
        item.randMove[2] = self.parent.ui.spinRandMoveZ.value()
        item.randPower[0] = self.parent.ui.spinRandPowerX.value()
        item.randPower[1] = self.parent.ui.spinRandPowerY.value()
        item.randPower[2] = self.parent.ui.spinRandPowerZ.value()
        item.randColor[0] = self.parent.ui.spinRandColorRed.value()
        item.randColor[1] = self.parent.ui.spinRandColorGreen.value()
        item.randColor[2] = self.parent.ui.spinRandColorBlue.value()

    def UpdateParticleParameters(self, item):
        item.particleParameter.posX = self.parent.ui.spinPosX.value()
        item.particleParameter.posY = self.parent.ui.spinPosY.value()
        item.particleParameter.posZ = self.parent.ui.spinPosZ.value()
        item.particleParameter.moveX = self.parent.ui.spinMoveX.value()
        item.particleParameter.moveY = self.parent.ui.spinMoveY.value()
        item.particleParameter.moveZ = self.parent.ui.spinMoveZ.value()
        item.particleParameter.uvPosX = self.parent.ui.spinUVPosX.value()
        item.particleParameter.uvPosY = self.parent.ui.spinUVPosY.value()
        item.particleParameter.uvSizeX = self.parent.ui.spinUVSizeX.value()
        item.particleParameter.uvSizeY = self.parent.ui.spinUVSizeY.value()
        item.particleParameter.aliveTime = self.parent.ui.spinAliveTime.value()
        item.particleParameter.elapsedTime = self.parent.ui.spinAliveTime.value()
        item.particleParameter.fadeInTime = self.parent.ui.spinFadeInTime.value()
        item.particleParameter.fadeOutTime = self.parent.ui.spinFadeOutTime.value()
        item.particleParameter.startScale = self.parent.ui.spinStartScale.value()
        item.particleParameter.endScale = self.parent.ui.spinEndScale.value()
        item.particleParameter.startRad = self.parent.ui.spinStartRad.value()
        item.particleParameter.endRad = self.parent.ui.spinEndRad.value()
        item.particleParameter.textureIndex = self.parent.ui.spinTextureIndex.value()
        item.particleParameter.colorRed = self.parent.colorRed
        item.particleParameter.colorGreen = self.parent.colorGreen
        item.particleParameter.colorBlue = self.parent.colorBlue
        if self.parent.ui.blendCopy.isChecked():
            item.blendMode = DirectX11.ParticleBlendMode.COPY
        elif self.parent.ui.blendAdd.isChecked():
            item.blendMode = DirectX11.ParticleBlendMode.ADD
        elif self.parent.ui.blendSub.isChecked():
            item.blendMode = DirectX11.ParticleBlendMode.SUB
        elif self.parent.ui.blendScreen.isChecked():
            item.blendMode = DirectX11.ParticleBlendMode.SCREEN

    def ChangeInstance(self, item):
        self.SetGenerateParameters(item)
        self.SetParticleParameters(item)
        self.SetActive()

    def SetActive(self):
        self.index = self.currentRow()
        # self.parent.ui.activeInstance.setText(instance_name)

    def SetParentEditor(self, parent):
        self.parent = parent

    def ContextMenu(self, point):
        menu = QtWidgets.QMenu(self)
        self.createInstance = QtWidgets.QAction("Create Instance", self)
        self.createInstance.triggered.connect(self.CreateInstance)
        menu.addAction(self.createInstance)
        self.renameInstance = QtWidgets.QAction("Rename Instance", self)
        self.renameInstance.triggered.connect(self.RenameInstance)
        self.renameInstance.setDisabled(True)
        menu.addAction(self.renameInstance)
        self.deleteInstance = QtWidgets.QAction("Delete Instance", self)
        self.deleteInstance.triggered.connect(self.DeleteInstance)
        self.deleteInstance.setDisabled(True)
        menu.addAction(self.deleteInstance)
        self.copyValue = QtWidgets.QAction("Copy Value", self)
        self.copyValue.triggered.connect(self.CopyValue)
        self.copyValue.setDisabled(True)
        menu.addAction(self.copyValue)
        self.pasteValue = QtWidgets.QAction("Paste Value", self)
        self.pasteValue.triggered.connect(self.PasteValue)
        self.pasteValue.setDisabled(True)
        menu.addAction(self.pasteValue)
        self.resetValue = QtWidgets.QAction("Reset Value", self)
        self.resetValue.triggered.connect(self.ResetValue)
        self.resetValue.setDisabled(True)
        menu.addAction(self.resetValue)
        menu.exec_(self.mapToGlobal(point))

    def CreateInstance(self):
        instanceName = self.enterName.Exec()
        if instanceName == "":
            return
        instance = ParticleInstance(instanceName, self)
        # self.instanceList += [instance]
        self.addItem(instance)
        self.setCurrentRow(self.count() - 1)
        self.SetActive()
        # self.model.appendRow(instance)
        self.parent.ResetParameters()

    def DeleteInstance(self):
        index = self.index - 1
        if index < 0:
            index = 0
        # 削除
        self.takeItem(self.currentRow())
        if self.count() == 0:
            self.parent.ResetParameters()
            return
        self.ChangeInstance(self.item(index))
        self.setCurrentRow(index)

    def RenameInstance(self):
        item = self.currentItem()
        instanceName = self.enterName.Exec()
        if instanceName == "":
            return
        item.setText(instanceName)

    def CopyValue(self):
        self.copy = self.currentItem()
        self.copyColorRed = self.parent.colorRed
        self.copyColorGreen = self.parent.colorGreen
        self.copyColorBlue = self.parent.colorBlue

    def PasteValue(self):
        if self.copy == None:
            return
        item = self.currentItem()
        #コピーコンストラクタでコピー
        item.particleParameter = DirectX11.GPUParticle(self.copy.particleParameter)
        # 内部用
        item.particleParameter.elapsedTime = copy.copy(self.copy.particleParameter.elapsedTime)
        item.elapsedTime = copy.copy(self.copy.particleParameter.elapsedTime)
        # 生成用パラメーター
        item.generateCount = copy.copy(self.copy.generateCount)
        item.sectionTime = copy.copy(self.copy.sectionTime)
        item.blendMode = copy.copy(self.copy.blendMode)
        item.randPos = copy.copy(self.copy.randPos)
        item.randMove = copy.copy(self.copy.randMove)
        item.randPower = copy.copy(self.copy.randPower)
        item.randColor = copy.copy(self.copy.randColor)
        self.parent.colorRed = copy.copy(self.copyColorRed)
        self.parent.colorGreen = copy.copy(self.copyColorGreen)
        self.parent.colorBlue = copy.copy(self.copyColorBlue)
        self.SetGenerateParameters(item)
        self.SetParticleParameters(item)
        
    def ResetValue(self):
        item = self.currentItem()
        # パーティクルパラメーター
        item.particleParameter = DirectX11.GPUParticle()
        # 内部用
        item.particleParameter.elapsedTime = 0.0
        item.elapsedTime = 0.0
        # 生成用パラメーター
        item.generateCount = 0
        item.sectionTime = 0.0
        item.blendMode = DirectX11.ParticleBlendMode.COPY
        item.randPos = [0.0, 0.0, 0.0]
        item.randMove = [0.0, 0.0, 0.0]
        item.randPower = [0.0, 0.0, 0.0]
        item.randColor = [0.0, 0.0, 0.0]
        self.SetGenerateParameters(item)
        self.SetParticleParameters(item)

    def Update(self):
        if self.count() == 0:
            if self.renameInstance != None:
                self.renameInstance.setDisabled(True)
            if self.deleteInstance != None:
                self.deleteInstance.setDisabled(True)
            if self.copyValue != None:
               self.copyValue.setDisabled(True)
            if self.pasteValue != None:
               self.pasteValue.setDisabled(True)
            if self.resetValue != None:
               self.resetValue.setDisabled(True)
            self.copy = None
            return
        self.renameInstance.setEnabled(True)
        self.deleteInstance.setEnabled(True)
        self.copyValue.setEnabled(True)
        self.resetValue.setEnabled(True)
        if self.copy != None:
            self.pasteValue.setEnabled(True)
        else:
            self.pasteValue.setDisabled(True)
        item = self.item(self.index)
        self.UpdateGenerateParameters(item)
        self.UpdateParticleParameters(item)
