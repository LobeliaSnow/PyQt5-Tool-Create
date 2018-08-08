#ただグリッドを出すためだけのあれ。
import directxwidget
import DirectX11

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


class GridRenderer(directxwidget.RenderObject):
    def __init__(self,scale):
        super(GridRenderer,self).__init__()
        self.renderer = DirectX11.PolygonRenderer(12,1000)
        self.material = DirectX11.Material("VertexColor")
        # code = "%s\n%s"%shader
        # length = len(code)
        # self.material.ChangeVS3DMemory(code,length,"SimpleVS",DirectX11.ShaderModel._4_0,False)
        # self.material.ChangePS3DMemory(code,length,"SimplePS",DirectX11.ShaderModel._4_0,False)
        vertices = []
        self.renderer.Begin()
        for i in range(int(-scale/2),int(scale/2)+1,int(scale/10)):
            i = float(i)
            vertices += [i,0.0,-scale/2] + [i,0.0,scale/2]
            vertices += [-scale/2,0.0,i] + [scale/2,0.0,i]
        self.renderer.Set(0,vertices)
        self.renderer.End()
        self.renderCount = len(vertices)
        
    def Render(self):
        self.material.ActivateState()
        self.material.ActivateShader3D()
        self.renderer.Render(self.renderCount,DirectX11.PrimitiveTopology.LINE_LIST)

def Initialize(parent):
    directxwidget.DirectXWidget.renderList += [GridRenderer(500)]
    pass