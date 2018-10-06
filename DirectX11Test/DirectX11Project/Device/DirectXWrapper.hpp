#pragma once

//-------------------------------------------------------------------------------
//
//	Python用 DirectX11ラッパー
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
	//TODO : デプスステンシルはクラス化すること
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
		//Python用 Bufferを書き込み可能に作成した場合のみ使用可能
		void Begin();
		//明示的インスタンス化によってバインドする
		template<class T> void Set(int byte_pos, std::vector<T> data) {
			T* temp = static_cast<T*>(resuorcePointer);
			int index = byte_pos / 4;
			for each(auto&& d in data) {
				temp[index++] = d;
			}
		}
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
	class IndexBuffer :public BufferBase {
	public:
		IndexBuffer(int vertex_count);
		//index
		void Activate(int start_vertex_location);
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
	class ComputeShader :public Shader {
	public:
		ComputeShader(const char* file_path, const char* entry_point);
		~ComputeShader() = default;
	public:
		//Python用
		void Run(int thread_x, int thread_y, int thread_z);
	public:
		//static関数
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
	class GeometryShader :public Shader {
	public:
		GeometryShader(const char* file_path, const char* entry_point);
		~GeometryShader() = default;
	public:
		//Python用
		void Set();
	public:
		//Python用 static関数
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
	extern std::string constantBufferInfo;
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
		void ChangeBlendState(std::shared_ptr<BlendState> blend);
		void ChangeSamplerState(std::shared_ptr<SamplerState> sampler);
		void ChangeRasterizerState(std::shared_ptr<RasterizerState> rasterizer);
		void ChangeDepthStencilState(std::shared_ptr<DepthStencilState> depth_stencil);
		std::shared_ptr<BlendState> GetBlendState();
		std::shared_ptr<SamplerState> GetSamplerState();
		std::shared_ptr<RasterizerState> GetRasterizerState();
		std::shared_ptr<DepthStencilState> GetDepthStencilState();
		//ファイルとメモリのほかにインスタンスで変更できるようにもすること
		//またインスタンスの入手もできるようにする事
		void ChangeVS2DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS2DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS2DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS2DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS3DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangeVS3DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS3DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage);
		void ChangePS3DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage);
		//後に消える 3Dと2D用のシェーダーという概念がなくなる(予定)
		std::shared_ptr<VertexShader> GetVS3D();
		std::shared_ptr<InputLayout> GetInputLayout3D();
		std::shared_ptr<PixelShader> GetPS3D();
	private:
		//デフォルトを作成
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
	//ビュー変換と射影変換用クラス
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
		//Python用
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
		//プロジェクション用
		float nearZ;
		float farZ;
		float fov;
		float aspect;
		//ビュー用
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
		void RotationRollPitchYaw(float x, float y, float z);
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
	class PolygonRenderer :public VertexBuffer {
	public:
		PolygonRenderer(int struct_size, int vertex_count_max);
		~PolygonRenderer() = default;
	public:
		//Python用
		void Render(int render_count, PrimitiveTopology topology);
		void RenderIndexed(int render_count, PrimitiveTopology topology);
	private:
		const int STRUCT_SIZE;
		const int VERTEX_COUNT_MAX;
	};
	////とりあえずは3Dの何かを描画するところから
	//class Polygon3DRenderer :public VertexBuffer {
	//public:
	//	Polygon3DRenderer(int struct_size, int vertex_count_max);
	//	~Polygon3DRenderer() = default;
	//public:
	//	//Python用
	//	//void Set(const Transformer& transformer);
	//	void Render(int render_count, PrimitiveTopology topology);
	//	void RenderIndexed(int render_count, PrimitiveTopology topology);
	//private:
	//	Transformer transformer;
	//	//テスト用
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
	//インスタンス生成少し時間かかる可能性あり
	//生成はCPUで、更新、描画はGPUで 
	//TODO : 後に値が不変のものは、staticにしてまとめる
	class GPUParticleSystem {
	public:
		enum class BlendMode { COPY, ADD, SUB, SCREEN };
	private:
		ALIGN(16) struct Info {
		public:
			//そのフレームの追加数
			int appendCount = 0;
			//そのフレームの経過時間
			float elapsedTime = 0.0f;
			//バイトニックソートの比較する要素の間隔
			int compareInterval = 0;
			//昇順ソートか降順ソートかの判別用
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
		//パーティクル一つを定義する構造体
		struct Particle {
			float posX;
			float posY;
			float posZ;
			//移動量
			float moveX;
			float moveY;
			float moveZ;
			//加速度 (メートル毎秒毎秒)
			float powerX;
			float powerY;
			float powerZ;
			//使用するテクスチャの番号
			int textureIndex;
			//0.0f~1.0f
			float uvPosX;
			float uvPosY;
			float uvSizeX;
			float uvSizeY;
			//生存時間
			float aliveTime;
			//経過時間
			float elapsedTime;
			//フェードインにかかる時間
			float fadeInTime;
			//フェードアウト開始時間
			float fadeOutTime;
			//開始時の拡大率
			float startScale;
			//終了時の拡大率
			float endScale;
			//開始時の回転角
			float startRad;
			//終了時の回転角
			float endRad;
			//補正色
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
		//何倍速にするかの決定
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
	//デバイスを所持 static
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
		//アクセス権取得
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