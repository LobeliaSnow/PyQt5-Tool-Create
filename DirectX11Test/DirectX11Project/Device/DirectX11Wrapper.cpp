#undef min
#undef max
#include <pybind11/pybind11.h>
//STLを自動でpythonのlistやmapに変換
#include <pybind11/stl.h>
#include <pycapsule.h>
#include <sstream>
#include "DirectXWrapper.hpp"
#include "../Error/ErrorCode.hpp"
#include <wincodec.h>
#include <wincodecsdk.h>
#include <regex>
#pragma comment(lib, "WindowsCodecs.lib")

namespace Lobelia {
	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Device
	//
	//--------------------------------------------------------------------------------------------------
	ComPtr<ID3D11Device> Device::device;
	ComPtr<ID3D11DeviceContext> Device::context;
	void Device::Create(UINT device_flag) {
		Destroy();
		HRESULT hr = S_OK;
		//これを外から決めれるようにするか否か。。。
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3,D3D_FEATURE_LEVEL_9_2,D3D_FEATURE_LEVEL_9_1 };
		IDXGIAdapter* adapter = nullptr;
		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE;
		hr = D3D11CreateDevice(adapter, driverType, nullptr, device_flag, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, device.GetAddressOf(), nullptr, context.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00000);
	}
	void Device::Destroy() {
		context.Reset();
		device.Reset();
	}

	ComPtr<ID3D11Device>& Device::Get() { return device; }
	ComPtr<ID3D11DeviceContext>& Device::GetContext() { return context; }
	//----------------------------------------End Device--------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Texture etc...
	//
	//--------------------------------------------------------------------------------------------------
	Texture::Texture(int x, int y, TextureFormat format, UINT bind_flags, const RTSampleDesc& sample_desc, ACCESS_FLAG access_flag, CPU_ACCESS_FLAG cpu_flag, int array_count) {
		CreateTexture(x, y, format, bind_flags, sample_desc.count, sample_desc.quality, access_flag, cpu_flag, array_count);
		D3D11_TEXTURE2D_DESC desc = {};
		texture->GetDesc(&desc);
		if (desc.BindFlags&D3D11_BIND_SHADER_RESOURCE)	CreateShaderResourceView(format);
	}
	Texture::Texture(const ComPtr<ID3D11Texture2D>& texture) : texture(texture) {
		D3D11_TEXTURE2D_DESC desc = {};
		this->texture->GetDesc(&desc);
		x = static_cast<int>(desc.Width); y = static_cast<int>(desc.Height);
		if (desc.BindFlags&D3D11_BIND_SHADER_RESOURCE)	CreateShaderResourceView(static_cast<TextureFormat>(desc.Format));
	}
	void Texture::Set(int num_slot, ShaderStageList list) {
		switch (list) {
		case ShaderStageList::VS:	Device::GetContext()->VSSetShaderResources(num_slot, 1, view.GetAddressOf());	break;
		case ShaderStageList::PS:	Device::GetContext()->PSSetShaderResources(num_slot, 1, view.GetAddressOf());	break;
		case ShaderStageList::HS:	Device::GetContext()->HSSetShaderResources(num_slot, 1, view.GetAddressOf());	break;
		case ShaderStageList::GS:	Device::GetContext()->GSSetShaderResources(num_slot, 1, view.GetAddressOf());	break;
		case ShaderStageList::DS:	Device::GetContext()->DSSetShaderResources(num_slot, 1, view.GetAddressOf());	break;
		case ShaderStageList::CS:	Device::GetContext()->CSSetShaderResources(num_slot, 1, view.GetAddressOf());	break;
		default:	Error::Message(ErrorCode::DXE01000);
		}
	}
	int Texture::GetWidth() { return x; }
	int Texture::GetHeight() { return y; }
	void Texture::Clean(int num_slot, ShaderStageList list) {
		ID3D11ShaderResourceView* renderTransform = nullptr;
		switch (list) {
		case ShaderStageList::VS:	Device::GetContext()->VSSetShaderResources(num_slot, 1, &renderTransform);	break;
		case ShaderStageList::PS:	Device::GetContext()->PSSetShaderResources(num_slot, 1, &renderTransform);	break;
		case ShaderStageList::HS:	Device::GetContext()->HSSetShaderResources(num_slot, 1, &renderTransform);	break;
		case ShaderStageList::GS:	Device::GetContext()->GSSetShaderResources(num_slot, 1, &renderTransform);	break;
		case ShaderStageList::DS:	Device::GetContext()->DSSetShaderResources(num_slot, 1, &renderTransform);	break;
		case ShaderStageList::CS:	Device::GetContext()->CSSetShaderResources(num_slot, 1, &renderTransform);	break;
		default:	Error::Message(ErrorCode::DXE01000);
		}
	}
	ComPtr<ID3D11ShaderResourceView>& Texture::GetView() { return view; }
	ComPtr<ID3D11Texture2D>& Texture::GetTexture() { return texture; }
	void Texture::CreateTexture(int x, int y, TextureFormat format, UINT bind_flags, int count, int quality, ACCESS_FLAG access_flag, CPU_ACCESS_FLAG cpu_flag, int array_count) {
		HRESULT hr = S_OK;
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = x;
		desc.Height = y;
		desc.MipLevels = 1;
		desc.ArraySize = array_count;
		desc.Format = static_cast<DXGI_FORMAT>(format);
		desc.SampleDesc.Count = count;
		desc.SampleDesc.Quality = quality;
		desc.Usage = static_cast<D3D11_USAGE>(access_flag);
		desc.CPUAccessFlags = static_cast<UINT>(cpu_flag);
		desc.BindFlags = bind_flags;
		hr = Device::Get()->CreateTexture2D(&desc, nullptr, texture.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00003);
	}
	void Texture::CreateShaderResourceView(TextureFormat format) {
		D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
		srvdesc.Format = static_cast<DXGI_FORMAT>(format);
		srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvdesc.Texture2D.MostDetailedMip = 0;
		srvdesc.Texture2D.MipLevels = 1;
		HRESULT hr = Device::Get()->CreateShaderResourceView(texture.Get(), &srvdesc, view.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00004);
	}
	void TextureManager::CreateEmptyTexture(std::shared_ptr<Texture>& texture) {
		RTSampleDesc msaa = { 1,0 };
		static constexpr size_t X = 16UL;
		static constexpr size_t Y = 16UL;
		texture = ResourceBank<Texture>::Factory("Empty", X, Y, TextureFormat::R32G32B32A32_FLOAT, D3D11_BIND_SHADER_RESOURCE, msaa, ACCESS_FLAG::DYNAMIC, CPU_ACCESS_FLAG::WRITE);
		D3D11_MAPPED_SUBRESOURCE resource;
		Device::GetContext()->Map(texture->GetTexture().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		float* ptr = reinterpret_cast<float*>(resource.pData);
		for (int y = 0; y < Y; y++) {
			for (int x = 0; x < X; x++) {
				for (int e = 0; e < 4; e++) {
					*ptr = 1.0f;
					ptr++;
				}
			}
		}
		Device::GetContext()->Unmap(texture->GetTexture().Get(), 0);
	}
	void TextureManager::LoadFile(const wchar_t* file_path, TextureExtension extension, DirectX::TexMetadata* meta, DirectX::ScratchImage& image) {
		HRESULT hr = S_OK;
		switch (extension) {
		case TextureExtension::PNG:
		case TextureExtension::BMP:
		case TextureExtension::JPG:
			hr = DirectX::LoadFromWICFile(file_path, DirectX::WIC_FLAGS_ALL_FRAMES, meta, image);
			break;
		case TextureExtension::TGA:
			hr = DirectX::LoadFromTGAFile(file_path, meta, image);
			break;
		case TextureExtension::NO_SUPPORT:
		default:	Error::Message(ErrorCode::DXE01003);
		}
		if (FAILED(hr))Error::Message(ErrorCode::DXE01004);
	}
	std::shared_ptr<Texture> TextureManager::Load(const char* file_path, bool force) {
		std::shared_ptr<Texture> texture;
		try {
			if (force)ResourceBank<Texture>::Erase(file_path);
			else if (ResourceBank<Texture>::IsExist(file_path)) {
				texture = ResourceBank<Texture>::Get(file_path);
				return texture;
			}
			//からのテクスチャを作るか否か
			if (strcmp(file_path, "") == 0 || FilePathControl::GetFilename(file_path) == ".")throw;
			TextureExtension extension = JudgeExtension(file_path);
			DirectX::TexMetadata meta = {};
			DirectX::ScratchImage image = {};
			std::wstring filePath = ConverteWString(file_path);
			LoadFile(filePath.c_str(), extension, &meta, image);
			ComPtr<ID3D11Texture2D> renderTransform;
			HRESULT hr = DirectX::CreateTexture(Device::Get().Get(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), reinterpret_cast<ID3D11Resource**>(renderTransform.GetAddressOf()));
			if (FAILED(hr))Error::Message(ErrorCode::DXE00003);
			texture = ResourceBank<Texture>::Factory(file_path, renderTransform);
		}
		catch (...) {
			//読み込みや作成に失敗した場合
			CreateEmptyTexture(texture);
			return texture;
		}
		return texture;
	}
	void TextureManager::Save(const char* file_path, Texture* texture) {
		HRESULT 	hr = S_OK;
		DirectX::ScratchImage image = {};
		hr = DirectX::CaptureTexture(Device::Get().Get(), Device::GetContext().Get(), texture->GetTexture().Get(), image);
		if (FAILED(hr))Error::Message(ErrorCode::DXE01005);
		TextureExtension extension = JudgeExtension(file_path);
		GUID guid = {};
		switch (extension) {
		case TextureExtension::PNG:	guid = GUID_ContainerFormatPng;		break;
		case TextureExtension::BMP:	guid = GUID_ContainerFormatBmp;		break;
		case TextureExtension::JPG:	guid = GUID_ContainerFormatJpeg;		break;
		case TextureExtension::TGA:	DirectX::SaveToTGAFile(*image.GetImages(), ConverteWString(file_path).c_str()); return;
		default:
			break;
		}
		hr = DirectX::SaveToWICFile(*image.GetImages(), 0, guid, ConverteWString(file_path).c_str());
		if (FAILED(hr))Error::Message(ErrorCode::DXE01006);
	}
	std::shared_ptr<Texture> TextureManager::CreateNormalMap(Texture* src, float amplitude) {
		HRESULT 	hr = S_OK;
		DirectX::ScratchImage image = {};
		hr = DirectX::CaptureTexture(Device::Get().Get(), Device::GetContext().Get(), src->GetTexture().Get(), image);
		if (FAILED(hr))Error::Message(ErrorCode::DXE01005);
		DirectX::ScratchImage srcImage = {};
		//ちょっと調べる
		//CNMAP_DEFAULT  DXGI_FORMAT_R8G8B8A8_UNORM
		hr = DirectX::ComputeNormalMap(*image.GetImages(), DirectX::CNMAP_DEFAULT, amplitude, DXGI_FORMAT_R8G8B8A8_UNORM, srcImage);
		if (FAILED(hr))Error::Message(ErrorCode::DXE00018);
		RTSampleDesc msaa = { 1,0 };
		//ここ何とかしような
		std::shared_ptr<Texture> normal = std::make_shared<Texture>(src->GetWidth(), src->GetHeight(), TextureFormat::R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE, msaa);
		hr = DirectX::CreateTexture(Device::Get().Get(), srcImage.GetImages(), srcImage.GetImageCount(), srcImage.GetMetadata(), reinterpret_cast<ID3D11Resource**>(normal->GetTexture().GetAddressOf()));
		if (FAILED(hr))Error::Message(ErrorCode::DXE00003);
		return normal;
	}

	RenderTarget::RenderTarget(int x, int y, TextureFormat format, const RTSampleDesc& sample_desc, int array_count) {
		texture = std::make_shared<Texture>(x, y, format, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, sample_desc, ACCESS_FLAG::DEFAULT, CPU_ACCESS_FLAG::NONE, array_count);
		CreateRenderTarget(x, y, sample_desc, array_count);
	}
	RenderTarget::RenderTarget(const ComPtr<ID3D11Texture2D>& texture) {
		this->texture = std::make_shared<Texture>(texture);
		D3D11_TEXTURE2D_DESC desc;
		this->texture->texture->GetDesc(&desc);
		RTSampleDesc sample_desc;
		sample_desc.count = desc.SampleDesc.Count;
		sample_desc.quality = desc.SampleDesc.Quality;
		CreateRenderTarget(desc.Width, desc.Height, sample_desc, 1);
	}
	void RenderTarget::Clear(int a, int r, int g, int b) {
		float clearColor[4] = { static_cast<float>(r) / 255.0f,static_cast<float>(g) / 255.0f,static_cast<float>(b) / 255.0f,static_cast<float>(a) / 255.0f };
		Device::GetContext()->ClearRenderTargetView(renderTarget.Get(), clearColor);
	}
	void RenderTarget::Activate() {
		Device::GetContext()->OMSetRenderTargets(1, renderTarget.GetAddressOf(), depthView.Get());
	}
	void RenderTarget::CreateRenderTarget(int x, int y, const RTSampleDesc& sample_desc, int array_count) {
		HRESULT hr = S_OK;
		if (array_count > 1) {
			D3D11_TEXTURE2D_DESC texDesc;
			texture->texture->GetDesc(&texDesc);
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			desc.Format = texDesc.Format;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.ArraySize = array_count;
			desc.Texture2DArray.MipSlice = 0;
			hr = Device::Get()->CreateRenderTargetView(texture->texture.Get(), &desc, renderTarget.GetAddressOf());
			if (FAILED(hr))Error::Message(ErrorCode::DXE00002);
			oneFaceTarget.resize(array_count);
			desc.Texture2DArray.ArraySize = 1;
			for (int i = 0; i < array_count; i++) {
				desc.Texture2DArray.FirstArraySlice = i;
				hr = Device::Get()->CreateRenderTargetView(texture->texture.Get(), &desc, oneFaceTarget[i].GetAddressOf());
				if (FAILED(hr))Error::Message(ErrorCode::DXE00002);
			}
		}
		else {
			hr = Device::Get()->CreateRenderTargetView(texture->texture.Get(), nullptr, renderTarget.GetAddressOf());
			if (FAILED(hr))Error::Message(ErrorCode::DXE00002);
		}
	}
	void RenderTarget::CreateDepthView() {
		D3D11_TEXTURE2D_DESC tdesc = {};
		depth->texture->GetDesc(&tdesc);
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = tdesc.Format;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = Device::Get()->CreateDepthStencilView(depth->texture.Get(), &desc, depthView.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00005);
	}

	//todo : depth stencil view
	SwapChain::SwapChain(void* hwnd, int x, int y, const RTSampleDesc& sample_desc) {
		CreateSwapChain(static_cast<HWND>(hwnd), x, y, { 1,0 });
		ComPtr<ID3D11Texture2D> backBuffer;
		HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		if (FAILED(hr))Error::Message(ErrorCode::DXE00002);
		rt = std::make_shared<RenderTarget>(backBuffer);
		rt->Activate();
	}
	SwapChain::~SwapChain() {
		swapChain->SetFullscreenState(false, nullptr);
	}
	RenderTarget * SwapChain::GetRenderTarget() { return rt.get(); }
	void SwapChain::Clear(int a, int r, int g, int b) {
		rt->Clear(a, r, g, b);
	}
	void SwapChain::Present(int v_sync) {
		swapChain->Present(v_sync, 0);
	}
	const ComPtr<IDXGISwapChain>& SwapChain::Get() { return swapChain; }
	void SwapChain::CreateSwapChain(HWND hwnd, int x, int y, const RTSampleDesc& sample_desc) {
		ComPtr<IDXGIFactory> factory;
		HRESULT	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));
		if (FAILED(hr))Error::Message(ErrorCode::DXE00001);
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Width = x;
		desc.BufferDesc.Height = y;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = sample_desc.count;
		desc.SampleDesc.Quality = sample_desc.quality;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = hwnd;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		hr = factory->CreateSwapChain(Device::Get().Get(), &desc, swapChain.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00001);
	}
	//--------------------------------------End Texture etc...--------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin RenderState
	//
	//--------------------------------------------------------------------------------------------------
	BlendState::BlendState(BlendPreset preset, bool blend, bool alpha_coverage) {
		HRESULT hr = S_OK;
		D3D11_BLEND_DESC desc = {};
		desc.AlphaToCoverageEnable = alpha_coverage;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = blend;
		SettingPreset(&desc, static_cast<int>(preset));
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = Device::Get()->CreateBlendState(&desc, state.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00014);
	}
	void BlendState::Set() {
		float blendFactory[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
		Device::GetContext()->OMSetBlendState(state.Get(), blendFactory, 0xFFFFFFFF);
	}
	void BlendState::SettingPreset(D3D11_BLEND_DESC* desc, int preset)const {
		BlendPreset p = static_cast<BlendPreset>(preset);
		switch (p) {
		default:
		case BlendPreset::NONE:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;
		case BlendPreset::COPY:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;
		case BlendPreset::ADD:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;
		case BlendPreset::SUB:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;
		case BlendPreset::REPLACE:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;
		case BlendPreset::MULTIPLY:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;
		case BlendPreset::LIGHTENESS:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
			break;
		case BlendPreset::DARKENESS:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;
		case BlendPreset::SCREEN:
			desc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc->RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_COLOR;
			desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
			desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;
		}
	}
	SamplerState::SamplerState(SamplerPreset sampler, int max_anisotropy, bool is_border) {
		HRESULT hr = S_OK;
		D3D11_SAMPLER_DESC desc = {};
		SettingPreset(&desc, static_cast<int>(sampler));
		if (is_border) {
			desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		}
		else {
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		desc.MaxAnisotropy = max_anisotropy;
		desc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = +FLT_MAX;
		DirectX::XMFLOAT4 block(0.0f, 0.0f, 0.0f, 0.0f);
		memcpy(desc.BorderColor, &block, sizeof(DirectX::XMFLOAT4));

		hr = Device::Get()->CreateSamplerState(&desc, state.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00015);
	}
	void SamplerState::SettingPreset(D3D11_SAMPLER_DESC* desc, int preset)const {
		SamplerPreset p = static_cast<SamplerPreset>(preset);
		switch (p) {
		default:
		case	SamplerPreset::POINT:										desc->Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
		case	SamplerPreset::LINEAR:										desc->Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
		case	SamplerPreset::ANISOTROPIC:							desc->Filter = D3D11_FILTER_ANISOTROPIC; break;
		case 	SamplerPreset::COMPARISON_POINT:				desc->Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; break;
		case	SamplerPreset::COMPARISON_LINEAR:				desc->Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; break;
		case	SamplerPreset::COMPARISON_ANISOTROPIC:	desc->Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC; break;
		}
	}
	void SamplerState::Set() {
		Device::GetContext()->PSSetSamplers(0, 1, state.GetAddressOf());
		Device::GetContext()->DSSetSamplers(0, 1, state.GetAddressOf());
	}
	RasterizerState::RasterizerState(RasterizerPreset rasterizer, bool wire_frame) {
		HRESULT hr = S_OK;
		//ラスタライズの設定
		D3D11_RASTERIZER_DESC desc = {};
		//2:ワイヤーフレーム表示 3:面表示
		desc.FillMode = wire_frame ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
		//表面が反時計回りが表(TRUE)か時計回りが表(FALSE)かの設定
		desc.FrontCounterClockwise = false;
		desc.AntialiasedLineEnable = true;
		desc.MultisampleEnable = true;
		SettingPreset(&desc, static_cast<int>(rasterizer));
		hr = Device::Get()->CreateRasterizerState(&desc, state.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00016);
	}
	void RasterizerState::SettingPreset(D3D11_RASTERIZER_DESC* desc, int preset)const {
		RasterizerPreset p = static_cast<RasterizerPreset>(preset);
		switch (p) {
		default:
		case RasterizerPreset::FRONT:	desc->CullMode = D3D11_CULL_FRONT; break;
		case RasterizerPreset::BACK:		desc->CullMode = D3D11_CULL_BACK;	break;
		case RasterizerPreset::NONE:	desc->CullMode = D3D11_CULL_NONE;	break;
		}
	}
	void RasterizerState::Set() {
		Device::GetContext()->RSSetState(state.Get());
	}
	DepthStencilState::DepthStencilState(DepthPreset depth_stencil, bool depth, StencilDesc sdesc, bool stencil) {
		HRESULT hr = S_OK;
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = depth;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = stencil;
		if (stencil) {
			desc.StencilReadMask = sdesc.readMask;
			desc.StencilWriteMask = sdesc.writeMask;
			//表面をどうするか
			desc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(sdesc.front.depthFail);
			desc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(sdesc.front.faile);
			desc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(sdesc.front.pass);
			desc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(sdesc.front.testFunc);
			//裏面をどうするか
			desc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(sdesc.back.depthFail);
			desc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(sdesc.back.faile);
			desc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(sdesc.back.pass);
			desc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(sdesc.back.testFunc);
		}
		SettingPreset(&desc, static_cast<int>(depth_stencil));
		hr = Device::Get()->CreateDepthStencilState(&desc, state.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00017);
	}
	void DepthStencilState::SettingPreset(D3D11_DEPTH_STENCIL_DESC* desc, int preset)const {
		DepthPreset p = static_cast<DepthPreset>(preset);
		switch (p) {
		default:
		case DepthPreset::NEVER:					desc->DepthFunc = D3D11_COMPARISON_NEVER;					break;
		case DepthPreset::LESS:						desc->DepthFunc = D3D11_COMPARISON_LESS;						break;
		case DepthPreset::EQUAL:					desc->DepthFunc = D3D11_COMPARISON_EQUAL;					break;
		case DepthPreset::LESS_EQUAL:			desc->DepthFunc = D3D11_COMPARISON_NEVER;					break;
		case DepthPreset::GREATER:				desc->DepthFunc = D3D11_COMPARISON_GREATER;				break;
		case DepthPreset::NOT_EQUAL:			desc->DepthFunc = D3D11_COMPARISON_NOT_EQUAL;			break;
		case DepthPreset::GREATER_EQUAL:	desc->DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;	break;
		case DepthPreset::ALWAYS:					desc->DepthFunc = D3D11_COMPARISON_ALWAYS;				break;
		}
	}
	void DepthStencilState::Set() {
		Device::GetContext()->OMSetDepthStencilState(state.Get(), 1);
	}
	//--------------------------------------End RenderState---------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Buffer
	//
	//--------------------------------------------------------------------------------------------------
	BufferBase::BufferBase() :resuorcePointer(nullptr) {}
	void BufferBase::Begin() {
		D3D11_MAPPED_SUBRESOURCE resource;
		if (FAILED(Device::GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource))) {
			Error::Message(ErrorCode::DXE01001);
		}
		resuorcePointer = resource.pData;
		if (!resuorcePointer)	Error::Message(ErrorCode::DXE90000);
	}
	void BufferBase::Set(int byte_pos, std::vector<float> data) {
		float* temp = static_cast<float*>(resuorcePointer);
		int index = byte_pos / 4;
		for each(auto&& d in data) {
			temp[index++] = d;
		}
	}
	void BufferBase::End() {
		if (!resuorcePointer) {
			Error::Message(ErrorCode::DXE90000);
			return;
		}
		Device::GetContext()->Unmap(buffer.Get(), 0);
		resuorcePointer = nullptr;
	}

	ComPtr<ID3D11Buffer>& BufferBase::Get() { return buffer; }
	void BufferBase::Create(ID3D11Buffer** buffer, void* resource, UINT byte_width, D3D11_USAGE usage, UINT bind_flag, UINT cpu_flag, UINT struct_stride, UINT misc_flags) {
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = byte_width;
		desc.Usage = usage;
		desc.BindFlags = bind_flag;
		desc.CPUAccessFlags = cpu_flag;
		desc.MiscFlags = misc_flags;
		desc.StructureByteStride = struct_stride;
		HRESULT hr = S_OK;
		if (resource) {
			D3D11_SUBRESOURCE_DATA subResource;
			subResource.pSysMem = resource;
			subResource.SysMemPitch = 0;
			subResource.SysMemSlicePitch = 0;
			hr = Device::Get()->CreateBuffer(&desc, &subResource, buffer);
		}
		else	hr = Device::Get()->CreateBuffer(&desc, nullptr, buffer);

		if (FAILED(hr))Error::Message(ErrorCode::DXE00006);
	}
	VertexBuffer::VertexBuffer(int struct_size, int vertex_count) :STRUCT_SIZE(struct_size), VERTEX_COUNT(vertex_count) {
		BufferBase::Create(Get().GetAddressOf(), nullptr, static_cast<UINT>(struct_size*vertex_count), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0);
	}
	void VertexBuffer::Activate() {
		UINT strides = STRUCT_SIZE;
		UINT offset = 0;
		char buffer[128] = {};
		sprintf_s(buffer, "stride %d\n", strides);
		OutputDebugString(buffer);
		Device::GetContext()->IASetVertexBuffers(0, 1, Get().GetAddressOf(), &strides, &offset);
	}
	int VertexBuffer::GetMaxCount() { return VERTEX_COUNT; }
	//------------------------------------------End Buffer-------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Shader
	//
	//--------------------------------------------------------------------------------------------------
	void ShaderCompiler::CompileFromFile(const char* file_path, const char* entry_point, const char* shader_model, ID3DBlob** blob) {
		HRESULT hr = S_OK;
		ComPtr<ID3DBlob> error = {};
		//これを引数でとるか否かは要調整
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		shaderFlags |= D3DCOMPILE_DEBUG;
#else
		shaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		hr = D3DCompileFromFile(ConverteWString(file_path).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, shaderFlags, 0, blob, &error);
		if (FAILED(hr)) {
			std::stringstream errorMes;
			if (!error)Error::Message(ErrorCode::DXE00007);
			errorMes << "\n//////////////シェーダーコンパイルエラー！//////////////\n\n" << reinterpret_cast<const char*>(error->GetBufferPointer()) << "\n////////////////シェーダーのコンパイルに失敗////////////////\n" << "\n";
			Error::Message(ErrorCode::DXE00007, errorMes.str().c_str());
		}
	}
	void ShaderCompiler::CompileFromMemory(const char* shader_code, int size, const char* entry_point, const char* shader_model, ID3DBlob** blob) {
		HRESULT hr = S_OK;
		ComPtr<ID3DBlob> error = {};
		//これを引数でとるか否かは要調整
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		shaderFlags |= D3DCOMPILE_DEBUG;
#else
		shaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		hr = D3DCompile(shader_code, size, nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, shaderFlags, 0, blob, &error);
		if (FAILED(hr)) {
			std::stringstream errorMes;
			if (!error)Error::Message(ErrorCode::DXE00007);
			errorMes << "\n//////////////シェーダーコンパイルエラー！//////////////\n\n" << reinterpret_cast<const char*>(error->GetBufferPointer()) << "\n////////////////シェーダーのコンパイルに失敗////////////////\n" << "\n";
			Error::Message(ErrorCode::DXE00007, errorMes.str().c_str());
		}
	}
	const std::string& ShaderLinkageInstance::GetName() { return name; }
	ComPtr<ID3D11ClassInstance>& ShaderLinkageInstance::Get() { return instance; }

	ShaderLinkage::ShaderLinkage() :instanceCount(0) {
		Device::Get()->CreateClassLinkage(classLinkage.GetAddressOf());
	}
	InstanceID ShaderLinkage::CreateInstance(const char* instance_name) {
		instances.push_back(std::make_shared<ShaderLinkageInstance>());
		instances[instanceCount]->name = instance_name;
		if (FAILED(classLinkage->CreateClassInstance(instance_name, 0, 0, 0, 0, instances[instanceCount]->instance.GetAddressOf()))) {
			Error::Message(ErrorCode::DXE00008);
		}
		return instanceCount++;
	}
	//クラス内にデータメンバーが存在する際に呼び出す
	InstanceID ShaderLinkage::GetInstance(const char* instance_name, int instance_index) {
		instances.push_back(std::make_shared<ShaderLinkageInstance>());
		instances[instanceCount]->name = instance_name;
		if (FAILED(classLinkage->GetClassInstance(instance_name, instance_index, instances[instanceCount]->instance.GetAddressOf()))) {
			Error::Message(ErrorCode::DXE00009);
		}
		return instanceCount++;
	}

	Shader::Shader(const char* file_path, const char* entry_point, const char* shader_model, ShaderLinkage* linkage) :instanceCount(0) {
		ShaderCompiler::CompileFromFile(file_path, entry_point, shader_model, blob.GetAddressOf());
	}
	Shader::Shader(const char* shader_code, int size, const char* entry_point, const char* shader_model, ShaderLinkage* linkage) : instanceCount(0) {
		ShaderCompiler::CompileFromMemory(shader_code, size, entry_point, shader_model, blob.GetAddressOf());
	}
	std::string Shader::GetShaderModelVersionString(ShaderModel shader_model) {
		std::string modelName;
		switch (shader_model) {
		case ShaderModel::_4_0:	modelName += "4_0"; break;
		case ShaderModel::_5_0:	modelName += "5_0"; break;
		case ShaderModel::_5_1:	modelName += "5_1"; break;
		}
		return modelName;
	}
	ShaderLinkage* Shader::GetLinkage() { return linkage.get(); }
	void Shader::SetLinkage(std::vector<InstanceID> ids) {
		instanceCount = 0;
		int count = static_cast<int>(ids.size());
		for (int i = 0; i < count; i++) {
			instances.push_back(linkage->instances[ids[instanceCount]]->Get().Get());
			instanceCount++;
		}
	}

	VertexShader::VertexShader(const char* file_path, const char* entry_point, ShaderModel shader_model, bool use_linkage) :Shader(file_path, entry_point, ("vs_" + GetShaderModelVersionString(shader_model)).c_str(), use_linkage ? new ShaderLinkage() : nullptr) {
		CreateVertexShader();
	}
	VertexShader::VertexShader(const char* shader_code, int size, const char* entry_point, ShaderModel shader_model, bool use_linkage) : Shader(shader_code, size, entry_point, ("vs_" + GetShaderModelVersionString(shader_model)).c_str(), use_linkage ? new ShaderLinkage() : nullptr) {
		CreateVertexShader();
	}
	void VertexShader::CreateVertexShader() {
		HRESULT hr = Device::Get()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), linkage ? linkage->classLinkage.Get() : nullptr, vs.GetAddressOf());
		if (FAILED(hr)) Error::Message(ErrorCode::DXE00011);
	}
	void VertexShader::Set() { Device::GetContext()->VSSetShader(vs.Get(), instances.data(), instanceCount); }

	PixelShader::PixelShader(const char* file_path, const char* entry_point, ShaderModel shader_model, bool use_linkage) : Shader(file_path, entry_point, ("ps_" + GetShaderModelVersionString(shader_model)).c_str(), use_linkage ? new ShaderLinkage() : nullptr) {
		CreatePixelShader();
	}
	PixelShader::PixelShader(const char* shader_code, int size, const char* entry_point, ShaderModel shader_model, bool use_linkage) : Shader(shader_code, size, entry_point, ("ps_" + GetShaderModelVersionString(shader_model)).c_str(), use_linkage ? new ShaderLinkage() : nullptr) {
		CreatePixelShader();
	}
	void PixelShader::CreatePixelShader() {
		HRESULT hr = Device::Get()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), linkage ? linkage->classLinkage.Get() : nullptr, ps.GetAddressOf());
		if (FAILED(hr)) Error::Message(ErrorCode::DXE00012);
	}
	void PixelShader::Set() { Device::GetContext()->PSSetShader(ps.Get(), instances.data(), instanceCount); }

	Reflection::Reflection(Shader* shader) {
		HRESULT hr = S_OK;
		hr = D3DReflect(shader->blob->GetBufferPointer(), shader->blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(reflector.GetAddressOf()));
		if (FAILED(hr))Error::Message(ErrorCode::DXE00010);
		reflector->GetDesc(&shaderDesc);
		TakeInputparameterDescs();
	}
	void Reflection::TakeInputparameterDescs() {
		inputParameterDescs.resize(shaderDesc.InputParameters);
		for (int index = 0; index < static_cast<int>(shaderDesc.InputParameters); index++) {
			reflector->GetInputParameterDesc(index, &inputParameterDescs[index]);
		}
	}
	DXGI_FORMAT Reflection::ConverteDXGIFormat(D3D10_REGISTER_COMPONENT_TYPE type, BYTE mask) {
		if (mask == 1) {
			if (type == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32_UINT;
			else if (type == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32_SINT;
			else if (type == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32_FLOAT;
		}
		else if (mask <= 3) {
			if (type == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32_UINT;
			else if (type == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32_SINT;
			else if (type == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (mask <= 7) {
			if (type == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32_UINT;
			else if (type == D3D_REGISTER_COMPONENT_SINT32)return DXGI_FORMAT_R32G32B32_SINT;
			else if (type == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (mask <= 15) {
			if (type == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32A32_UINT;
			else if (type == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32A32_SINT;
			else if (type == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
	const D3D11_SHADER_DESC& Reflection::GetShaderDesc() { return shaderDesc; }
	const D3D11_SIGNATURE_PARAMETER_DESC& Reflection::GetInputParameterDesc(int index) { return inputParameterDescs[index]; }
	DXGI_FORMAT Reflection::GetDXGIFormat(int index) {
		D3D11_SIGNATURE_PARAMETER_DESC& desc = inputParameterDescs[index];
		return ConverteDXGIFormat(desc.ComponentType, desc.Mask);
	}

	InputLayout::InputLayout(VertexShader* vs, Reflection* reflector) {
		std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
		D3D11_SHADER_DESC shaderDesc = reflector->GetShaderDesc();
		elements.resize(shaderDesc.InputParameters);
		for (int index = 0; index < static_cast<int>(shaderDesc.InputParameters); index++) {
			D3D11_SIGNATURE_PARAMETER_DESC parameterDesc = reflector->GetInputParameterDesc(index);
			std::string semantic = parameterDesc.SemanticName;
			UINT instanceTag = SlotAssignment(semantic);
			elements[index].SemanticName = parameterDesc.SemanticName;
			elements[index].SemanticIndex = parameterDesc.SemanticIndex;
			elements[index].Format = reflector->GetDXGIFormat(index);
			elements[index].InputSlot = instanceTag;
			elements[index].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elements[index].InputSlotClass = instanceTag == 0 ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
			elements[index].InstanceDataStepRate = instanceTag;
			//Error::PrintBox("Name : %s\nindex : %d\nformat : %d\ninputSlot : %d\nAlignedByteOffset : %d\nInputSlotClass : %d\nInstanceDataStepRate : %d", elements[index].SemanticName, elements[index].SemanticIndex, elements[index].Format, elements[index].InputSlot, elements[index].AlignedByteOffset, elements[index].InputSlotClass, elements[index].InstanceDataStepRate);
		}
		HRESULT hr = Device::Get()->CreateInputLayout(elements.data(), static_cast<UINT>(elements.size()), vs->blob->GetBufferPointer(), vs->blob->GetBufferSize(), inputLayout.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00013);
	}
	UINT InputLayout::SlotAssignment(const std::string& semantic) { return (semantic.find("_I") < UINT_MAX) ? 1 : 0; }
	void InputLayout::Set() { Device::GetContext()->IASetInputLayout(inputLayout.Get()); }
	//この先シェーダーコードの定義
	//とりあえず何も考えず適当に
	//2D
	std::string constantBufferInfo = {
		"cbuffer Model : register(b1) {\n"
		"	float4x4 world;\n"
		"};\n"
		"cbuffer Material : register(b2) {\n"
		"	float4 diffuse;\n"
		"	float4 ambient;\n"
		"	float4 specular;\n"
		"	float4 texColor;\n"
		"};\n"
	};
	std::string header2DInfo = {
		"struct SIMPLE_VS_IN{\n"
		"	float2 pos : POSITION;\n"
		"};\n"
		"struct SIMPLE_PS_IN{\n"
		"	float4 pos : SV_Position;\n"
		"};\n",
	};
	std::string simpleShader2DVS = {
		"SIMPLE_PS_IN SimpleVS(SIMPLE_VS_IN input){ \n"
		"	SIMPLE_PS_IN output = (SIMPLE_PS_IN)0;\n"
		"	output.pos.xy = input.pos;\n"
		"	output.pos.z = 0.0f;\n"
		"	output.pos.w = 1.0f;\n"
		"	return output;\n"
		"}\n"
	};
	std::string simpleShader2DPS = {
		"float4 SimplePS(SIMPLE_PS_IN input) : SV_Target {\n"
		"	return float4(1.0f,1.0f,1.0f,1.0f);\n"
		"}\n"
	};
	//3D
	std::string header3DInfo = {
		"struct SIMPLE_VS_IN{\n"
		"	float2 pos : POSITION;\n"
		"};\n"
		"struct SIMPLE_PS_IN{\n"
		"	float4 pos : SV_Position;\n"
		"};\n",
	};
	std::string simpleShader3DVS = {
		"SIMPLE_PS_IN SimpleVS(SIMPLE_VS_IN input){ \n"
		"	SIMPLE_PS_IN output = (SIMPLE_PS_IN)0;\n"
		"	float4 pos = input.pos;"
		"	//World Converte"
		"	output.pos.xy = pos;\n"
		"	output.pos.z = 0.0f;\n"
		"	output.pos.w = 1.0f;\n"
		"	return output;\n"
		"}\n"
	};
	std::string simpleShader3DPS = {
		"float4 SimplePS(SIMPLE_PS_IN input) : SV_Target {\n"
		"	return float4(1.0f,1.0f,1.0f,1.0f);\n"
		"}\n"
	};

	bool ShaderConverter::Converte3D(const std::string& shader, std::string& normal, std::string& skinning_normal, std::string& instancing, std::string& skinning_instancing) {
		bool ret = true;
		ret &= ConverteNormal3DShader(shader, normal);
		ret &= ConverteSkinningNormal3DShader(shader, skinning_normal);
		ret &= ConverteInstancing3DShader(shader, instancing);
		ret &= ConverteSkinningInstancing3DShader(shader, skinning_instancing);
		return ret;
	}
	bool ShaderConverter::ReplaceString(const std::string& source, const std::string& from, const std::string& replace, std::string& output) {
		const UINT strPos = source.find(from);
		UINT length = from.length();
		output = source;
		if (strPos == std::string::npos || from.empty()) return false;
		output = output.replace(strPos, length, replace);
		return true;
	}
	bool ShaderConverter::ConverteNormal3DShader(const std::string& shader, std::string& output) {
		bool ret = ReplaceString(shader, "//World Converte", "\n	pos = mul(pos, world);\n", output);
		OutputDebugString(output.c_str());
		return ret;
	}
	bool ShaderConverter::ConverteSkinningNormal3DShader(const std::string& shader, std::string& output) {
		bool ret = ReplaceString(shader, "//World Converte", "\n	pos = mul(pos, world)S;\n", output);
		OutputDebugString(output.c_str());
		return ret;
	}
	bool ShaderConverter::ConverteInstancing3DShader(const std::string& shader, std::string& output) {
		bool ret = ReplaceString(shader, "//World Converte", "\n	pos = mul(pos, world)I;\n", output);
		OutputDebugString(output.c_str());
		return ret;
	}
	bool ShaderConverter::ConverteSkinningInstancing3DShader(const std::string& shader, std::string& output) {
		bool ret = ReplaceString(shader, "//World Converte", "\n	pos = mul(pos, world)IS\n;", output);
		OutputDebugString(output.c_str());
		return ret;
	}
	//------------------------------------------End Shader-------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Material
	//
	//--------------------------------------------------------------------------------------------------
	Material::Material(const char* name) :name(name) {
		CreateEmptyMaterial();
	}
	void Material::CreateEmptyMaterial() {
		CreateDefautRenderState();
		CreateDefaultShaders();
	}
	void Material::CreateDefautRenderState() {
		blend = std::make_shared<BlendState>(BlendPreset::COPY, true, false);
		sampler = std::make_shared<SamplerState>(SamplerPreset::ANISOTROPIC, 16);
		rasterizer = std::make_shared<RasterizerState>(RasterizerPreset::NONE);
		depthStencil = std::make_shared<DepthStencilState>(DepthPreset::ALWAYS, false, StencilDesc(), false);
	}
	void Material::CreateDefaultShaders() {
		//シェーダーコード作成
		std::string shader2DCode = constantBufferInfo;
		shader2DCode += header2DInfo;
		shader2DCode += simpleShader2DVS;
		shader2DCode += simpleShader2DPS;
		std::string shader3DCode = constantBufferInfo;
		shader3DCode += header3DInfo;
		shader3DCode += simpleShader3DVS;
		shader3DCode += simpleShader3DPS;
		std::string normal, skinning, instancing, skinningInstancing;
		ShaderConverter::Converte3D(shader3DCode, normal, skinning, instancing, skinningInstancing);
		//この先シェーダー
		vs = std::make_shared<VertexShader>(shader2DCode.c_str(), shader2DCode.size(), "SimpleVS", ShaderModel::_4_0);
		ps = std::make_shared<PixelShader>(shader2DCode.c_str(), shader2DCode.size(), "SimplePS", ShaderModel::_4_0);
		//入力レイアウト
		std::unique_ptr<Reflection> reflector = std::make_unique<Reflection>(vs.get());
		inputLayout = std::make_unique<InputLayout>(vs.get(), reflector.get());
	}
	void Material::Activate() {
		blend->Set();
		sampler->Set();
		rasterizer->Set();
		depthStencil->Set();
		vs->Set();
		ps->Set();
		inputLayout->Set();
	}
	//----------------------------------------End Material------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Viewport
	//
	//--------------------------------------------------------------------------------------------------
	Viewport::Viewport(int pos_x, int pos_y, int size_x, int size_y) {
		viewport.Width = static_cast<FLOAT>(size_x);
		viewport.Height = static_cast<FLOAT>(size_y);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = static_cast<FLOAT>(pos_x);
		viewport.TopLeftY = static_cast<FLOAT>(pos_y);
	}
	void Viewport::Activate() {
		Device::GetContext()->RSSetViewports(1, &viewport);
	}
	//----------------------------------------End Viewport------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Renderer
	//
	//--------------------------------------------------------------------------------------------------
	Polygon2DRenderer::Polygon2DRenderer(int struct_size, int vertex_count_max) :VertexBuffer(struct_size, vertex_count_max), STRUCT_SIZE(struct_size), VERTEX_COUNT_MAX(vertex_count_max) {
	}
	void Polygon2DRenderer::Render(int render_count, PrimitiveTopology topology) {
		Activate();
		Device::GetContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
		Device::GetContext()->Draw(render_count, 0);
	}
	//----------------------------------------End Renderer-----------------------------------------
}

