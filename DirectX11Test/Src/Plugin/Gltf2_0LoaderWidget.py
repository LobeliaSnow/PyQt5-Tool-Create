import directxwidget
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from Plugin.Module.ModelRenderer import ModelRenderer
from Plugin.Module.Gltf2_0Loader import *
import Plugin.Module.GltfConsole
import ScriptModule


class ModelLoader(directxwidget.DirectXObject,QtWidgets.QWidget):
    windowMenu = None
    def __init__(self,parent):
        super(ModelLoader,self).__init__(parent)
        self.ui = Plugin.Module.GltfConsole.Ui_GLTFConsole()
        self.ui.setupUi(self)
        self.renderers = []
        self.loader = GLTF2_0Loader()
        self.parent = parent
        self.ui.pushButton.clicked.connect(self.OpenFile)
        self.setWindowFlags(Qt.Window)    
        self.wire = False

    def RegisterMenuAction(self,parent):
        action = QtWidgets.QAction(parent)
        action.setObjectName("glTF")
        ModelLoader.windowMenu = parent.AddMenu("&glTF")
        _translate = QtCore.QCoreApplication.translate
        action.setText(_translate("MainWindow", "&Open Console"))
        action.triggered.connect(self.Show)
        ModelLoader.windowMenu.addAction(action)
        parent.ui.menubar.addAction(ModelLoader.windowMenu.menuAction())
    def Show(self):
        self.setWindowState(Qt.WindowActive)
        self.show()

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
                pos = [-positions[meshCount].data[i*3+0],positions[meshCount].data[i*3+1],positions[meshCount].data[i*3+2]]
                normal = [-normals[meshCount].data[i*3+0],normals[meshCount].data[i*3+1],normals[meshCount].data[i*3+2]]
                texcoord = None
                if texcoords != None:
                    texcoord = [texcoords[meshCount].data[i*2+0],texcoords[meshCount].data[i*2+1]]
                else:
                    texcoord = [0.0,0.0]
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
            materialIndex = self.loader.meshHeaders[i].material
            textureIndex = self.loader.materials[materialIndex].baseColorTextureIndex
            path = ""
            if textureIndex != None:
                path = self.loader.images[textureIndex].path
            renderer.SetDiffuseTexture(self.loader.GetRelativePath() + path)
            renderer.SetCount(int(len(indices)))
            self.renderers += [renderer]            
            #デフォのノード
            #使い道は現在不明
            # self.defaultNode
            #ノード群
            # self.loader.nodes
            #ノードのインデックスを管理しているリスト
            # self.loader.sceneNodes
        # self.Test()

    def Traverse(self,node_index):
        if self.nodes[node_index].tag == "mesh":
            return 
        pass
        
    def Test(self):
        self.tree = []
        for nodeIndex in self.loader.sceneNodes:
            self.tree += [self.Traverse(nodeIndex)]

    def Update(self):
        if self.ui.checkBox.checkState() == Qt.Checked:
            self.wire = True
        else:
            self.wire = False
        for renderer in self.renderers:
            renderer.Scalling(self.ui.doubleSpinBox.value())
            
    def Render(self):
        for renderer in self.renderers:
            renderer.Render(self.wire)

#glTFの情報読みます
#まだまだがばがば、ほしい情報はあまりとれていない
def Initialize(parent):
    if ModelLoader.windowMenu != None:
        return        
    loader = ModelLoader(parent)
    loader.RegisterMenuAction(parent)
    # loader.Load("Plugin/testModel.glTF")
    directxwidget.DirectXWidget.objectList += [loader]
