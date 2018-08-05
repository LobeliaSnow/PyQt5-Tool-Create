#pragma once

//-------------------------------------------------------------------------------
//
//	Python用 DirectX11ラッパー
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

//todo : rendertargetのdepthstencilview作成
//memo : データとレンダラ(処理)は分ける

namespace Lobelia {
	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Device
	//
	//--------------------------------------------------------------------------------------------------
	class Device {
	public:
		//Python用
		static void Create(UINT device_flag = 0);
		static void Destroy();
	public:
		//C++用
		static ComPtr<ID3D11Device>& Get();
		//ディファードコンテキストについて考える
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
		//Python用
		void Set(int num_slot, ShaderStageList list);
		int GetWidth();
		int GetHeight();
	public:
		//C++用
		ComPtr<ID3D11ShaderResourceView>& GetView();
		ComPtr<ID3D11Texture2D>& GetTexture();
	public:
		//Python用
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
		//Python用
		RenderTarget(int x, int y, TextureFormat format, const RTSampleDesc& sample_desc, int array_count = 1);
		//C++ SwapChain用
		RenderTarget(const ComPtr<ID3D11Texture2D>& texture);
		~RenderTarget() = default;
	public:
		//Python用
		void Clear(int a, int r, int g, int b);
		void Activate();
	public:
		//C++用
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
		//HWNDの扱いがわからない。。調査。
		SwapChain(void* hwnd, int x, int y, const RTSampleDesc& sample_desc);
		~SwapChain();
	public:
		//Python用
		RenderTarget * GetRenderTarget();
		void Clear(int a, int r, int g, int b);
		void Present(int v_sync = 0);
	public:
		//C++用
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
		//Python用
		void Set();
	private:
		void SettingPreset(D3D11_BLEND_DESC* desc, int preset)const;
	private:
		/**@brief ブレンドステート本体*/
		ComPtr<ID3D11BlendState> state;
	};
	class SamplerState {
	public:
		SamplerState(SamplerPreset sampler, int max_anisotropy, bool is_border = false);
		~SamplerState() = default;
	public:
		//Python用
		void Set();
	private:
		void SettingPreset(D3D11_SAMPLER_DESC* desc, int preset)const;
	private:
		/**@brief サンプラーステート本体*/
		ComPtr<ID3D11SamplerState> state;
	};
	class RasterizerState {
	public:
		RasterizerState(RasterizerPreset rasterizer, bool wire_frame = false);
		~RasterizerState() = default;
	public:
		//Python用
		void Set();
	private:
		void SettingPreset(D3D11_RASTERIZER_DESC* desc, int preset)const;
	private:
		/**@brief ラスタライザーステート本体*/
		ComPtr<ID3D11RasterizerState> state;
	};
	//ステンシル用
	struct StencilDesc {
		//読み書きしないときは0を指定してね。
		UINT8 readMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		UINT8 writeMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		struct Face {
			enum class Operation :BYTE { KEEP = D3D11_STENCIL_OP_KEEP, ZERO = D3D11_STENCIL_OP_ZERO, REPLACE = D3D11_STENCIL_OP_REPLACE, INCR_CLAMP = D3D11_STENCIL_OP_INCR_SAT, DECR_CLAMP = D3D11_STENCIL_OP_DECR_SAT, INVERT = D3D11_STENCIL_OP_INVERT, INCR_WRAP = D3D11_STENCIL_OP_INCR, DECR_WRAP = D3D11_STENCIL_OP_DECR };
			//テスト失敗
			Operation faile;
			//深度テスト失敗
			Operation depthFail;
			//テスト合格
			Operation pass;
			//左項 ソース値 右項 ステンシル値
			enum class Function :BYTE { ALWAYS_FAIL = D3D11_COMPARISON_NEVER, LESS = D3D11_COMPARISON_LESS, EQUAL = D3D11_COMPARISON_EQUAL, LESS_EQUAL = D3D11_COMPARISON_LESS_EQUAL, GREATER = D3D11_COMPARISON_GREATER, NOT_EQUAL = D3D11_COMPARISON_NOT_EQUAL, GREATER_EQUAL = D3D11_COMPARISON_GREATER_EQUAL, ALWAYS_PASS = D3D11_COMPARISON_ALWAYS };
			//ステンシルテストを行う関数
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
		//Python用
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
		//Python用 Bufferを書き込み可能に作成した場合のみ使用可能
		void Begin();
		void Set(int byte_pos, std::vector<float> data);
		void End();
	public:
		//C++用
		ComPtr<ID3D11Buffer>& Get();
	public:
		//C++用
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
	class ConstantBuffer :public BufferBase {
	public:
		//ShaderStageListがフラグになってます。
		ConstantBuffer(UINT slot, UINT stuct_size, byte activate_shader_elements);
		~ConstantBuffer() = default;
		void Activate(void* cb_struct);
	private:
		void CreateConstantBuffer(UINT stuct_size);
		void SetFunctor(byte activate_shader_elements);
	private:
		ComPtr<ID3D11Buffer> buffer;
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
		//ファイルからコンパイル
		static void CompileFromFile(const char* file_path, const char* entry_point, const char* shader_model, ID3DBlob** blob);
		//メモリからコンパイル
		static void CompileFromMemory(const char* shader_code, int size, const char* entry_point, const char* shader_model, ID3DBlob** blob);
	};
	//リンケージするためのインスタンス
	class ShaderLinkageInstance final {
		friend class ShaderLinkage;
	public:
		ShaderLinkageInstance() = default;
		~ShaderLinkageInstance() = default;
	public:
		//Python用
		const std::string& GetName();
	public:
		//C++用
		ComPtr<ID3D11ClassInstance>& Get();
	private:
		//インスタンス名
		std::string name;
		ComPtr<ID3D11ClassInstance> instance;
	};
	using InstanceID = UINT;
	//リンケージ管理
	class ShaderLinkage final {
	public:
		ShaderLinkage();
		~ShaderLinkage() = default;
	public:
		//Python用
		//リフレクション等は後程
		//クラス内にデータメンバーが存在しない際に呼び出す
		//色々調整が必要となりそう
		InstanceID CreateInstance(const char* instance_name);
		//クラス内にデータメンバーが存在する際に呼び出す
		InstanceID GetInstance(const char* instance_name, int instance_index);
	public:
		ComPtr<ID3D11ClassLinkage> classLinkage;
		std::vector<std::shared_ptr<ShaderLinkageInstance>> instances;
		int instanceCount;
	};
	//後にマテリアルで所持される予定
	class Shader {
		friend class Reflection;
	public:
		Shader(const char* file_path, const char* entry_point, const char* shader_model, ShaderLinkage* linkage);
		Shader(const char* shader_code, int size, const char* entry_point, const char* shader_model, ShaderLinkage* linkage);
		~Shader() = default;
	public:
		//Python用
		ShaderLinkage * GetLinkage();
		void SetLinkage(std::vector<InstanceID> ids);
		//private:
		//	//C++用 使わんと思う
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
		//Python用
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
		//Python用
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
		//C++用
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
		//Python用
		void Set();
	private:
		UINT SlotAssignment(const std::string& semantic);
	private:
		ComPtr<ID3D11InputLayout> inputLayout;
	};
	//特定の構文で書かれたシェーダーをインスタンシングやその他に対応させる
	//構文
	//ワールド変換編
	//予め一番上にfloat4 pos;で頂点情報を置いておく	
	//そして↓
	//World Converte
	//というコメントを残すと、そこに各ワールド変換のコードが用意される
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
	//とりあえず最低限
	//後々、通常のシェーダーコードをインスタンシング用のシェーダーコードや
	//スキンメッシュ用に変換するものをPython上で書いていきたい
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
		//Python用
		void ActivateState();
		void ActivateShader2D();
		void ActivateShader3D();
		void ChangeDiffuseTexture(const char* file_name);
		std::shared_ptr<Texture> GetDiffuseTexture();
		void ChangeVS2DMemory(const char* data, int data_len, const char* entry, ShaderModel model, bool use_linkage);
		void ChangeVS2DFile(const char* file_path, const char* entry, ShaderModel model, bool use_linkage);
		void ChangePS2DMemory(const char* data, int data_len, const char* entry, ShaderModel model, bool use_linkage);
		void ChangePS2DFile(const char* file_path, const char* entry, ShaderModel model, bool use_linkage);
		void ChangeVS3DMemory(const char* data, int data_len, const char* entry, ShaderModel model, bool use_linkage);
		void ChangeVS3DFile(const char* file_path, const char* entry, ShaderModel model, bool use_linkage);
		void ChangePS3DMemory(const char* data, int data_len, const char* entry, ShaderModel model, bool use_linkage);
		void ChangePS3DFile(const char* file_path, const char* entry, ShaderModel model, bool use_linkage);

		//void ChangeBlendState();
		//void ChangeSamplerState();
		//void ChangeRasterizerState();
		//void ChangeBlendState();
	private:
		//デフォルトを作成
		void CreateEmptyMaterial();
		void CreateDefautRenderState();
		void CreateDefaultShaders();
		void CreateInputLayout(std::shared_ptr<VertexShader> vs, std::shared_ptr<InputLayout> input_layout);
	private:
		std::string name;
		std::shared_ptr<BlendState> blend;
		std::shared_ptr<SamplerState> sampler;
		std::shared_ptr<RasterizerState> rasterizer;
		std::shared_ptr<DepthStencilState> depthStencil;
		//後にHSDSGS等増えるのと、スキニング用やインスタンシング用も増える
		std::shared_ptr<VertexShader> vs2D;
		std::shared_ptr<InputLayout> inputLayout2D;
		std::shared_ptr<PixelShader> ps2D;
		std::shared_ptr<VertexShader> vs3D;
		std::shared_ptr<InputLayout> inputLayout3D;
		std::shared_ptr<PixelShader> ps3D;
		//Textureの管理棟はC++側で作る
		//その際Flyweightを適用したのち、管理側ではweak_ptrを使って前回の過ち(*)を訂正する
		//*使い終わってもアプリが終了するまで解放されない
		//そのうち、複数テクスチャに対応したい
		std::shared_ptr<Texture> diffuseTexture;
		//予約 インスタンシングを使用するかかどうかのフラグ
		bool useInstancing;
	};
	
	//----------------------------------------End Material------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Viewport
	//
	//--------------------------------------------------------------------------------------------------
	//とりあえずの間に合わせ、設計は考えていない
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
	//----------------------------------------End Viewport------------------------------------------
	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Transformer
	//
	//--------------------------------------------------------------------------------------------------
	class Transformer {
	public:
		//Python用
		Transformer();
		~Transformer() = default;
		//移動 行列計算も行われます
		void Translation(const Vector3& pos);
		void Translation(float x, float y, float z);
		void TranslationMove(const Vector3& move);
		void TranslationMove(float x, float y, float z);
		//回転 行列計算も行われます
		void RotationQuaternion(const DirectX::XMVECTOR& quaternion);
		void RotationAxis(const Vector3& axis, float rad);
		void RotationRollPitchYaw(const Vector3& rpy);
		void RotationYAxis(float rad);
		//拡縮 行列計算も行われます
		void Scalling(const Vector3& scale);
		void Scalling(float x, float y, float z);
		void Scalling(float scale);
		//更新処理
		void CalcWorldMatrix();
		//行列取得
		DirectX::XMMATRIX GetTranslateMatrix();
		DirectX::XMMATRIX CalcInverseTranslateMatrix();
		DirectX::XMMATRIX GetScallingMatrix();
		DirectX::XMMATRIX CalcInverseScallingMatrix();
		DirectX::XMMATRIX GetRotationMatrix();
		DirectX::XMMATRIX CalcInverseRotationMatrix();
		//転置行列を返します
		DirectX::XMMATRIX GetWorldMatrix();
		DirectX::XMMATRIX GetWorldMatrixTranspose();
		DirectX::XMMATRIX CalcInverseWorldMatrix();
		const Vector3& GetPos();
		const Vector3& GetScale();
		const Vector3& GetRollPitchYaw();
	private:
		//transformから移動行列生成
		void CalcTranslateMatrix();
		//transformから拡縮行列生成
		void CalcScallingMatrix();
	private:
		Vector3 position;
		Vector3 scale;
		Vector3 rpy;
		//そのうちXMFLOAT4X4に変換
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
	class Polygon2DRenderer :public VertexBuffer {
	public:
		Polygon2DRenderer(int struct_size, int vertex_count_max);
		~Polygon2DRenderer() = default;
	public:
		//Python用
		void Render(int render_count, PrimitiveTopology topology);
	private:
		const int STRUCT_SIZE;
		const int VERTEX_COUNT_MAX;
	};
	//とりあえずは3Dの何かを描画するところから
	class Polygon3DRenderer :public VertexBuffer {
	public:
		Polygon3DRenderer(int struct_size, int vertex_count_max);
		~Polygon3DRenderer() = default;
	public:
		//Python用
		void Set(const Transformer& transformer);
		void Render(int render_count, PrimitiveTopology topology);
	private:
		Transformer transformer;
		//テスト用
		std::unique_ptr<Material> material;
	private:
		const int STRUCT_SIZE;
		const int VERTEX_COUNT_MAX;
	};
	//----------------------------------------End Renderer-----------------------------------------
}