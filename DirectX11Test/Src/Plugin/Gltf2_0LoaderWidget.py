import directxwidget
from PyQt5 import QtCore, QtGui, QtWidgets
from Plugin.Module.ModelRenderer import ModelRenderer
from Plugin.Module.Gltf2_0Loader import *
import ScriptModule

class ModelLoader(directxwidget.DirectXObject):
    windowMenu = None
    def __init__(self,parent):
        self.renderers = []
        self.loader = GLTF2_0Loader()
        self.parent = parent
    def RegisterMenuAction(self,parent):
        action = QtWidgets.QAction(parent)
        action.setObjectName("Load glTF")
        ModelLoader.windowMenu = parent.AddMenu("&File")
        _translate = QtCore.QCoreApplication.translate
        action.setText(_translate("MainWindow", "&Load glTF"))
        action.triggered.connect(self.OpenFile)
        ModelLoader.windowMenu.addAction(action)
        parent.ui.menubar.addAction(ModelLoader.windowMenu.menuAction())

    def OpenFile(self):
        filePath = ScriptModule.OpenFileDialog(self.parent,"glTFを読み込み","glTF(*gltf *glTF *GLTF);;全ての形式(*.*)")
        if filePath[0] != "":
            self.loader.Load(filePath[0])
            self.renderers.clear()
            self.BuildRenderer()
            
    def CreateVertexBufferStruct(self,data_base):
        positions = DictInValue(data_base,"POSITION")
        normals = DictInValue(data_base,"NORMAL")
        texcoords = DictInValue(data_base,"TEXCOORD_0")
        ret = []
        for meshCount in range(len(positions)):
            for i in range(positions[meshCount].count):
                pos = [positions[meshCount].data[i*3+0],positions[meshCount].data[i*3+1],positions[meshCount].data[i*3+2]]
                normal = [normals[meshCount].data[i*3+0],normals[meshCount].data[i*3+1],normals[meshCount].data[i*3+2]]
                texcoord = [texcoords[meshCount].data[i*2+0],texcoords[meshCount].data[i*2+1]]
                ret += pos
                ret += normal
                ret += texcoord
        return ret
    def BuildRenderer(self):
        rendererCount = len(self.loader.dataBaseDict["indices"])
        for i in range(rendererCount):
            renderer = ModelRenderer()
            indices = self.loader.dataBaseDict["indices"][i].data
            renderer.SetIndexBuffer(indices)
            vertices = self.CreateVertexBufferStruct(self.loader.dataBaseDict)
            renderer.Begin()
            renderer.SetVertices(0,vertices)
            renderer.End()
            self.loader.meshHeaders[i].material
            textureIndex = self.loader.materials[self.loader.meshHeaders[i].material].baseColorTextureIndex
            renderer.SetDiffuseTexture(self.loader.GetRelativePath() + self.loader.images[textureIndex].path)
            renderer.SetCount(int(len(indices)))
            self.renderers += [renderer]

    def Render(self):
        for renderer in self.renderers:
            renderer.Render()

#glTFの情報読みます
#まだまだがばがば、ほしい情報はあまりとれていない
def Initialize(parent):
    if ModelLoader.windowMenu != None:
        return        
    loader = ModelLoader(parent)
    loader.RegisterMenuAction(parent)
    # loader.Load("Plugin/testModel.glTF")
    directxwidget.DirectXWidget.objectList += [loader]
