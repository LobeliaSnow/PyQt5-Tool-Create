import json
import os
import struct
#開発時
if __name__ == "__main__":
    from GltfHelper import *
else:#自分のツールにスクリプトとして取り込まれたとき(ルートディレクトリがかわるため)
    from Plugin.Module.GltfHelper import *

#公式リファレンス
#https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#meshes

#ヘルパー
#ディクショナリの中にそのキーがあればその値を返し、なければNoneを返す
def DictInValue(dictionary,value_name):
    if value_name in dictionary:
        return dictionary[value_name]
    return None

class GLTF2_0Node:
    def __init__(self,tag):
        self.tag = tag
class GLTF2_0MeshNode(GLTF2_0Node):
    def __init__(self,node_info):
        super(GLTF2_0MeshNode,self).__init__("mesh")
        self.meshIndex = node_info["mesh"]
        #アニメーションの情報をこのノードが持っていなかったらNone
        self.skinIndex = DictInValue(node_info,"skin")
class GLTF2_0TransformNode(GLTF2_0Node):
    def __init__(self,node_info):
        super(GLTF2_0TransformNode,self).__init__("transform")
        #親子関係周り
        self.children = DictInValue(node_info,"children")
        self.matrix = DictInValue(node_info,"matrix")
        #ノードがローカル変換行列を持っているかどうか
        if self.matrix == None:
            self.translation = DictInValue(node_info,"translation")
            if self.translation == None:
                self.translation = [0.0,0.0,0.0]
            #クォータニオン
            self.rotation = DictInValue(node_info,"rotation")
            if self.rotation == None:
                self.rotation = [0.0,0.0,0.0,1.0]
            self.scale = DictInValue(node_info,"scale")
            if self.scale == None:
                self.scale = [1.0,1.0,1.0]
class GLTF2_0CameraNode(GLTF2_0Node):
    def __init__(self,node_info):
        super(GLTF2_0CameraNode,self).__init__("camera")
        #カメラはトランスフォーム情報がカメラというだけなので
        #トランスフォーム情報はトランスフォームのノードを所持します
        self.transform = GLTF2_0TransformNode(node_info)

#カメラ情報
class GLTF2_0Camera:
    def __init__(self,type_name):
        self.typeName = type_name
class GLTF2_0PerspectiveCamera(GLTF2_0Camera):
    def __init__(self,aspect,fov,near,far):
        self.aspect = aspect
        self.fov = fov
        self.near = near
        self.far = far        
class GLTF2_0OrthographicCamera(GLTF2_0Camera):
    def __init__(self,xmag,ymag,near,far):
        self.xmag = xmag
        self.ymag = ymag
        self.near = near
        self.far = far  

class GLTF2_0Buffer:
    def __init__(self,file_path,byte_length):
        self.file_path = file_path
        self.rawBuffer = open(file_path,"rb").read()
        self.byteLength = byte_length
        self.decodeData = {}

class GLTF2_0MeshHeader:
    def __init__(self,mesh_info):
        self.name = DictInValue(mesh_info,"name")
        if self.name == None:
            self.name = ""
        self.ParesePrimitives(mesh_info)
    def ParesePrimitives(self,mesh_info):
        #primitivesの要素を1と仮定
        primitives = mesh_info["primitives"][0]
        if "mode" in primitives:
            self.topology = ModeConverteString(primitives["mode"])
        else:
            self.topology = ModeConverteString(4)
        #インデックスバッファが存在してるかどうか
        self.indices = DictInValue(primitives,"indices")
        self.material = DictInValue(primitives,"material")
        if self.material == None:
            self.material = 0
        self.attributes = primitives["attributes"]
        #モーフデータ
        self.targets = DictInValue(primitives,"targets")
        #拡張機能系
        self.extension = DictInValue(primitives,"extension")
        self.extras = DictInValue(primitives,"extras")

class GLTF2_0BufferData:
    def __init__(self,data,count,min_value,max_value):
        self.data = data
        self.count = count
        self.min = min_value
        self.max = max_value

#他にもいろいろ情報は持ってると思うけど、とりあえずこれだけ
#texCoordの数字はTEXCOORD_<X>の部分のインデックス
class GLTF2_0Material:
    def __init__(self,materials):
        self.name = DictInValue(materials,"name")
        if self.name == None:
            self.name = ""
        material = materials["pbrMetallicRoughness"]
        textureInfo = DictInValue(material,"baseColorTexture")
        self.baseColorTextureIndex = None
        self.baseColorTextureTexCoord = None
        if textureInfo != None:
            self.baseColorTextureIndex = DictInValue(textureInfo,"index")
            self.baseColorTextureTexCoord = DictInValue(textureInfo,"texCoord")
            if self.baseColorTextureTexCoord == None:
                self.baseColorTextureTexCoord = 0
        self.baseColorFactor = DictInValue(material,"baseColorFactor")
        if self.baseColorFactor == None:
            self.baseColorFactor = [1.0,1.0,1.0,1.0]
        #物理ベースでのエミッシブファクターを使用押したことがないのでこのデフォルト値はでたらめな可能性がある
        # self.emmisiveFactor = DictInValue(materials,"emmisiveFactor")
        # if self.emmisiveFactor == None:
        #     self.emmisiveFactor = [1.0,1.0,1.0,1.0]
        normalTexture = DictInValue(materials,"normalTexture")
        if normalTexture != None:
            self.normalMapIndex = normalTexture["index"]
            self.normalMapScale = DictInValue(normalTexture,"scale")
            if self.normalMapScale == None:
                self.normalMapScale = 1.0
            self.normalMapTexCoord = DictInValue(normalTexture,"texCoord")
            if self.normalMapTexCoord == None:
                self.normalMapTexCoord = 0

class GLTF2_0Sampler:
    def __init__(self,mag_filter,min_filter,wrap_s,wrap_t,name,extensions,extras):
        self.magFilter = mag_filter
        self.minFilter = min_filter
        self.wrapS = wrap_s
        self.wrapT = wrap_t
        self.name = name
        self.extensions = extensions
        self.extras = extras
        
class GLTF2_0Image:
    def __init__(self,path,sampler):
        self.path = path
        self.sampler = sampler
        pass                

#TODO : 使用し終わったBufferをクリアすること
class GLTF2_0Loader:
    #外部用
    def __init__(self):
        self.header = None        
    def Load(self,file_path):
        #相対パス取得(フォルダまで)
        self.relativePath = os.path.normpath(os.path.join(os.path.abspath(file_path),'../')) + "/"
        #ヘッダー情報 俗にいう.gltfファイルの中身
        self.ParseHeader(file_path)
        #Gltf2.0用のローダーの為、それ以外は弾く
        if self.header["asset"]["version"] != "2.0":
            raise Exception
        self.LoadBuffers()
        #ここからガッツリ情報が読みだされる
        self.ParseScenes()
        #ここでindices等の情報結びつけられたデータベースが手に入る
        self.CreateDB()
        
    #読み込まれたGLTFが何で作られたものかを返す
    #読み込まれていない場合はエラーとなる
    def GetGenerator(self):
        return self.header["asset"]["generator"]
    #相対パス(フォルダまで)が返る
    def GetRelativePath(self):
        return self.relativePath
    #内部用
    #Jsonの読み込み
    def ParseHeader(self,file_path):
        self.header = json.load(open(file_path, "r"))
            
    #この先の関数の前提条件 ヘッダーがパースされていること
    #メンバ変数buffersに中身が入る
    #中身はタプルを用いたペアになっていて、[サイズ,生データ]になっている
    def LoadBuffers(self):
        self.buffers = []
        for buffer in self.header["buffers"]:
            self.buffers += [GLTF2_0Buffer(self.relativePath + buffer["uri"],buffer["byteLength"])]
    #シーンを読んでいく
    def ParseScenes(self):
        #scenesの要素が1だと仮定 ノードへのインデックスが入っている
        self.sceneNodes = self.header["scenes"][0]["nodes"]
        #self.nodesに必要なノードデータがそろう
        self.ParseNodes()
        #カメラ情報
        self.ParseCamera()
        #デフォルトのシーンがどれか
        self.defaultNode = DictInValue(self.header,"scene")
        if self.defaultNode == None:
            self.defaultNode = 0
        #メッシュのプリミティブ情報をパース
        self.ParseMeshes()        
        #バイナリ情報をパース
        self.ParseBuffers()
        #マテリアル情報をパース
        self.ParseMaterial()

    #扱いやすい形式にノードをパースして変換
    def ParseNodes(self):
        self.nodes = []
        for nodeInfo in self.header["nodes"]:
            node = None
            #このノードがメッシュを表すノードかどうか
            if "mesh" in nodeInfo:
                node = GLTF2_0MeshNode(nodeInfo)
            else:#トランスフォーム情報
                if "camera" in nodeInfo:
                    node = GLTF2_0CameraNode(nodeInfo)
                else:
                    node = GLTF2_0TransformNode(nodeInfo)
            self.nodes += [node]
    #カメラ情報取得
    def ParseCamera(self):
        cameras = DictInValue(self.header,"cameras")
        if cameras != None:
            for camera in cameras:
                self.cameras = []
                typeName = camera["type"]
                if typeName == "perspective":
                    camera = camera["perspective"]
                    far = DictInValue(self.header,"cameras")
                    if far == None:
                        far = 1000.0
                    self.cameras += [GLTF2_0PerspectiveCamera(camera["aspectRatio"],camera["yfov"],camera["znear"],far)]
                elif typeName == "orthographic":
                    camera = camera["orthographic"]
                    far = DictInValue(self.header,"cameras")
                    if far == None:
                        far = 1000.0
                    self.cameras += [GLTF2_0OrthographicCamera(camera["xmag"],camera["ymag"],camera["znear"],far)]
                else:
                    raise Exception
    #プリミティブの情報を取得
    def ParseMeshes(self):
        self.meshHeaders = []
        for mesh in self.header["meshes"]:
            self.meshHeaders += [GLTF2_0MeshHeader(mesh)]

    #bufferViews,accessorを利用して、バイナリデータのパース
    #sparseに対応する必要あり
    def ParseBuffers(self):
        self.data = []
        for accessor in self.header["accessors"]:
            #このデータブロックが1要素当たりどのような構成しているかをstructで読める形で返す
            singleFormat = ParseVariableFormatStringToAccessor(accessor)
            #どのブロックの情報を読むか
            viewIndex = accessor["bufferView"]
            bufferView = self.header["bufferViews"][viewIndex]
            #どのファイルの情報を読むか
            bufferIndex = bufferView["buffer"]
            offset = 0
            #どの地点から読むか
            if "byteOffset" in accessor:
                offset = accessor["byteOffset"]
            if "byteOffset" in bufferView:
                offset += bufferView["byteOffset"]
            count = accessor["count"]
            bufferData = struct.unpack_from(singleFormat*count,self.buffers[bufferIndex].rawBuffer,offset)
            self.data += [GLTF2_0BufferData(bufferData,count,DictInValue(accessor,"min"),DictInValue(accessor,"max"))]

    def ParseMaterial(self):
        materials = DictInValue(self.header,"materials")
        if materials == None:
            self.materials = None
        else:
            self.materials = []
            for material in materials:
                self.materials += [GLTF2_0Material(material)]
        self.ParseImages()
        
    #テクスチャを持っていない場合はself.images = Noneとなる
    def ParseImages(self):
        textures = DictInValue(self.header,"textures")
        samplers = DictInValue(self.header,"samplers")
        self.images = None
        if textures != None:
            self.images = []
            for texture in textures:
                path = self.header["images"][texture["source"]]["uri"]
                sampler = None
                if samplers != None:
                    samplerData = samplers[texture["sampler"]]
                    magFilter = SamplersFilterValueToString(DictInValue(samplerData,"magFilter"))
                    minFilter = SamplersFilterValueToString(DictInValue(samplerData,"minFilter"))
                    wrapS = SamplerWrapValueToString(DictInValue(samplerData,"wrapS"))
                    wrapT = SamplerWrapValueToString(DictInValue(samplerData,"wrapT"))
                    name = DictInValue(samplers,"name")
                    if name == None:
                        name = "defaultSampler"
                    extensions = DictInValue(samplers,"extensions")
                    extras = DictInValue(samplers,"extras")
                    sampler = GLTF2_0Sampler(magFilter,minFilter,wrapS,wrapT,name,extensions,extras)
                self.images += [GLTF2_0Image(path,sampler)]
        else:#テクスチャを所持していないモデル
            self.textures = None
        
    def CreateDB(self):
        #インデックス
        self.dataBase = []
        #要素名
        self.dataBaseDict = {}
        for header in self.meshHeaders:
            #インデックスが存在すれば(たいていのモデルでは存在するが、ごくまれに存在しないものがあり)
            if header.indices != None:
                indicesPair = ("indices",self.data[header.indices])
                self.dataBase += [indicesPair]
                if "indices" in self.dataBaseDict:
                    self.dataBaseDict["indices"] += [indicesPair[1]]
                else:
                    self.dataBaseDict["indices"] = [indicesPair[1]]
            for attribute in header.attributes:
                pair = (attribute,self.data[header.attributes[attribute]])
                if attribute in self.dataBaseDict:
                    self.dataBaseDict[attribute] += [pair[1]]
                else:
                    self.dataBaseDict[attribute] = [pair[1]]
                self.dataBase += [pair]

#TODO : レンダラ構築

         
def Initialize(parent):
    loader = GLTF2_0Loader()
    loader.Load("../testModel.gltf")
    loader.Load("C:/Users/black/Desktop/2.0/2CylinderEngine/glTF/2CylinderEngine.gltf")
    loader.Load("C:/Users/black/Desktop/2.0/AlphaBlendModeTest/glTF/AlphaBlendModeTest.gltf")
    loader.Load("C:/Users/black/Desktop/2.0/Monster/glTF/Monster.gltf")
    loader.Load("C:/Users/black/Desktop/2.0/GearboxAssy/glTF/GearboxAssy.gltf")
    loader.Load("C:/Users/black/Desktop/2.0/Cameras/glTF/Cameras.gltf")
    loader.Load("C:/Users/black/Desktop/2.0/TriangleWithoutIndices/glTF/TriangleWithoutIndices.gltf")
    loader.Load("C:/Users/black/Desktop/2.0/WaterBottle/glTF/WaterBottle.gltf")
    loader.Load("C:/Users/black/Desktop/glTF-Blender-Exporter-master/polly/project_polly.gltf")
    # loader.Load("C:/Users/black/Desktop/2.0//glTF/.gltf")

if __name__ == "__main__":
    Initialize(None)

