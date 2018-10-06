#ただグリッドを出すためだけのあれ。
import directxwidget
import DirectX11
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from Plugin.Module.GridSetting import Ui_GridSetting

#カラー変更用のシェーダーを書く。
#あくまでも仮。本チャンではシェーダーの形式が変わるはず。
shader = DirectX11.constantBufferInfo + \
		"struct SIMPLE_VS_IN{\n"\
		"	float3 pos : POSITION;\n"\
		"	float3 color : COLOR;\n"\
		"};\n"\
		"struct SIMPLE_PS_IN{\n"\
		"	float4 pos : SV_Position;\n"\
		"	float3 color : COLOR;\n"\
		"};\n",\
        "SIMPLE_PS_IN SimpleVS(SIMPLE_VS_IN input){ \n"\
		"	SIMPLE_PS_IN output = (SIMPLE_PS_IN)0;\n"\
		"	float4 pos = (float4)0;\n"\
		"	pos.xyz = input.pos;\n"\
		"	pos.w = 1.0f;\n"\
		"	//World Converte\n"\
		"	output.pos = mul(pos, view);\n"\
		"	output.pos = mul(output.pos, projection);\n"\
		"	output.color = input.color;\n"\
		"	return output;\n"\
		"}\n"\
        "float4 SimplePS(SIMPLE_PS_IN input) : SV_Target {\n"\
		"	return float4(input.color,1.0f);\n"\
		"}\n"


class GridSystem(directxwidget.DirectXObject,QWidget):
    windowMenu = None
    def __init__(self,parent):
        super(GridSystem,self).__init__(parent)
        self.renderer = DirectX11.PolygonRenderer(24,1000)
        self.material = DirectX11.Material("VertexColor")
        code = "%s\n%s"%shader
        length = len(code)
        self.material.ChangeVS3DMemory(code,length,"SimpleVS",DirectX11.ShaderModel._4_0,False)
        self.material.ChangePS3DMemory(code,length,"SimplePS",DirectX11.ShaderModel._4_0,False)
        self.ui = Ui_GridSetting()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.UpdateGrid)
        self.setWindowFlags(Qt.Window) 
        self.xColor = QColor(255,0,0,255)
        self.yColor = QColor(0,255,0,255)
        self.zColor = QColor(0,0,255,255)
        self.ui.pushButton_2.clicked.connect(self.GetXColor)
        self.ui.pushButton_3.clicked.connect(self.GetYColor)
        self.ui.pushButton_4.clicked.connect(self.GetZColor)
        self.UpdateGrid()
        self.hide()
        
    def UpdateGrid(self):
        self.CreateGrid(self.ui.spinBox.value())

    def RegisterMenuAction(self,parent):
        action = QAction(parent)
        action.setObjectName("Grid Setting")
        GridSystem.windowMenu = parent.AddMenu("&Window")
        _translate = QCoreApplication.translate
        action.setText(_translate("MainWindow", "&Grid Setting"))
        action.triggered.connect(self.Show)
        GridSystem.windowMenu.addAction(action)
        parent.ui.menubar.addAction(GridSystem.windowMenu.menuAction())

    def Show(self):
        self.setWindowState(Qt.WindowActive)
        self.show()

    def CreateGrid(self,scale):
        vertices = []
        self.renderer.Begin()
        #升目上のグリッド作成
        for i in range(int(-scale/2-0.5),int(scale/2)+1,int(scale/10)):
            i = float(i)
            white = [1.0,1.0,1.0]
            #xの方向へ伸びる線
            x0 = [-scale/2,0.0,i]
            x1 = [scale/2,0.0,i]
            #zの方向へ伸びる線
            z0 = [i,0.0,-scale/2]
            z1 = [i,0.0,scale/2]
            #xyz軸表示と被る部分だけ表示しない
            if i == 0.0:
                x1 = [0.0,0.0,0.0]
                z1 = [0.0,0.0,0.0]
            vertices += x0 + white + x1 + white
            vertices += z0 + white + z1 + white
        #軸を作成
        vertices += [scale,0.0,0.0,self.xColor.redF(),self.xColor.greenF(),self.xColor.blueF()] + [0.0,0.0,0.0,self.xColor.redF(),self.xColor.greenF(),self.xColor.blueF()]
        vertices += [0.0,scale,0.0,self.yColor.redF(),self.yColor.greenF(),self.yColor.blueF()] + [0.0,0.0,0.0,self.yColor.redF(),self.yColor.greenF(),self.yColor.blueF()]
        vertices += [0.0,0.0,scale,self.zColor.redF(),self.zColor.greenF(),self.zColor.blueF()] + [0.0,0.0,0.0,self.zColor.redF(),self.zColor.greenF(),self.zColor.blueF()]
        #頂点をセットする
        self.renderer.Set(0,vertices)
        self.renderer.End()
        self.renderCount = int(len(vertices)/6)

    def GetXColor(self):
    	self.xColor = QColorDialog.getColor(self.xColor)

    def GetYColor(self):
    	self.yColor = QColorDialog.getColor(self.yColor)

    def GetZColor(self):
    	self.zColor = QColorDialog.getColor(self.zColor)

    def Update(self):
        if self.ui.checkBox.checkState() == Qt.Checked:
            self.UpdateGrid()
            self.ui.pushButton.setEnabled(False)
        else:
            self.ui.pushButton.setEnabled(True)
        
    def Render(self):
        self.material.ActivateState()
        self.material.ActivateShader3D()
        self.renderer.Render(self.renderCount,DirectX11.PrimitiveTopology.LINE_LIST)

def Initialize(parent):    
    if GridSystem.windowMenu != None:
        return        
    grid = GridSystem(parent)
    grid.RegisterMenuAction(parent)
    directxwidget.DirectXWidget.objectList += [grid]
    pass