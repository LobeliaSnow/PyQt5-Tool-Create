import json
import struct
import Plugin.JsonViewer
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
        fileName = modelHeader["buffers"][bufferIndex]["uri"]
        with open("Plugin/"+fileName, "rb") as fileData:
            buffer = fileData.read()
            variableFormatSingle = ParseVariableFormatStringToAccessor(accessor)
            stride = VariableFormatDataLength(variableFormatSingle)
            loadLength = (bufferView["byteLength"] / stride)
            variableFormat = variableFormatSingle * int(loadLength)
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
    #JsonViewerがプラグインとして読み込まれていた場合は
    #ディクショナリをそのビュワーに登録
    try:
        parent.jsonViewer.OpenJsonDict(modelDict)
    except NameError:
        pass
        
            
if __name__ == '__main__':
    Initialize(None)