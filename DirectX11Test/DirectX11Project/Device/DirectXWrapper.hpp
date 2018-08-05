#pragma once

//-------------------------------------------------------------------------------
//
//	Python�p DirectX11���b�p�[
//												author Lobelia
//
//-------------------------------------------------------------------------------

#include <Windows.h>
#include <d3d11.h>
#define _XM_NO_INTRINSICS_
#include "../D3DCompiler/d3dcompiler.h"
#include "../DirectXTex/DirectXTex.h"
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
		void SettingPreset(D3D11_DEPTH_STENCIL_DESC* desc, int preset)const;
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
		void ChangeVS2DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS2DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS2DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS2DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS3DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS3DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS3DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS3DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);

		//void ChangeBlendState();
		//void ChangeSamplerState();
		//void ChangeRasterizerState();
		//void ChangeBlendState();
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
		static DirectX::XMMATRIX CreateProjection(float fov_rad, float aspect, float near_z, float far_z);
		static DirectX::XMMATRIX CreateView(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& at, const DirectX::XMFLOAT4&up);
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
		Transformer transformer;
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
}