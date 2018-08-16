import Main
from StdOutputHooker import *
from PyQt5 import QtGui, QtCore, QtWidgets
import DirectX11
import directxwidget
import test

# http://dungeonneko.hatenablog.com/entry/2015/06/07/134938
# import cppmod
# from ctypes import pythonapi, py_object
# import ctypes

#ツールで使用される関数軍
#スクリプト作成時に使えるようにと
#マテリアルのあたり考えよう

#スプライトレンダラクラスで使用するシェーダー
shaderHeader = "struct VS_IN {\n"\
               "    float2 pos : POSITION;\n"\
               "    float4 color : COLOR;\n"\
               "    float2 uv : TEXCOORD;\n"\
               "};\n"\
               "struct PS_IN {\n"\
               "    float4 pos : SV_Position;\n"\
               "    float4 color : COLOR;\n"\
               "    float2 uv : TEXCOORD;\n"\
               "};\n"\
               "Texture2D txDiffuse : register(t0);\n"\
               "SamplerState samLinear : register(s0);"
               
shaderVS = "PS_IN VSMain(VS_IN vs_in) {\n"\
           "    PS_IN ps_in = (PS_IN)0;\n"\
           "    ps_in.pos.xy = vs_in.pos;\n"\
           "    ps_in.pos.w = 1.0f;\n"\
           "    ps_in.pos.z = 0.0f;\n"\
           "    ps_in.color = vs_in.color;\n"\
           "    ps_in.uv = vs_in.uv;\n"\
           "    return ps_in;\n"\
           "}"

shaderPS = "float4 PSMain(PS_IN ps_in) : SV_Target {\n"\
           "    float4 color = txDiffuse.Sample(samLinear, ps_in.uv);\n"\
           "    return color*ps_in.color;\n"\
           "};"
           
#そのうちちゃんとしたの作る windowサイズから
#構造体
#float2 pos     -- 8Byte
#float4 color   --16Byte
#float2 uv      -- 8Byte
class SpriteRenderer:
    stride = 8 * 4
    def __init__(self):
        self.renderer = DirectX11.Polygon2DRenderer    
        self.material = DirectX11.Material
        self.renderer = DirectX11.Polygon2DRenderer(SpriteRenderer.stride,4)
        self.material = DirectX11.Material("Sprite")
        shader = shaderHeader + shaderVS + shaderPS
        self.material.ChangeVSMemory(shader, len(shader),"VSMain",DirectX11.ShaderModel._4_0,False)
        self.material.ChangePSMemory(shader, len(shader),"PSMain",DirectX11.ShaderModel._4_0,False)
        
    def SetStructData(self,index,pos_x,pos_y,color_r,color_g,color_b,color_a,uv_x,uv_y):
        self.renderer.Set(index * SpriteRenderer.stride,[pos_x,pos_y,color_r,color_g,color_b,color_a,uv_x,uv_y])
    #全てlist 4点
    def Render(self,positions,scale,uv_begin,uv_end,color):
        self.material.Activate()
        self.renderer.Begin()
        self.SetStructData(0,            positions[0],            positions[1], color[0], color[1], color[2], color[3],             uv_begin[0],             uv_begin[1])
        self.SetStructData(1, positions[0] + scale[0],            positions[1], color[0], color[1], color[2], color[3], uv_begin[0] + uv_end[0],             uv_begin[1])
        self.SetStructData(2,            positions[0], positions[1] + scale[1], color[0], color[1], color[2], color[3],             uv_begin[0], uv_begin[1] + uv_end[1])
        self.SetStructData(3, positions[0] + scale[0], positions[1] + scale[1], color[0], color[1], color[2], color[3], uv_begin[0] + uv_end[0], uv_begin[1] + uv_end[1])
        self.renderer.End()
        self.renderer.Render(4,DirectX11.PrimitiveTopology.TRIANGLE_STRIP)
    
class DX2DObject(directxwidget.DirectXObject,object):
    def __init__(self,file_path):
        super(DX2DObject,self).__init__()
        self.renderer = SpriteRenderer()
        self.renderer.material.ChangeDiffuseTexture(file_path)
        self.pos = [-1.0,-1.0]
        self.scale = [2.0,2.0]
        self.uvBegin = [0.0,0.0]
        self.uvEnd = [1.0,1.0]
        self.color = [1.0,1.0,1.0,1.0]
        # SpriteRenderer.Initialize()

    def Render(self):
        self.renderer.Render(self.pos,self.scale,self.uvBegin,self.uvEnd,self.color)
        
#ツール上の出力ウインドウの文字を空にする
def OutputWindowClear():
    StdOutputHooker.Clear()
    
#ファイルパスが返ってくる、選択がなければ空文字
def SaveFileDialog(parent = None, dialog_name = "ファイルを保存", ext_list="全ての形式(*.*)", filter = "", view_path=""):
    fd = QtWidgets.QFileDialog()
    fp = fd.getSaveFileName(parent, dialog_name, view_path, ext_list, filter)
    return fp
    
def OpenFileDialog(parent = None, dialog_name = "ファイルを読み込み", ext_list="全ての形式(*.*)", filter = "", view_path=""):
    fd = QtWidgets.QFileDialog()
    fp = fd.getOpenFileName(parent, dialog_name, view_path, ext_list, filter)
    return fp

#文字列として投げられるパイソンコードを実行
#実行結果の文字列を返す
def ExecutePythonCode(text):
    try:
        exec(text)
    except Exception:
        e = sys.exc_info()[1]
        return e.args[0]
    # text = StdOutputHooker.GetConsoleText()
    # return text

