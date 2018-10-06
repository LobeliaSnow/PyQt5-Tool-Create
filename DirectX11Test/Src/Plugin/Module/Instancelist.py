from PyQt5 import QtCore
from PyQt5 import QtWidgets
from PyQt5 import QtGui
import DirectX11
from Plugin.Module.EnterName import Ui_EnterNameDialog


class EnterName(QtWidgets.QDialog):
    def __init__(self, parent):
        super(EnterName, self).__init__(parent)
        self.ui = Ui_EnterNameDialog()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.Close)

    def Close(self):
        ret = str(self.ui.lineEdit.text())
        if ret == "":
            return
        self.close()

    def Exec(self):
        self.exec()
        ret = str(self.ui.lineEdit.text())
        return ret


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

    def SetGenerateParameters(self, item):
        self.parent.ui.spinGenerateFrame.setValue(item.generateCount)
        self.parent.ui.spinSectionTime.setValue(item.sectionTime)
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
        self.parent.ui.spinColorRed.setValue(item.particleParameter.colorRed)
        self.parent.ui.spinColorGreen.setValue(item.particleParameter.colorGreen)
        self.parent.ui.spinColorBlue.setValue(item.particleParameter.colorBlue)
        self.parent.ui.spinAliveTime.setValue(item.particleParameter.aliveTime)
        self.parent.ui.spinFadeInTime.setValue(item.particleParameter.fadeInTime)
        self.parent.ui.spinFadeOutTime.setValue(item.particleParameter.fadeOutTime)
        self.parent.ui.spinStartScale.setValue(item.particleParameter.startScale)
        self.parent.ui.spinStartScale.setValue(item.particleParameter.endScale)
        self.parent.ui.spinTextureIndex.setValue(item.particleParameter.textureIndex)

    def UpdateGenerateParameters(self, item):
        item.generateCount = self.parent.ui.spinGenerateFrame.value()
        item.sectionTime = self.parent.ui.spinSectionTime.value()
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
        item.particleParameter.colorRed = self.parent.ui.spinColorRed.value()
        item.particleParameter.colorGreen = self.parent.ui.spinColorGreen.value()
        item.particleParameter.colorBlue = self.parent.ui.spinColorBlue.value()
        item.particleParameter.aliveTime = self.parent.ui.spinAliveTime.value()
        item.particleParameter.fadeInTime = self.parent.ui.spinFadeInTime.value()
        item.particleParameter.fadeOutTime = self.parent.ui.spinFadeOutTime.value()
        item.particleParameter.startScale = self.parent.ui.spinStartScale.value()
        item.particleParameter.endScale = self.parent.ui.spinStartScale.value()
        item.particleParameter.textureIndex = self.parent.ui.spinTextureIndex.value()
        if self.parent.ui.blendCopy.isChecked():
            item.blendMode = DirectX11.ParticleBlendMode.COPY
        if self.parent.ui.blendAdd.isChecked():
            item.blendMode = DirectX11.ParticleBlendMode.ADD
        if self.parent.ui.blendSub.isChecked():
            item.blendMode = DirectX11.ParticleBlendMode.SUB
        if self.parent.ui.blendScreen.isChecked():
            item.blendMode = DirectX11.ParticleBlendMode.SCREEN

    def ChangeInstance(self, item):
        self.SetGenerateParameters(item)
        self.SetParticleParameters(item)
        self.SetActive(item.text())

    def SetActive(self,instance_name):
        self.index = self.currentRow()
        self.parent.ui.activeInstance.setText(instance_name)
    def SetParentEditor(self, parent):
        self.parent = parent

    def ContextMenu(self, point):
        menu = QtWidgets.QMenu(self)
        action = QtWidgets.QAction("Add Instance", self)
        action.triggered.connect(self.AddInstance)
        menu.addAction(action)
        menu.exec_(self.mapToGlobal(point))

    def AddInstance(self):
        instanceName = self.enterName.Exec()
        if instanceName == "":
            return
        instance = ParticleInstance(instanceName, self)
        # self.instanceList += [instance]
        self.addItem(instance)
        self.setCurrentRow(self.count() - 1)
        self.SetActive(instanceName)
        # self.model.appendRow(instance)
        self.parent.ResetParameters()

    def Update(self):
        if self.count() == 0:
            return
        item = self.item(self.index)
        self.UpdateGenerateParameters(item)
        self.UpdateParticleParameters(item)
