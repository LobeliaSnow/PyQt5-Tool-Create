import json
import struct
import Plugin.JsonViewer
import directxwidget
import sys
import os
from PyQt5 import QtCore, QtGui, QtWidgets
import ctypes
import ScriptModule
from Plugin.Module.ModelRenderer import ModelRenderer
from Plugin.Module.GltfHelper import *

class ModelLoader(directxwidget.DirectXObject):
    windowMenu = None
    def __init__(self,parent):
        # super(ModelLoader, self).__init__(parent)
        self.renderer = ModelRenderer()
        #読み込むファイルの相対パスが入る
        self.relativePath = ""
        self.parent = parent
        #テクスチャファイル,影響頂点数
        #厳密にはマテリアルではないが、とりあえずはマテリアルとする
        self.materialList = []
        
    #バイナリ情報を読み込んで返す
    def LoadBinary(self,header):
        data = []
        #メッシュ情報のバイナリデータが入ってる
        for accessor in header["accessors"]:
            viewIndex = accessor["bufferView"]
            bufferView = header["bufferViews"][viewIndex]
            bufferIndex = bufferView["buffer"]
            #要修正 同じファイルをオープンし続けるため無駄が発生
            fileName = self.relativePath + header["buffers"][bufferIndex]["uri"]
            with open(fileName, "rb") as fileData:
                buffer = fileData.read()
                variableFormatSingle = ParseVariableFormatStringToAccessor(accessor)
                # stride = VariableFormatDataLength(variableFormatSingle)
                # loadLength = (bufferView["byteLength"] / stride)
                # variableFormat = variableFormatSingle * int(loadLength)
                variableFormat = variableFormatSingle * accessor["count"]
                offset = 0
                if "byteOffset" in accessor:
                    offset = bufferView["byteOffset"] + accessor["byteOffset"]
                else:
                    offset = bufferView["byteOffset"]
                data += [struct.unpack_from(variableFormat,buffer,offset)]
        return data

    #バイナリデータと、ヘッダーから辞書を作成
    def CreateDictionary(self,header,data):
        modelDict = {}
        #ディクショナリ作成
        for mesh in header["meshes"]:
            primitives = mesh["primitives"]
            for primitive in primitives:
                if "indices" in modelDict:
                    modelDict["indices"] += data[primitive["indices"]]
                else:
                    modelDict["indices"] = data[primitive["indices"]]
                for attribute in primitive["attributes"]:
                    if attribute in modelDict:
                        modelDict[attribute] += data[primitive["attributes"][attribute]]
                    else:
                        modelDict[attribute] = data[primitive["attributes"][attribute]]
        return modelDict
        
    #辞書データからレンダラの情報を構築する
    def BuildRenderer(self,model_dict):
        posLength = len(model_dict["POSITION"]) / 3
        indLength = len(model_dict["indices"])
        self.renderer.SetIndexBuffer(model_dict["indices"])
        self.renderer.Begin()
        for i in range(int(posLength)):
            vert = [model_dict["POSITION"][i * 3 + 0],model_dict["POSITION"][i * 3 + 1],model_dict["POSITION"][i * 3 + 2]]
            vert += [model_dict["NORMAL"][i * 3 + 0],model_dict["NORMAL"][i * 3 + 1],model_dict["NORMAL"][i * 3 + 2]]
            if "TEXCOORD_0" in model_dict:
                vert += [model_dict["TEXCOORD_0"][i * 2 + 0],model_dict["TEXCOORD_0"][i * 2 + 1]]
            else:
                vert += [0.0,0.0]
            self.renderer.SetVertices(i * ModelRenderer.structSize, vert)
        self.renderer.End()
        self.renderer.SetCount(indLength)
        # self.renderer.SetDiffuseTexture("C:/Users/black/Desktop/PyQt5-Tool-Create/DirectX11Test/Src/Plugin/white.png")
        self.renderer.SetDiffuseTexture("Plugin/DuckCM.png")
    
    #glTFロード用関数
    def Load(self,file_path):
        modelFile = open(file_path, "r")
        #Jsonの読み込み
        modelHeader = json.load(modelFile)
        #相対パスを取得
        self.relativePath = os.path.normpath(os.path.join(os.path.abspath(file_path),'../')) + "/"
        #データ保存用
        data = self.LoadBinary(modelHeader)
        #頂点情報などの辞書データ作成
        modelDict = self.CreateDictionary(modelHeader,data)
        self.BuildRenderer(modelDict)
        print("Complete")
        #JsonViewerがプラグインとして読み込まれていた場合は
        #ディクショナリをそのビュワーに登録
        try:
            self.parent.jsonViewer.OpenJsonDict(modelHeader)
            pass
        except NameError:
            pass
    
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
            self.Load(filePath[0])

    def Render(self):
        self.renderer.Render()
        
#glTFの情報読みます
#まだまだがばがば、ほしい情報はあまりとれていない
def Initialize(parent):
    if ModelLoader.windowMenu != None:
        return        
    loader = ModelLoader(parent)
    loader.RegisterMenuAction(parent)
    # loader.Load("Plugin/testModel.glTF")
    directxwidget.DirectXWidget.objectList += [loader]

if __name__ == '__main__':
    Initialize(None)