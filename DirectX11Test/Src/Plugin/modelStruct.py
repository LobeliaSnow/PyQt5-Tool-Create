import json
import struct
import Plugin.JsonViewer
import directxwidget
import DirectX11
import sys
import os
from PyQt5 import QtCore, QtGui, QtWidgets
import ctypes
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

shader = DirectX11.constantBufferInfo + \
    "Texture2D txDiffuse : register(t0);"\
    "SamplerState samLinear : register(s0);"\
	"struct SIMPLE_VS_IN{\n"\
	"	float3 pos : POSITION;\n"\
	"	float3 normal : NORMAL;\n"\
	"	float2 uv : TEXCOORD;\n"\
	"};\n"\
	"struct SIMPLE_PS_IN{\n"\
	"	float4 pos : SV_Position;\n"\
	"	float3 normal : NORMAL;\n"\
	"	float2 uv : TEXCOORD;\n"\
	"};\n",\
    "SIMPLE_PS_IN SimpleVS(SIMPLE_VS_IN input){ \n"\
	"	SIMPLE_PS_IN output = (SIMPLE_PS_IN)0;\n"\
	"	float4 pos = (float4)0;\n"\
	"	pos.xyz = input.pos;\n"\
	"	pos.w = 1.0f;\n"\
	"	output.pos = mul(pos, world);\n"\
	"	output.pos = mul(output.pos, view);\n"\
	"	output.pos = mul(output.pos, projection);\n"\
	"	output.normal = mul(input.normal,(float3x3)world);\n"\
	"	output.uv = input.uv;\n"\
	"	return output;\n"\
	"}\n"\
    "float4 SimplePS(SIMPLE_PS_IN input) : SV_Target {\n"\
    "   float3 lightDir = float3(1.0f,1.0f,1.0f);\n"\
	"	float4 diffuse = txDiffuse.Sample(samLinear, input.uv);\n"\
    "   diffuse.rgb *= (dot(lightDir,input.normal)+1.0f)*0.5f;\n"\
    "   return diffuse;\n"\
	"}\n"
    
class GLTFRenderer(directxwidget.RenderObject):
    def __init__(self):
        super(GLTFRenderer,self).__init__()
        # self.renderer = DirectX11.PolygonRenderer(12,15000)
        self.renderer = DirectX11.PolygonRenderer(32,15000)
        self.indexBuffer = DirectX11.IndexBuffer(15000)
        self.count = 0
        self.material = DirectX11.Material("Simple")
        code = "%s\n%s" % shader
        length = len(code)
        self.material.ChangeVS3DMemory(code,length,"SimpleVS",DirectX11.ShaderModel._4_0,False)
        self.material.ChangePS3DMemory(code,length,"SimplePS",DirectX11.ShaderModel._4_0,False)
        self.transform = DirectX11.Transformer()
        self.transform.Scalling(0.1)
        self.constantBuffer = DirectX11.ConstantBuffer(1,64,int(DirectX11.ShaderStageList.VS))
        self.rad = 0.0
        ####テスト用ステート####
        self.testRasterizer = DirectX11.RasterizerState(DirectX11.RasterizerPreset.BACK,False)
        self.testRasterizerWire = DirectX11.RasterizerState(DirectX11.RasterizerPreset.BACK,True)
        stencilDesc = DirectX11.StencilDesc()
        self.testDepthTrue = DirectX11.DepthStencilState(DirectX11.DepthPreset.ALWAYS,True,stencilDesc,False)
        self.testDepthFalse = DirectX11.DepthStencilState(DirectX11.DepthPreset.ALWAYS,False,stencilDesc,False)
        self.testBlend = DirectX11.BlendState(DirectX11.BlendPreset.COPY,False,False)
        self.testBlendTrue = DirectX11.BlendState(DirectX11.BlendPreset.COPY,True,False)
        self.testBlendCoverage = DirectX11.BlendState(DirectX11.BlendPreset.COPY,True,True)
        #########################
        
    def SetIndexBuffer(self,indices):
        self.indexBuffer.Begin()
        self.indexBuffer.Set(0,indices)
        self.indexBuffer.End()
        
    def Begin(self):
        self.renderer.Begin()

    def SetVertices(self,location,vertices):
        self.renderer.Set(location,vertices)

    def End(self):
        self.renderer.End()
        
    def SetCount(self,count):
        self.count = count
    
    def SetDiffuseTexture(self,file_path):
        self.material.ChangeDiffuseTexture(file_path)
     
    def Render(self):
        self.material.ActivateState()
        self.material.ActivateShader3D()
        ####テスト用ステート####
        # self.testRasterizer.Set()
        # self.testDepthTrue.Set()
        # self.testBlendCoverage.Set()
        #########################
        self.indexBuffer.Activate(0)
        self.rad += 0.01
        self.transform.RotationYAxis(self.rad)
        self.transform.CalcWorldMatrix()
        world = self.transform.GetWorldMatrixTranspose()
        self.constantBuffer.Activate(world)
        self.renderer.RenderIndexed(int(self.count),DirectX11.PrimitiveTopology.TRIANGLE_LIST)


#glTFの情報読みます
#まだまだがばがば、ほしい情報はあまりとれていない
#現状はインデックスバッファと、アトリビュート(頂点や法線)のみ
#後にクラス化
def Initialize(parent):
    #現在テスト用
    modelFile = open("Plugin/testModel.glTF", "r")
    modelHeader = json.load(modelFile)
    # print(modelHeader)
    data = []
    #メッシュ情報のバイナリデータが入ってる
    for accessor in modelHeader["accessors"]:
        viewIndex = accessor["bufferView"]
        bufferView = modelHeader["bufferViews"][viewIndex]
        bufferIndex = bufferView["buffer"]
        #要修正 同じファイルをオープンし続けるため無駄が発生
        fileName = modelHeader["buffers"][bufferIndex]["uri"]
        with open("Plugin/" + fileName, "rb") as fileData:
            buffer = fileData.read()
            variableFormatSingle = ParseVariableFormatStringToAccessor(accessor)
            # stride = VariableFormatDataLength(variableFormatSingle)
            # loadLength = (bufferView["byteLength"] / stride)
            # variableFormat = variableFormatSingle * int(loadLength)
            variableFormat = variableFormatSingle * accessor["count"]
            offset = bufferView["byteOffset"] + accessor["byteOffset"]
            data += [struct.unpack_from(variableFormat,buffer,offset)]
    modelDict = {}
    #ディクショナリ作成
    for mesh in modelHeader["meshes"]:
        primitives = mesh["primitives"]
        for primitive in primitives:
            modelDict["indices"] = data[primitive["indices"]]
            for attribute in primitive["attributes"]:
                modelDict[attribute] = data[primitive["attributes"][attribute]]
    renderer = GLTFRenderer()
    renderer.SetIndexBuffer(modelDict["indices"])
    posLength = len(modelDict["POSITION"]) / 3
    indLength = len(modelDict["indices"])
    renderer.Begin()
    for i in range(int(posLength)):
        vert = [modelDict["POSITION"][i * 3 + 0],modelDict["POSITION"][i * 3 + 1],modelDict["POSITION"][i * 3 + 2]]
        vert += [modelDict["NORMAL"][i * 3 + 0],modelDict["NORMAL"][i * 3 + 1],modelDict["NORMAL"][i * 3 + 2]]
        vert += [modelDict["TEXCOORD_0"][i * 2 + 0],modelDict["TEXCOORD_0"][i * 2 + 1]]
        renderer.SetVertices(i * 32, vert)
    renderer.End()
    renderer.SetCount(indLength)
    renderer.SetDiffuseTexture("Plugin/DuckCM.png")
    directxwidget.DirectXWidget.renderList += [renderer]
    #JsonViewerがプラグインとして読み込まれていた場合は
    #ディクショナリをそのビュワーに登録
    try:
        parent.jsonViewer.OpenJsonDict(modelDict)
    except NameError:
        pass

if __name__ == '__main__':
    Initialize(None)