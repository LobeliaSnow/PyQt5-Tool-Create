#pragma once

//-------------------------------------------------------------------------------
//
//	Python�p DirectX11���b�p�[
//												author Lobelia
//
//-------------------------------------------------------------------------------
#include "../Define/Define.hpp"
#include <Windows.h>
#include <d3d11.h>
#include "../D3DCompiler/d3dcompiler.h"
#include "../DirectXTex/DirectXTex.h"
#include <dinput.h>
#include <wrl.h>
#include <memory>
#include <string>
#include <math.h>

#include "../Enum/TextureFormat.hpp"
#include "../Enum/TextureAccessFlag.hpp"
#include "../Enum/CpuAccessFlag.hpp"
#include "../Enum/ShaderStageList.hpp"
#include "../Enum/ShaderModel.hpp"
#include "../Enum/RenderState.hpp"
#include "../Enum/PrimitiveTopology.hpp"

#include "../Utility/Utility.hpp"
#include "../Utility/Math.hpp"
#include "../Enum/Extension.hpp"

using Microsoft::WRL::ComPtr;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dinput8.lib")
#ifdef _DEBUG
#pragma comment(lib,"DirectXTexDebug.lib")
#else
#pragma comment(lib,"DirectXTexRelease.lib")
#endif


//todo : rendertarget��depthstencilview�쐬
//memo : �f�[�^�ƃ����_��(����)�͕�����

namespace Lobelia {
	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Device
	//
	//--------------------------------------------------------------------------------------------------
	class Device {
	public:
		//Python�p
		static void Create(UINT device_flag = 0);
		static void Destroy();
	public:
		//C++�p
		static ComPtr<ID3D11Device>& Get();
		//�f�B�t�@�[�h�R���e�L�X�g�ɂ��čl����
		static ComPtr<ID3D11DeviceContext>& GetContext();
	private:
		static ComPtr<ID3D11Device> device;
		static ComPtr<ID3D11DeviceContext> context;
	};
	//----------------------------------------End Device--------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Texture etc...
	//
	//--------------------------------------------------------------------------------------------------
	struct RTSampleDesc {
		int count;
		int quality;
	};
	class Texture {
		friend class RenderTarget;
	public:
		Texture(int x, int y, TextureFormat format, UINT bind_flags, const RTSampleDesc& sample_desc, ACCESS_FLAG access_flag = ACCESS_FLAG::DEFAULT, CPU_ACCESS_FLAG cpu_flag = CPU_ACCESS_FLAG::NONE, int array_count = 1);
		Texture(const ComPtr<ID3D11Texture2D>& texture);
		~Texture() = default;
	public:
		//Python�p
		void Set(int num_slot, ShaderStageList list);
		int GetWidth();
		int GetHeight();
	public:
		//C++�p
		ComPtr<ID3D11ShaderResourceView>& GetView();
		ComPtr<ID3D11Texture2D>& GetTexture();
	public:
		//Python�p
		static void Clean(int num_slot, ShaderStageList list);
	private:
		void CreateTexture(int x, int y, TextureFormat format, UINT bind_flags, int count, int quality, ACCESS_FLAG access_flag, CPU_ACCESS_FLAG cpu_flag, int array_count);
		void CreateShaderResourceView(TextureFormat format);
	private:
		ComPtr<ID3D11ShaderResourceView> view;
		ComPtr<ID3D11Texture2D> texture;
		int x;
		int y;
	};

	class TextureManager final {
	public:
		static std::shared_ptr<Texture> Load(const char* file_path, bool force = false);
		static void Save(const char* file_path, Texture* texture);
		static std::shared_ptr<Texture> CreateNormalMap(Texture* src, float amplitude);
	private:
		static void CreateEmptyTexture(std::shared_ptr<Texture>& texture);
		static void LoadFile(const wchar_t* file_path, TextureExtension extension, DirectX::TexMetadata* meta, DirectX::ScratchImage& image);
	};
	//TODO : �f�v�X�X�e���V���̓N���X�����邱��
	class RenderTarget {
	public:
		//Python�p
		RenderTarget(int x, int y, TextureFormat format, const RTSampleDesc& sample_desc, int array_count = 1);
		//C++ SwapChain�p
		RenderTarget(const ComPtr<ID3D11Texture2D>& texture);
		~RenderTarget() = default;
	public:
		//Python�p
		void Clear(int a, int r, int g, int b);
		void Activate();
	public:
		//C++�p
	private:
		void CreateRenderTarget(int x, int y, const RTSampleDesc& sample_desc, int array_count);
		void CreateDepthView();
	private:
		ComPtr<ID3D11RenderTargetView> renderTarget;
		std::shared_ptr<Texture> texture;
		std::vector<ComPtr<ID3D11RenderTargetView>> oneFaceTarget;
		std::shared_ptr<Texture> depth;
		ComPtr<ID3D11DepthStencilView> depthView;
	};
	class SwapChain {
	public:
		//HWND�̈������킩��Ȃ��B�B�����B
		SwapChain(void* hwnd, int x, int y, const RTSampleDesc& sample_desc);
		~SwapChain();
	public:
		//Python�p
		RenderTarget * GetRenderTarget();
		void Clear(int a, int r, int g, int b);
		void Present(int v_sync = 0);
	public:
		//C++�p
		const ComPtr<IDXGISwapChain>& Get();
	private:
		void CreateSwapChain(HWND hwnd, int x, int y, const RTSampleDesc& sample_desc);
	private:
		ComPtr<IDXGISwapChain> swapChain;
		std::shared_ptr<RenderTarget> rt;
	};
	//--------------------------------------End Texture etc...--------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin RenderState
	//
	//--------------------------------------------------------------------------------------------------
	class BlendState {
	public:
		BlendState(BlendPreset preset, bool blend, bool alpha_coverage);
		~BlendState() = default;
	public:
		//Python�p
		void Set();
	private:
		void SettingPreset(D3D11_BLEND_DESC* desc, int preset)const;
	private:
		/**@brief �u�����h�X�e�[�g�{��*/
		ComPtr<ID3D11BlendState> state;
	};
	class SamplerState {
	public:
		SamplerState(SamplerPreset sampler, int max_anisotropy, bool is_border = false);
		~SamplerState() = default;
	public:
		//Python�p
		void Set();
	private:
		void SettingPreset(D3D11_SAMPLER_DESC* desc, int preset)const;
	private:
		/**@brief �T���v���[�X�e�[�g�{��*/
		ComPtr<ID3D11SamplerState> state;
	};
	class RasterizerState {
	public:
		RasterizerState(RasterizerPreset rasterizer, bool wire_frame = false);
		~RasterizerState() = default;
	public:
		//Python�p
		void Set();
	private:
		void SettingPreset(D3D11_RASTERIZER_DESC* desc, int preset)const;
	private:
		/**@brief ���X�^���C�U�[�X�e�[�g�{��*/
		ComPtr<ID3D11RasterizerState> state;
	};
	//�X�e���V���p
	struct StencilDesc {
		//�ǂݏ������Ȃ��Ƃ���0���w�肵�ĂˁB
		UINT8 readMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		UINT8 writeMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		struct Face {
			enum class Operation :BYTE { KEEP = D3D11_STENCIL_OP_KEEP, ZERO = D3D11_STENCIL_OP_ZERO, REPLACE = D3D11_STENCIL_OP_REPLACE, INCR_CLAMP = D3D11_STENCIL_OP_INCR_SAT, DECR_CLAMP = D3D11_STENCIL_OP_DECR_SAT, INVERT = D3D11_STENCIL_OP_INVERT, INCR_WRAP = D3D11_STENCIL_OP_INCR, DECR_WRAP = D3D11_STENCIL_OP_DECR };
			//�e�X�g���s
			Operation faile;
			//�[�x�e�X�g���s
			Operation depthFail;
			//�e�X�g���i
			Operation pass;
			//���� �\�[�X�l �E�� �X�e���V���l
			enum class Function :BYTE { ALWAYS_FAIL = D3D11_COMPARISON_NEVER, LESS = D3D11_COMPARISON_LESS, EQUAL = D3D11_COMPARISON_EQUAL, LESS_EQUAL = D3D11_COMPARISON_LESS_EQUAL, GREATER = D3D11_COMPARISON_GREATER, NOT_EQUAL = D3D11_COMPARISON_NOT_EQUAL, GREATER_EQUAL = D3D11_COMPARISON_GREATER_EQUAL, ALWAYS_PASS = D3D11_COMPARISON_ALWAYS };
			//�X�e���V���e�X�g���s���֐�
			Function testFunc;
		};
		Face front;
		Face back;
	};
	class DepthStencilState {
	public:
		DepthStencilState(DepthPreset depth_stencil, bool depth, StencilDesc sdesc, bool stencil);
		~DepthStencilState() = default;
	public:
		//Python�p
		void Set();
	private:
		void SettingPreset(D3D11_DEPTH_STENCIL_DESC* desc, DepthPreset preset)const;
	private:
		ComPtr<ID3D11DepthStencilState> state;
	};
	//--------------------------------------End RenderState---------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Buffer
	//
	//--------------------------------------------------------------------------------------------------
	class BufferBase {
	public:
		BufferBase();
		virtual ~BufferBase() = default;
	public:
		//Python�p Buffer���������݉\�ɍ쐬�����ꍇ�̂ݎg�p�\
		void Begin();
		//�����I�C���X�^���X���ɂ���ăo�C���h����
		template<class T> void Set(int byte_pos, std::vector<T> data) {
			T* temp = static_cast<T*>(resuorcePointer);
			int index = byte_pos / 4;
			for each(auto&& d in data) {
				temp[index++] = d;
			}
		}
		void End();
	public:
		//C++�p
		ComPtr<ID3D11Buffer>& Get();
	public:
		//C++�p
		static void Create(ID3D11Buffer** buffer, void* resource, UINT byte_width, D3D11_USAGE usage, UINT bind_flag, UINT cpu_flag, UINT struct_stride, UINT misc_flags = 0);
	private:
		ComPtr<ID3D11Buffer> buffer;
		void* resuorcePointer;
	};
	class VertexBuffer :public BufferBase {
	public:
		VertexBuffer(int struct_size, int vertex_count);
		~VertexBuffer() = default;
		void Activate();
		int GetMaxCount();
	private:
		int STRUCT_SIZE;
		int VERTEX_COUNT;
	};
	class IndexBuffer :public BufferBase {
	public:
		IndexBuffer(int vertex_count);
		//index
		void Activate(int start_vertex_location);
	};
	class ConstantBuffer :public BufferBase {
	public:
		//ShaderStageList���t���O�ɂȂ��Ă܂��B
		ConstantBuffer(UINT slot, UINT stuct_size, byte activate_shader_elements);
		~ConstantBuffer() = default;
		void Activate(void* cb_struct);
	private:
		void CreateConstantBuffer(UINT stuct_size);
		void SetFunctor(byte activate_shader_elements);
	private:
		UINT slot;
		std::vector<std::function<void()>> functor;
	};
	//------------------------------------------End Buffer-------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Shader
	//
	//--------------------------------------------------------------------------------------------------
	class ShaderCompiler {
	public:
		//�t�@�C������R���p�C��
		static void CompileFromFile(const char* file_path, const char* entry_point, const char* shader_model, ID3DBlob** blob);
		//����������R���p�C��
		static void CompileFromMemory(const char* shader_code, int size, const char* entry_point, const char* shader_model, ID3DBlob** blob);
	};
	//�����P�[�W���邽�߂̃C���X�^���X
	class ShaderLinkageInstance final {
		friend class ShaderLinkage;
	public:
		ShaderLinkageInstance() = default;
		~ShaderLinkageInstance() = default;
	public:
		//Python�p
		const std::string& GetName();
	public:
		//C++�p
		ComPtr<ID3D11ClassInstance>& Get();
	private:
		//�C���X�^���X��
		std::string name;
		ComPtr<ID3D11ClassInstance> instance;
	};
	using InstanceID = UINT;
	//�����P�[�W�Ǘ�
	class ShaderLinkage final {
	public:
		ShaderLinkage();
		~ShaderLinkage() = default;
	public:
		//Python�p
		//���t���N�V�������͌��
		//�N���X���Ƀf�[�^�����o�[�����݂��Ȃ��ۂɌĂяo��
		//�F�X�������K�v�ƂȂ肻��
		InstanceID CreateInstance(const char* instance_name);
		//�N���X���Ƀf�[�^�����o�[�����݂���ۂɌĂяo��
		InstanceID GetInstance(const char* instance_name, int instance_index);
	public:
		ComPtr<ID3D11ClassLinkage> classLinkage;
		std::vector<std::shared_ptr<ShaderLinkageInstance>> instances;
		int instanceCount;
	};
	//��Ƀ}�e���A���ŏ��������\��
	class Shader {
		friend class Reflection;
	public:
		Shader(const char* file_path, const char* entry_point, const char* shader_model, ShaderLinkage* linkage);
		Shader(const char* shader_code, int size, const char* entry_point, const char* shader_model, ShaderLinkage* linkage);
		~Shader() = default;
	public:
		//Python�p
		ShaderLinkage * GetLinkage();
		void SetLinkage(std::vector<InstanceID> ids);
		//private:
		//	//C++�p �g���Ǝv��
		//	template<class... Args> void SetLinkage(Args&&... args) {
		//		instanceCount = 0;
		//		using swallow = std::initializer_list<int>;
		//		(void)swallow {
		//			(instances.push_back(linkage->instances[args]->Get().Get()), instanceCount++)...
		//		};
		//	}
	protected:
		std::string GetShaderModelVersionString(ShaderModel shader_model);
	protected:
		ComPtr<ID3DBlob> blob;
		std::unique_ptr<ShaderLinkage> linkage;
		std::vector<ID3D11ClassInstance*> instances;
		int instanceCount;
	};
	class ComputeShader :public Shader {
	public:
		ComputeShader(const char* file_path, const char* entry_point);
		~ComputeShader() = default;
	public:
		//Python�p
		void Run(int thread_x, int thread_y, int thread_z);
	public:
		//static�֐�
		static void SetShaderResourceView(int slot, ID3D11ShaderResourceView* uav);
		static void SetShaderResourceView(int slot, int sum, ID3D11ShaderResourceView** srvs);
		static void SetUnorderedAccessView(int slot, ID3D11UnorderedAccessView* uav);
		static void SetUnorderedAccessView(int slot, int sum, ID3D11UnorderedAccessView** uavs);
	private:
		void CreateComputeShader();
	private:
		ComPtr<ID3D11ComputeShader> cs;
	};
	class VertexShader :public Shader {
		friend class InputLayout;
	public:
		VertexShader(const char* file_path, const char* entry_point, ShaderModel shader_model, bool use_linkage = false);
		VertexShader(const char* shader_code, int size, const char* entry_point, ShaderModel shader_model, bool use_linkage = false);
		~VertexShader() = default;
	public:
		//Python�p
		void Set();
	private:
		void CreateVertexShader();
	private:
		ComPtr<ID3D11VertexShader> vs;
	};
	class PixelShader :public Shader {
	public:
		PixelShader(const char* file_path, const char* entry_point, ShaderModel shader_model, bool use_linkage = false);
		PixelShader(const char* shader_code, int size, const char* entry_point, ShaderModel shader_model, bool use_linkage = false);
		~PixelShader() = default;
	public:
		//Python�p
		void Set();
	private:
		void CreatePixelShader();
	private:
		ComPtr<ID3D11PixelShader> ps;
	};
	class GeometryShader :public Shader {
	public:
		GeometryShader(const char* file_path, const char* entry_point);
		~GeometryShader() = default;
	public:
		//Python�p
		void Set();
	public:
		//Python�p static�֐�
		static void Clean();
	private:
		void CreateGeometryShader();
	private:
		ComPtr<ID3D11GeometryShader> gs;
	};
	class Reflection {
	public:
		Reflection(Shader* shader);
		~Reflection() = default;
	public:
		//C++�p
		const D3D11_SHADER_DESC& GetShaderDesc();
		const D3D11_SIGNATURE_PARAMETER_DESC& GetInputParameterDesc(int index);
		DXGI_FORMAT GetDXGIFormat(int index);
	private:
		void TakeInputparameterDescs();
		DXGI_FORMAT ConverteDXGIFormat(D3D10_REGISTER_COMPONENT_TYPE type, BYTE mask);
	private:
		ComPtr<ID3D11ShaderReflection> reflector;
		D3D11_SHADER_DESC shaderDesc;
		std::vector<D3D11_SIGNATURE_PARAMETER_DESC> inputParameterDescs;
	};
	class InputLayout {
	public:
		InputLayout(VertexShader* vs, Reflection* reflector);
		~InputLayout() = default;
	public:
		//Python�p
		void Set();
	private:
		UINT SlotAssignment(const std::string& semantic);
	private:
		ComPtr<ID3D11InputLayout> inputLayout;
	};
	extern std::string constantBufferInfo;
	//����̍\���ŏ����ꂽ�V�F�[�_�[���C���X�^���V���O�₻�̑��ɑΉ�������
	//�\��
	//���[���h�ϊ���
	//�\�߈�ԏ��float4 pos;�Œ��_����u���Ă���	
	//�����ā�
	//World Converte
	//�Ƃ����R�����g���c���ƁA�����Ɋe���[���h�ϊ��̃R�[�h���p�ӂ����
	class ShaderConverter {
	public:
		static bool Converte3D(const std::string& shader, std::string& normal, std::string& skinning_normal, std::string& instancing, std::string& skinning_instancing);
	private:
		static bool ReplaceString(const std::string& source, const std::string& from, const std::string& replace, std::string& output);
		static bool ConverteNormal3DShader(const std::string& shader, std::string& output);
		static bool ConverteSkinningNormal3DShader(const std::string& shader, std::string& output);
		static bool ConverteInstancing3DShader(const std::string& shader, std::string& output);
		static bool ConverteSkinningInstancing3DShader(const std::string& shader, std::string& output);
	};
	//------------------------------------------End Shader-------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Material
	//
	//--------------------------------------------------------------------------------------------------
	//�Ƃ肠�����Œ��
	//��X�A�ʏ�̃V�F�[�_�[�R�[�h���C���X�^���V���O�p�̃V�F�[�_�[�R�[�h��
	//�X�L�����b�V���p�ɕϊ�������̂�Python��ŏ����Ă�������
	class Material {
	public:
		struct Data {
			float diffuse[4];
			float ambient[4];
			float specular[4];
			float texColor[4];
		};
	public:
		Material(const char* name);
		~Material() = default;
	public:
		//Python�p
		void ActivateState();
		void ActivateShader2D();
		void ActivateShader3D();
		void ChangeDiffuseTexture(const char* file_name);
		std::shared_ptr<Texture> GetDiffuseTexture();
		void ChangeBlendState(std::shared_ptr<BlendState> blend);
		void ChangeSamplerState(std::shared_ptr<SamplerState> sampler);
		void ChangeRasterizerState(std::shared_ptr<RasterizerState> rasterizer);
		void ChangeDepthStencilState(std::shared_ptr<DepthStencilState> depth_stencil);
		std::shared_ptr<BlendState> GetBlendState();
		std::shared_ptr<SamplerState> GetSamplerState();
		std::shared_ptr<RasterizerState> GetRasterizerState();
		std::shared_ptr<DepthStencilState> GetDepthStencilState();
		//�t�@�C���ƃ������̂ق��ɃC���X�^���X�ŕύX�ł���悤�ɂ����邱��
		//�܂��C���X�^���X�̓�����ł���悤�ɂ��鎖
		void ChangeVS2DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS2DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS2DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS2DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS3DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS3DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS3DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS3DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		//��ɏ����� 3D��2D�p�̃V�F�[�_�[�Ƃ����T�O���Ȃ��Ȃ�(�\��)
		std::shared_ptr<VertexShader> GetVS3D();
		std::shared_ptr<InputLayout> GetInputLayout3D();
		std::shared_ptr<PixelShader> GetPS3D();
	private:
		//�f�t�H���g���쐬
		void CreateEmptyMaterial();
		void CreateDefautRenderState();
		void CreateDefaultShaders();
		void CreateInputLayout(std::shared_ptr<VertexShader>& vs, std::shared_ptr<InputLayout>& input_layout);
	private:
		std::string name;
		std::shared_ptr<BlendState> blend;
		std::shared_ptr<SamplerState> sampler;
		std::shared_ptr<RasterizerState> rasterizer;
		std::shared_ptr<DepthStencilState> depthStencil;
		//���HSDSGS��������̂ƁA�X�L�j���O�p��C���X�^���V���O�p��������
		std::shared_ptr<VertexShader> vs2D;
		std::shared_ptr<InputLayout> inputLayout2D;
		std::shared_ptr<PixelShader> ps2D;
		std::shared_ptr<VertexShader> vs3D;
		std::shared_ptr<InputLayout> inputLayout3D;
		std::shared_ptr<PixelShader> ps3D;
		//Texture�̊Ǘ�����C++���ō��
		//���̍�Flyweight��K�p�����̂��A�Ǘ����ł�weak_ptr���g���đO��̉߂�(*)���������
		//*�g���I����Ă��A�v�����I������܂ŉ������Ȃ�
		//���̂����A�����e�N�X�`���ɑΉ�������
		std::shared_ptr<Texture> diffuseTexture;
		//�\�� �C���X�^���V���O���g�p���邩���ǂ����̃t���O
		bool useInstancing;
	};

	//----------------------------------------End Material------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Viewport
	//
	//--------------------------------------------------------------------------------------------------
	//�Ƃ肠�����̊Ԃɍ��킹�A�݌v�͍l���Ă��Ȃ�
	class Viewport {
	public:
		Viewport(int pos_x, int pos_y, int size_x, int size_y);
		~Viewport() = default;
		void Activate();
	private:
		D3D11_VIEWPORT viewport = {};
		int posX;
		int posY;
		int sizeX;
		int sizeY;
	};
	//�r���[�ϊ��Ǝˉe�ϊ��p�N���X
	class Camera {
	private:
		struct CBData {
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX projection;
			DirectX::XMMATRIX billboardMat;
			float pos[4];
			struct Frustum {
				DirectX::XMFLOAT4 center[6];
				DirectX::XMFLOAT4 normal[6];
			}frustum;
		};
	public:
		//Python�p
		Camera(int size_x, int size_y, float fov_rad = PI / 4.0f, float near_z = 1.0f, float far_z = 1000.0f);
		~Camera() = default;
		void SetPos(float x, float y, float z);
		void SetAt(float x, float y, float z);
		void SetUp(float x, float y, float z);
		void Activate();
	private:
		void CreateFrustum();
	private:
		static DirectX::XMMATRIX CreateProjection(float fov_rad, float aspect, float near_z, float far_z);
		static DirectX::XMMATRIX CreateView(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& at, const DirectX::XMFLOAT4&up);
		static DirectX::XMMATRIX CreateBillboardMat(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& at, const DirectX::XMFLOAT4&up);
	private:
		std::unique_ptr<ConstantBuffer> constantBuffer;
		//�v���W�F�N�V�����p
		float nearZ;
		float farZ;
		float fov;
		float aspect;
		//�r���[�p
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 at;
		DirectX::XMFLOAT4 up;
		CBData::Frustum frustum;
	};
	//----------------------------------------End Viewport------------------------------------------
	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Transformer
	//
	//--------------------------------------------------------------------------------------------------
	class Transformer {
	public:
		//Python�p
		Transformer();
		~Transformer() = default;
		//�ړ� �s��v�Z���s���܂�
		void Translation(const Vector3& pos);
		void Translation(float x, float y, float z);
		void TranslationMove(const Vector3& move);
		void TranslationMove(float x, float y, float z);
		//��] �s��v�Z���s���܂�
		void RotationQuaternion(const DirectX::XMVECTOR& quaternion);
		void RotationAxis(const Vector3& axis, float rad);
		void RotationRollPitchYaw(const Vector3& rpy);
		void RotationRollPitchYaw(float x, float y, float z);
		void RotationYAxis(float rad);
		//�g�k �s��v�Z���s���܂�
		void Scalling(const Vector3& scale);
		void Scalling(float x, float y, float z);
		void Scalling(float scale);
		//�X�V����
		void CalcWorldMatrix();
		//�s��擾
		DirectX::XMMATRIX GetTranslateMatrix();
		DirectX::XMMATRIX CalcInverseTranslateMatrix();
		DirectX::XMMATRIX GetScallingMatrix();
		DirectX::XMMATRIX CalcInverseScallingMatrix();
		DirectX::XMMATRIX GetRotationMatrix();
		DirectX::XMMATRIX CalcInverseRotationMatrix();
		//�]�u�s���Ԃ��܂�
		DirectX::XMMATRIX GetWorldMatrix();
		DirectX::XMMATRIX GetWorldMatrixTranspose();
		DirectX::XMMATRIX CalcInverseWorldMatrix();
		const Vector3& GetPos();
		const Vector3& GetScale();
		const Vector3& GetRollPitchYaw();
	private:
		//transform����ړ��s�񐶐�
		void CalcTranslateMatrix();
		//transform����g�k�s�񐶐�
		void CalcScallingMatrix();
	private:
		Vector3 position;
		Vector3 scale;
		Vector3 rpy;
		//���̂���XMFLOAT4X4�ɕϊ�
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX translate;
		DirectX::XMMATRIX scalling;
		DirectX::XMMATRIX rotation;
	};
	//-------------------------------------End Transformer----------------------------------------
	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Renderer
	//
	//--------------------------------------------------------------------------------------------------
	class PolygonRenderer :public VertexBuffer {
	public:
		PolygonRenderer(int struct_size, int vertex_count_max);
		~PolygonRenderer() = default;
	public:
		//Python�p
		void Render(int render_count, PrimitiveTopology topology);
		void RenderIndexed(int render_count, PrimitiveTopology topology);
	private:
		const int STRUCT_SIZE;
		const int VERTEX_COUNT_MAX;
	};
	////�Ƃ肠������3D�̉�����`�悷��Ƃ��납��
	//class Polygon3DRenderer :public VertexBuffer {
	//public:
	//	Polygon3DRenderer(int struct_size, int vertex_count_max);
	//	~Polygon3DRenderer() = default;
	//public:
	//	//Python�p
	//	//void Set(const Transformer& transformer);
	//	void Render(int render_count, PrimitiveTopology topology);
	//	void RenderIndexed(int render_count, PrimitiveTopology topology);
	//private:
	//	Transformer transformer;
	//	//�e�X�g�p
	//	std::unique_ptr<Material> material;
	//private:
	//	const int STRUCT_SIZE;
	//	const int VERTEX_COUNT_MAX;
	//};
	//----------------------------------------End Renderer-----------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Particle
	//
	//--------------------------------------------------------------------------------------------------
	//�C���X�^���X�����������Ԃ�����\������
	//������CPU�ŁA�X�V�A�`���GPU�� 
	//TODO : ��ɒl���s�ς̂��̂́Astatic�ɂ��Ă܂Ƃ߂�
	class GPUParticleSystem {
	public:
		enum class BlendMode { COPY, ADD, SUB, SCREEN };
	private:
		ALIGN(16) struct Info {
		public:
			//���̃t���[���̒ǉ���
			int appendCount = 0;
			//���̃t���[���̌o�ߎ���
			float elapsedTime = 0.0f;
			//�o�C�g�j�b�N�\�[�g�̔�r����v�f�̊Ԋu
			int compareInterval = 0;
			//�����\�[�g���~���\�[�g���̔��ʗp
			BOOL divideLevel = 0;
			BOOL isBitonicFinal = false;
		public:
			void Update(float elapsed_time);
		};
		struct RWByteAddressBuffer :public BufferBase {
			ComPtr<ID3D11UnorderedAccessView> uav;
			RWByteAddressBuffer(void* init_buffer, UINT element_size, UINT element_count, bool is_vertex_buffer, bool is_index_buffer, bool is_indirect_args);
			~RWByteAddressBuffer();
			void ResourceUpdate(void* data_buffer, UINT element_size, UINT element_count);
		private:
			void CreateRWByteAddressBuffer(void* init_buffer, UINT element_size, UINT element_count, bool is_vertex_buffer, bool is_index_buffer, bool is_indirect_args);
			void CreateUAV(ComPtr<ID3D11UnorderedAccessView>& uav);
		};
	public:
		//�p�[�e�B�N������`����\����
		struct Particle {
			float posX;
			float posY;
			float posZ;
			//�ړ���
			float moveX;
			float moveY;
			float moveZ;
			//�����x (���[�g�����b���b)
			float powerX;
			float powerY;
			float powerZ;
			//�g�p����e�N�X�`���̔ԍ�
			int textureIndex;
			//0.0f~1.0f
			float uvPosX;
			float uvPosY;
			float uvSizeX;
			float uvSizeY;
			//��������
			float aliveTime;
			//�o�ߎ���
			float elapsedTime;
			//�t�F�[�h�C���ɂ����鎞��
			float fadeInTime;
			//�t�F�[�h�A�E�g�J�n����
			float fadeOutTime;
			//�J�n���̊g�嗦
			float startScale;
			//�I�����̊g�嗦
			float endScale;
			//�J�n���̉�]�p
			float startRad;
			//�I�����̉�]�p
			float endRad;
			//�␳�F
			float colorRed;
			float colorGreen;
			float colorBlue;
			Particle(float pos_x, float pos_y, float pos_z, float move_x, float move_y, float move_z, float power_x, float power_y, float power_z, int texture_index, float uv_pos_x, float uv_pos_y, float uv_size_y, float uv_size_x, float alive_time, float fade_in_time, float fade_out_time, float start_scale, float end_scale, float start_rad, float end_rad, float color_r, float color_g, float color_b);
			Particle();
			~Particle();
		};
	public:
		GPUParticleSystem();
		~GPUParticleSystem();
		void LoadTexture(int slot, std::string& file_path);
		float GetElapsedTime();
		void Append(const Particle& particle);
		//���{���ɂ��邩�̌���
		void Update(float time_scale = 1.0f);
		void Render(BlendMode mode = BlendMode::ADD);
	private:
		void RunAppend();
		void RunSort();
		void RunUpdate();
	private:
		std::array<std::shared_ptr<BlendState>, 4> blendList;
		std::unique_ptr<Material> material;
		std::unique_ptr<ConstantBuffer> infoCBuffer;
		Info info;
		std::unique_ptr<RWByteAddressBuffer> appendData;
		std::unique_ptr<RWByteAddressBuffer> indexBuffer;
		std::unique_ptr<RWByteAddressBuffer> dataBuffer;
		std::unique_ptr<RWByteAddressBuffer> indirectArgs;
		std::unique_ptr<ComputeShader> appendCS;
		std::unique_ptr<ComputeShader> sortCS;
		std::unique_ptr<ComputeShader> updateCS;
		std::unique_ptr<GeometryShader> gs;
		std::array<std::shared_ptr<Texture>, TEXTURE_COUNT> textureList;
		Particle appendParticles[APPEND_PARTICLE_MAX];
		std::unique_ptr<Timer> timer;
	private:
		static const std::string BLEND_LIST[4];
	};

	//----------------------------------------End Particle-----------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Input
	//
	//--------------------------------------------------------------------------------------------------
	//�f�o�C�X������ static
	class DirectInput {
	public:
		static void Initialize();
		static ComPtr<IDirectInput8>& GetDevice();
	public:
		DirectInput() = default;
		~DirectInput() = default;
		DirectInput(const DirectInput&) = delete;
		DirectInput(DirectInput&&) = delete;
		DirectInput& operator =(const DirectInput&) = delete;
		DirectInput& operator =(DirectInput&&) = delete;
	private:
		static ComPtr<IDirectInput8> device;
	};
	class InputDevice {
	public:
		ComPtr<IDirectInputDevice8>& Get();
		void Initialize(HWND hwnd, const GUID& guid, const DIDATAFORMAT& format, bool fore_ground, bool exclusive);
		//�A�N�Z�X���擾
		bool Acquire();
		HWND GetParentHundle();
	public:
		InputDevice() = default;
		virtual ~InputDevice() = default;
	private:
		ComPtr<IDirectInputDevice8> inputDevice;
		HWND hwnd;
	};
	class Mouse {
	public:
		static void Initialize(void* hwnd, bool fore_ground = true, bool exclusive = false);
		static void Update();
		static BYTE GetKey(int key_code);
		static bool IsPushAnyKey();
		static int PushKeyNo();
		static float GetMoveX();
		static float GetMoveY();
		static int GetWheel();
		static float GetClientPosX();
		static float GetClientPosY();
		static float GetScreenPosX();
		static float GetScreenPosY();
	public:
		Mouse() = default;
		~Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse(Mouse&&) = delete;
		Mouse& operator =(const Mouse&) = delete;
		Mouse& operator =(Mouse&&) = delete;
	private:
		static std::unique_ptr<InputDevice> device;
		static BYTE buffer[3];
		static float move[2];
		static int wheel;
		static float clientPos[2];
		static float screenPos[2];
		static int pushKeyNo;
	};
	//----------------------------------------End Input-----------------------------------------
	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Matrix Helper
	//
	//--------------------------------------------------------------------------------------------------
	std::vector<float> GetAxisXDirection(const DirectX::XMMATRIX& matrix);
	std::vector<float> GetAxisYDirection(const DirectX::XMMATRIX& matrix);
	std::vector<float> GetAxisZDirection(const DirectX::XMMATRIX& matrix);
	std::vector<float> GetPosition(const DirectX::XMMATRIX& matrix);
	//-------------------------------------End Matrix Helper--------------------------------------
}