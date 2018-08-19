def ModeConverteString(mode):
    topology = ""
    if mode == 0:
        topology = "POINTS"
    elif mode == 1:
        topology = "LINES"
    elif mode == 2:
        topology = "LINE_LOOP"
    elif mode == 3:
        topology = "LINE_STRIP"
    elif mode == 4:
        topology = "TRIANGLES"
    elif mode == 5:
        topology = "TRIANGLE_STRIP"
    elif mode == 6:
        topology = "TRIANGLE_FAN"
    return topology
    
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

def SamplersFilterValueToString(value):
    ret = ""
    #このデフォルト値は適当に決めたもの
    if value == None:
        value = 9729
    if value == 9728:
        ret = "NEAREST"
    elif value == 9729:
        ret = "LINEAR"
    elif value == 9984:
        ret = "NEAREST_MIPMAP_NEAREST"
    elif value == 9985:
        ret = "LINEAR_MIPMAP_NEAREST"
    elif value == 9986:
        ret = "NEAREST_MIPMAP_LINEAR"
    elif value == 9987:
        ret = "LINEAR_MIPMAP_LINEAR"
    return ret

def SamplerWrapValueToString(value):
    ret = ""
    if value == None:
        value = 10497
    if value == 33701:
        ret = "CLAMP_TO_EDGE"
    elif value == 33648:
        ret = "MIRRORED_REPEAT"
    elif value == 10497:
        ret = "REPEAT"
    return ret
