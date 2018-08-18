import DirectX11
import directxwidget

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
    
class ModelRenderer(directxwidget.DirectXObject):
    structSize = 32
    def __init__(self):
        super(ModelRenderer,self).__init__()
        # self.renderer = DirectX11.PolygonRenderer(12,15000)
        self.renderer = DirectX11.PolygonRenderer(ModelRenderer.structSize,900000)
        self.indexBuffer = DirectX11.IndexBuffer(900000)
        self.count = 0
        self.material = DirectX11.Material("Simple")
        code = "%s\n%s" % shader
        length = len(code)
        self.material.ChangeVS3DMemory(code,length,"SimpleVS",DirectX11.ShaderModel._4_0,False)
        self.material.ChangePS3DMemory(code,length,"SimplePS",DirectX11.ShaderModel._4_0,False)
        self.transform = DirectX11.Transformer()
        self.transform.Scalling(0.1)
        # self.transform.Scalling(4.0)
        self.constantBuffer = DirectX11.ConstantBuffer(1,64,int(DirectX11.ShaderStageList.VS))
        self.rad = 0.0
        self.testRasterizer = DirectX11.RasterizerState(DirectX11.RasterizerPreset.NONE,True)
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
     
    def Render(self,wire = False):
        if int(self.count) == 0:
            return
        self.material.ActivateState()
        self.material.ActivateShader3D()
        self.indexBuffer.Activate(0)
        self.rad += 0.01
        self.transform.RotationYAxis(self.rad)
        self.transform.CalcWorldMatrix()
        world = self.transform.GetWorldMatrixTranspose()
        self.constantBuffer.Activate(world)
        if wire:
            self.testRasterizer.Set()
        self.renderer.RenderIndexed(int(self.count),DirectX11.PrimitiveTopology.TRIANGLE_LIST)
