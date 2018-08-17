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
#コンスタントバッファの値でif文が書かれて、全スレッドで確定するならばif文は最適化されるかも

#文字列に対しての数を割り振り
def ParseVariableCount(define):
    count = 0
    if define == "SCALAR":
        count = 1
    elif define == "VEC2":
        count = 2
    elif define == "VEC3":
        count = 3
    elif define == "VEC4":
        count = 4
    elif define == "MAT2":
        count = 4
    elif define == "MAT3":
        count = 9
    elif define == "MAT4":
        count = 16
    return count
#タイプに対しての型を割り振り
def ComponentTypeToString(component_type):
    ret = ""
    if component_type == 5120:
        ret = "b"   #signed char
    elif component_type == 5121:
        ret = "B"   #unsigned char
    elif component_type == 5122:
        ret = "h"   #short
    elif component_type == 5123:
        ret = "H"   #unsigned short
    elif component_type == 5125:
        ret = "I"   #unsigned int
    elif component_type == 5126:
        ret = "f"   #float
    return ret
#型と数からバイナリ読み込み用文字列を生成
def ParseVariableFormatString(component_type,define):
    count = ParseVariableCount(define)
    ret = ""
    component = ComponentTypeToString(component_type)
    return component * count
#引数がアクセサーだけで大丈夫なバージョン
def ParseVariableFormatStringToAccessor(accessor):
    return ParseVariableFormatString(accessor["componentType"],accessor["type"])
#その文字列の型のサイズを算出
def VariableFormatDataLength(data_format):
    ret = 0
    for c in data_format:
        if c == "b":
            ret += 1
        elif c == "B":
            ret += 1
        elif c == "h":
            ret += 2
        elif c == "H":
            ret += 2
        elif c == "I":
            ret += 4
        elif c == "f":
            ret += 4
    return ret

# shader = DirectX11.constantBufferInfo + \
#     "Texture2D txDiffuse : register(t0);"\
#     "SamplerState samLinear : register(s0);"\
# 	"struct SIMPLE_VS_IN{\n"\
# 	"	float3 pos : POSITION;\n"\
# 	"	float3 normal : NORMAL;\n"\
# 	"	float2 uv : TEXCOORD;\n"\
# 	"};\n"\
# 	"struct SIMPLE_PS_IN{\n"\
# 	"	float4 pos : SV_Position;\n"\
# 	"	float3 normal : NORMAL;\n"\
# 	"	float2 uv : TEXCOORD;\n"\
# 	"};\n",\
#     "SIMPLE_PS_IN SimpleVS(SIMPLE_VS_IN input){ \n"\
# 	"	SIMPLE_PS_IN output = (SIMPLE_PS_IN)0;\n"\
# 	"	float4 pos = (float4)0;\n"\
# 	"	pos.xyz = input.pos;\n"\
# 	"	pos.w = 1.0f;\n"\
# 	"	output.pos = mul(pos, world);\n"\
# 	"	output.pos = mul(output.pos, view);\n"\
# 	"	output.pos = mul(output.pos, projection);\n"\
# 	"	output.normal = mul(input.normal,(float3x3)world);\n"\
# 	"	output.uv = input.uv;\n"\
# 	"	return output;\n"\
# 	"}\n"\
#     "float4 SimplePS(SIMPLE_PS_IN input) : SV_Target {\n"\
#     "   float3 lightDir = float3(1.0f,1.0f,1.0f);\n"\
# 	"	float4 diffuse = txDiffuse.Sample(samLinear, input.uv);\n"\
#     "   diffuse.rgb *= (dot(lightDir,input.normal)+1.0f)*0.5f;\n"\
#     "   return diffuse;\n"\
# 	"}\n"
    
# class ModelRenderer(directxwidget.DirectXObject):
#     structSize = 32
#     def __init__(self):
#         super(ModelRenderer,self).__init__()
#         # self.renderer = DirectX11.PolygonRenderer(12,15000)
#         self.renderer = DirectX11.PolygonRenderer(ModelRenderer.structSize,900000)
#         self.indexBuffer = DirectX11.IndexBuffer(900000)
#         self.count = 0
#         self.material = DirectX11.Material("Simple")
#         code = "%s\n%s" % shader
#         length = len(code)
#         self.material.ChangeVS3DMemory(code,length,"SimpleVS",DirectX11.ShaderModel._4_0,False)
#         self.material.ChangePS3DMemory(code,length,"SimplePS",DirectX11.ShaderModel._4_0,False)
#         self.transform = DirectX11.Transformer()
#         self.transform.Scalling(0.1)
#         # self.transform.Scalling(4.0)
#         self.constantBuffer = DirectX11.ConstantBuffer(1,64,int(DirectX11.ShaderStageList.VS))
#         self.rad = 0.0
#         self.testRasterizer = DirectX11.RasterizerState(DirectX11.RasterizerPreset.NONE,True)
#     def SetIndexBuffer(self,indices):
#         self.indexBuffer.Begin()
#         self.indexBuffer.Set(0,indices)
#         self.indexBuffer.End()
        
#     def Begin(self):
#         self.renderer.Begin()

#     def SetVertices(self,location,vertices):
#         self.renderer.Set(location,vertices)

#     def End(self):
#         self.renderer.End()
        
#     def SetCount(self,count):
#         self.count = count
    
#     def SetDiffuseTexture(self,file_path):
#         self.material.ChangeDiffuseTexture(file_path)
     
#     def Render(self):
#         if int(self.count) == 0:
#             return
#         self.material.ActivateState()
#         self.material.ActivateShader3D()
#         self.indexBuffer.Activate(0)
#         self.rad += 0.01
#         self.transform.RotationYAxis(self.rad)
#         self.transform.CalcWorldMatrix()
#         world = self.transform.GetWorldMatrixTranspose()
#         self.constantBuffer.Activate(world)
#         self.testRasterizer.Set()
#         self.renderer.RenderIndexed(int(self.count),DirectX11.PrimitiveTopology.TRIANGLE_LIST)

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
        self.renderer.SetDiffuseTexture("C:/Users/black/Desktop/PyQt5-Tool-Create/DirectX11Test/Src/Plugin/white.png")
    
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