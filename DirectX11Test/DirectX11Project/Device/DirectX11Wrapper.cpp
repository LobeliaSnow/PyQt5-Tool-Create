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
		x = i_cast(desc.Width); y = i_cast(desc.Height);
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
		default:
			Error::Message(ErrorCode::DXE01003);
			throw;
		}
		if (FAILED(hr)) {
			Error::Message(ErrorCode::DXE01004);
			throw;
		}
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
			std::string fileName = FilePathControl::GetFilename(file_path);
			if (strcmp(file_path, "") == 0 || fileName == "." || fileName == "") {
				//読み込みや作成に失敗した場合
				CreateEmptyTexture(texture);
				return texture;
			}
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
		depth = std::make_shared<Texture>(x, y, TextureFormat::D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, sample_desc, ACCESS_FLAG::DEFAULT, CPU_ACCESS_FLAG::NONE, array_count);
		CreateRenderTarget(x, y, sample_desc, array_count);
		CreateDepthView();
	}
	RenderTarget::RenderTarget(const ComPtr<ID3D11Texture2D>& texture) {
		this->texture = std::make_shared<Texture>(texture);
		D3D11_TEXTURE2D_DESC desc;
		this->texture->texture->GetDesc(&desc);
		RTSampleDesc sample_desc;
		sample_desc.count = desc.SampleDesc.Count;
		sample_desc.quality = desc.SampleDesc.Quality;
		CreateRenderTarget(desc.Width, desc.Height, sample_desc, 1);
		depth = std::make_shared<Texture>(desc.Width, desc.Height, TextureFormat::D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, sample_desc, ACCESS_FLAG::DEFAULT, CPU_ACCESS_FLAG::NONE, 1);
		CreateDepthView();
	}
	void RenderTarget::Clear(int a, int r, int g, int b) {
		float clearColor[4] = { f_cast(r) / 255.0f,f_cast(g) / 255.0f,f_cast(b) / 255.0f,f_cast(a) / 255.0f };
		Device::GetContext()->ClearRenderTargetView(renderTarget.Get(), clearColor);
		Device::GetContext()->ClearDepthStencilView(depthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
		SettingPreset(&desc, i_cast(preset));
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
		SettingPreset(&desc, i_cast(sampler));
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
		SettingPreset(&desc, i_cast(rasterizer));
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
		//SettingPreset(&desc, depth_stencil);
		hr = Device::Get()->CreateDepthStencilState(&desc, state.GetAddressOf());
		if (FAILED(hr))Error::Message(ErrorCode::DXE00017);
	}
	void DepthStencilState::SettingPreset(D3D11_DEPTH_STENCIL_DESC* desc, DepthPreset preset)const {
		switch (preset) {
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
		Device::GetContext()->IASetVertexBuffers(0, 1, Get().GetAddressOf(), &strides, &offset);
	}
	int VertexBuffer::GetMaxCount() { return VERTEX_COUNT; }
	IndexBuffer::IndexBuffer(int vertex_count) {
		BufferBase::Create(Get().GetAddressOf(), nullptr, static_cast<UINT>(sizeof(UINT)*vertex_count), D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0);
	}
	void IndexBuffer::Activate(int start_vertex_location) {
		Device::GetContext()->IASetIndexBuffer(Get().Get(), DXGI_FORMAT_R32_UINT, start_vertex_location * 4);
	}
	ConstantBuffer::ConstantBuffer(UINT slot, UINT stuct_size, byte activate_shader_elements) :slot(slot) {
		SetFunctor(activate_shader_elements);
		CreateConstantBuffer(stuct_size);
	}
	//TODO  : BufferCreatorを使って書き直すこと
	void ConstantBuffer::CreateConstantBuffer(UINT stuct_size) {
		BufferBase::Create(Get().GetAddressOf(), nullptr, stuct_size, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0);
	}
	void ConstantBuffer::SetFunctor(byte activate_shader_elements) {
		if (activate_shader_elements & i_cast(ShaderStageList::VS))	functor.push_back([this]() {Device::GetContext()->VSSetConstantBuffers(slot, 1, Get().GetAddressOf()); });
		if (activate_shader_elements & i_cast(ShaderStageList::PS))	functor.push_back([this]() {Device::GetContext()->PSSetConstantBuffers(slot, 1, Get().GetAddressOf()); });
		if (activate_shader_elements & i_cast(ShaderStageList::HS))	functor.push_back([this]() {Device::GetContext()->HSSetConstantBuffers(slot, 1, Get().GetAddressOf()); });
		if (activate_shader_elements & i_cast(ShaderStageList::GS))	functor.push_back([this]() {Device::GetContext()->GSSetConstantBuffers(slot, 1, Get().GetAddressOf()); });
		if (activate_shader_elements & i_cast(ShaderStageList::DS))	functor.push_back([this]() {Device::GetContext()->DSSetConstantBuffers(slot, 1, Get().GetAddressOf()); });
		if (activate_shader_elements & i_cast(ShaderStageList::CS))	functor.push_back([this]() {Device::GetContext()->CSSetConstantBuffers(slot, 1, Get().GetAddressOf()); });
	}
	void ConstantBuffer::Activate(void* cb_struct) {
		Device::GetContext()->UpdateSubresource(Get().Get(), 0, nullptr, cb_struct, 0, 0);
		for each(auto&& Set in functor) {
			Set();
		}
	}

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
		int count = i_cast(ids.size());
		for (int i = 0; i < count; i++) {
			instances.push_back(linkage->instances[ids[instanceCount]]->Get().Get());
			instanceCount++;
		}
	}

	ComputeShader::ComputeShader(const char* file_path, const char* entry_point) :Shader(file_path, entry_point, "cs_5_0", nullptr) {
		CreateComputeShader();
	}
	void ComputeShader::CreateComputeShader() {
		HRESULT hr = Device::Get()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, cs.GetAddressOf());
		if (FAILED(hr)) Error::Message(ErrorCode::DXE00019);
	}
	void ComputeShader::SetShaderResourceView(int slot, ID3D11ShaderResourceView* uav) {
		Device::GetContext()->CSSetShaderResources(slot, 1, &uav);
	}
	void ComputeShader::SetShaderResourceView(int slot, int sum, ID3D11ShaderResourceView** srvs) {
		Device::GetContext()->CSSetShaderResources(slot, sum, srvs);
	}
	void ComputeShader::SetUnorderedAccessView(int slot, ID3D11UnorderedAccessView* uav) {
		Device::GetContext()->CSSetUnorderedAccessViews(slot, 1, &uav, nullptr);
	}
	void ComputeShader::SetUnorderedAccessView(int slot, int sum, ID3D11UnorderedAccessView** uavs) {
		Device::GetContext()->CSSetUnorderedAccessViews(slot, sum, uavs, nullptr);
	}
	void ComputeShader::Run(int thread_x, int thread_y, int thread_z) {
		Device::GetContext()->CSSetShader(cs.Get(), nullptr, 0);
		Device::GetContext()->Dispatch(thread_x, thread_y, thread_z);
		Device::GetContext()->CSSetShader(nullptr, nullptr, 0);
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

	GeometryShader::GeometryShader(const char* file_path, const char* entry_point) :Shader(file_path, entry_point, "gs_5_0", nullptr) {
		CreateGeometryShader();
	}
	void GeometryShader::Set() {
		Device::GetContext()->GSSetShader(gs.Get(), instances.data(), instanceCount);
	}
	void GeometryShader::Clean() {
		Device::GetContext()->GSSetShader(nullptr, nullptr, 0);
	}
	void GeometryShader::CreateGeometryShader() {
		HRESULT hr = Device::Get()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, gs.GetAddressOf());
		if (FAILED(hr)) Error::Message(ErrorCode::DXE00020);
	}

	Reflection::Reflection(Shader* shader) {
		HRESULT hr = S_OK;
		hr = D3DReflect(shader->blob->GetBufferPointer(), shader->blob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(reflector.GetAddressOf()));
		if (FAILED(hr))Error::Message(ErrorCode::DXE00010);
		reflector->GetDesc(&shaderDesc);
		TakeInputparameterDescs();
	}
	void Reflection::TakeInputparameterDescs() {
		inputParameterDescs.resize(shaderDesc.InputParameters);
		for (int index = 0; index < i_cast(shaderDesc.InputParameters); index++) {
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
		for (int index = 0; index < i_cast(shaderDesc.InputParameters); index++) {
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
	std::string constantBufferInfo = {
		"cbuffer Camera : register(b0) {\n"
		"	column_major float4x4 view;\n"
		"	column_major float4x4 projection;\n"
		"	column_major float4x4 billboardMat;\n"
		"	float4 cpos;\n"
		"struct Frustum{\n"
		"	float4 center[6];\n"
		"	float4 normal[6];\n"
		"} frustum;\n"
		"};\n"
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
		"	float3 pos : POSITION;\n"
		"};\n"
		"struct SIMPLE_PS_IN{\n"
		"	float4 pos : SV_Position;\n"
		"};\n",
	};
	std::string simpleShader3DVS = {
		"SIMPLE_PS_IN SimpleVS(SIMPLE_VS_IN input){ \n"
		"	SIMPLE_PS_IN output = (SIMPLE_PS_IN)0;\n"
		"	float4 pos = (float4)0;\n"
		"	pos.xyz = input.pos;\n"
		"	pos.w = 1.0f;\n"
		"	//World Converte\n"
		"	output.pos = mul(pos, view);\n"
		"	output.pos = mul(output.pos, projection);\n"
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
		rasterizer = std::make_shared<RasterizerState>(RasterizerPreset::BACK);
		depthStencil = std::make_shared<DepthStencilState>(DepthPreset::ALWAYS, true, StencilDesc(), false);
	}
	//現状構想は、プログラマブルにできる関数を用意してそこにセマンティック通りに送り付ける
	//2Dも3Dもストリームに流れる情報量は同じにすることで、同じシェーダーを使えるのではないかという目論見
	//なので2Dも法線を持つし、接線も持つ
	//これで同じマテリアルを使いまわして2Dも3Dも一つのシェーダーに見せかけることができるのではないかな
	//具体的な方法はまだ考えていないが、シェーダーリンケージがワンチャン使えるかも。
	//モデルの情報だけ所持しているクラスも出る予定
	//複数テクスチャ用にUVも1メッシュ当たり4つまで許容予定、所持してなければ全て0番目のUVが入る
	//ステートに関して、メッシュフィルターなるものを作ってそいつがモデルデータを持って且つ、ラスタライザーやトポロジも持つようになる予定
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
		//std::string normal, skinning, instancing, skinningInstancing;
		//とりあえず放置
		//ShaderConverter::Converte3D(shader3DCode, normal, skinning, instancing, skinningInstancing);
		//この先シェーダー とりあえず適当
		ChangeVS2DMemory(shader2DCode, shader2DCode.size(), "SimpleVS", ShaderModel::_4_0, false);
		ChangePS2DMemory(shader2DCode, shader2DCode.size(), "SimplePS", ShaderModel::_4_0, false);
		ChangeVS3DMemory(shader3DCode, shader3DCode.size(), "SimpleVS", ShaderModel::_4_0, false);
		ChangePS3DMemory(shader3DCode, shader3DCode.size(), "SimplePS", ShaderModel::_4_0, false);
	}
	void Material::ActivateState() {
		blend->Set();
		sampler->Set();
		rasterizer->Set();
		depthStencil->Set();
		if (diffuseTexture)diffuseTexture->Set(0, ShaderStageList::PS);
	}
	void Material::ActivateShader2D() {
		vs2D->Set();
		inputLayout2D->Set();
		ps2D->Set();
	}
	void Material::ActivateShader3D() {
		vs3D->Set();
		inputLayout3D->Set();
		ps3D->Set();
	}
	void Material::ChangeDiffuseTexture(const char* file_name) {
		diffuseTexture = TextureManager::Load(file_name);
	}
	void Material::CreateInputLayout(std::shared_ptr<VertexShader>& vs, std::shared_ptr<InputLayout>& inputLayout) {
		//入力レイアウト
		std::unique_ptr<Reflection> reflector = std::make_unique<Reflection>(vs.get());
		inputLayout = std::make_shared<InputLayout>(vs.get(), reflector.get());
	}
	std::shared_ptr<Texture> Material::GetDiffuseTexture() { return diffuseTexture; }
	void Material::ChangeBlendState(std::shared_ptr<BlendState> blend) { this->blend = blend; }
	void Material::ChangeSamplerState(std::shared_ptr<SamplerState> sampler) { this->sampler = sampler; }
	void Material::ChangeRasterizerState(std::shared_ptr<RasterizerState> rasterizer) { this->rasterizer = rasterizer; }
	void Material::ChangeDepthStencilState(std::shared_ptr<DepthStencilState> depth_stencil) { depthStencil = depth_stencil; }
	std::shared_ptr<BlendState> Material::GetBlendState() { return blend; }
	std::shared_ptr<SamplerState> Material::GetSamplerState() { return sampler; }
	std::shared_ptr<RasterizerState> Material::GetRasterizerState() { return rasterizer; }
	std::shared_ptr<DepthStencilState> Material::GetDepthStencilState() { return depthStencil; }

	void Material::ChangeVS2DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage) {
		vs2D = std::make_shared<VertexShader>(data.c_str(), data_len, entry.c_str(), model, use_linkage);
		CreateInputLayout(vs2D, inputLayout2D);
	}
	void Material::ChangeVS2DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage) {
		vs2D = std::make_shared<VertexShader>(file_path.c_str(), entry.c_str(), model, use_linkage);
		CreateInputLayout(vs2D, inputLayout2D);
	}
	void Material::ChangePS2DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage) {
		ps2D = std::make_shared<PixelShader>(data.c_str(), data_len, entry.c_str(), model, use_linkage);
	}
	void Material::ChangePS2DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage) {
		ps2D = std::make_shared<PixelShader>(file_path.c_str(), entry.c_str(), model, use_linkage);
	}

	void Material::ChangeVS3DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage) {
		vs3D = std::make_shared<VertexShader>(data.c_str(), data_len, entry.c_str(), model, use_linkage);
		CreateInputLayout(vs3D, inputLayout3D);
	}
	void Material::ChangeVS3DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage) {
		vs3D = std::make_shared<VertexShader>(file_path.c_str(), entry.c_str(), model, use_linkage);
		CreateInputLayout(vs3D, inputLayout3D);
	}
	void Material::ChangePS3DMemory(std::string data, int data_len, std::string entry, ShaderModel model, bool use_linkage) {
		ps3D = std::make_shared<PixelShader>(data.c_str(), data_len, entry.c_str(), model, use_linkage);
	}
	void Material::ChangePS3DFile(std::string file_path, std::string entry, ShaderModel model, bool use_linkage) {
		ps3D = std::make_shared<PixelShader>(file_path.c_str(), entry.c_str(), model, use_linkage);
	}
	//後に消える 3Dと2D用のシェーダーという概念がなくなる(予定)
	std::shared_ptr<VertexShader> Material::GetVS3D() { return vs3D; }
	std::shared_ptr<InputLayout> Material::GetInputLayout3D() { return inputLayout3D; }
	std::shared_ptr<PixelShader> Material::GetPS3D() { return ps3D; }

	//----------------------------------------End Material------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Viewport
	//
	//--------------------------------------------------------------------------------------------------
	Viewport::Viewport(int pos_x, int pos_y, int size_x, int size_y) {
		viewport.Width = f_cast(size_x);
		viewport.Height = f_cast(size_y);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = f_cast(pos_x);
		viewport.TopLeftY = f_cast(pos_y);
	}
	void Viewport::Activate() {
		Device::GetContext()->RSSetViewports(1, &viewport);
	}
	Camera::Camera(int size_x, int  size_y, float fov_rad, float near_z, float far_z) :fov(fov_rad), nearZ(near_z), farZ(far_z) {
		constantBuffer = std::make_unique<ConstantBuffer>(0, sizeof(CBData), i_cast(ShaderStageList::VS) | i_cast(ShaderStageList::DS) | i_cast(ShaderStageList::GS));
		aspect = f_cast(size_x) / f_cast(size_y);
	}
	void Camera::SetPos(float x, float y, float z) { pos.x = x; pos.y = y; pos.z = z; pos.w = 1.0f; }
	void Camera::SetAt(float x, float y, float z) { at.x = x; at.y = y; at.z = z; at.w = 1.0f; }
	void Camera::SetUp(float x, float y, float z) { up.x = x; up.y = y; up.z = z; up.w = 1.0f; }
	void Camera::Activate() {
		CBData data = {};
		data.projection = DirectX::XMMatrixTranspose(CreateProjection(fov, aspect, nearZ, farZ));
		data.view = DirectX::XMMatrixTranspose(CreateView(pos, at, up));
		data.billboardMat = DirectX::XMMatrixTranspose(CreateBillboardMat(pos, at, up));
		data.frustum = frustum;
		constantBuffer->Activate(&data);
	}
	void Camera::CreateFrustum() {
		//情報をロード
		DirectX::XMVECTOR dAt = DirectX::XMLoadFloat4(&at);
		DirectX::XMVECTOR dPos = DirectX::XMLoadFloat4(&pos);
		DirectX::XMVECTOR dUp = DirectX::XMLoadFloat4(&up);
		//三軸算出
		DirectX::XMVECTOR camZ = DirectX::XMVector3Normalize(dAt - dPos);
		DirectX::XMVECTOR camX = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(dUp, camZ));
		DirectX::XMVECTOR camY = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(camZ, camX));
		float nearHeight = tanf(fov*0.5f)*nearZ;
		float farHeight = tanf(fov*0.5f)*farZ;
		//視錘台の幅取得
		float nearWidth = nearHeight * aspect;
		float farWidth = farHeight * aspect;
		//中心地算出
		DirectX::XMVECTOR nearPlaneCenter = dPos + camZ * nearZ;
		DirectX::XMVECTOR farPlaneCenter = dPos + camZ * farZ;
		//視錘台頂点算出
		DirectX::XMVECTOR frustumVertices[8];
		frustumVertices[0] = nearPlaneCenter - camX * nearWidth + camY * nearHeight;
		frustumVertices[1] = nearPlaneCenter - camX * nearWidth - camY * nearHeight;
		frustumVertices[2] = nearPlaneCenter + camX * nearWidth - camY * nearHeight;
		frustumVertices[3] = nearPlaneCenter + camX * nearWidth + camY * nearHeight;
		frustumVertices[4] = farPlaneCenter - camX * farWidth + camY * farHeight;
		frustumVertices[5] = farPlaneCenter - camX * farWidth - camY * farHeight;
		frustumVertices[6] = farPlaneCenter + camX * farWidth - camY * farHeight;
		frustumVertices[7] = farPlaneCenter + camX * farWidth + camY * farHeight;
		//重点算出
		DirectX::XMVECTOR temp;
		temp = (frustumVertices[0] + frustumVertices[1] + frustumVertices[2] + frustumVertices[3]) / 4.0f;
		DirectX::XMStoreFloat4(&frustum.center[0], temp);
		temp = (frustumVertices[0] + frustumVertices[3] + frustumVertices[4] + frustumVertices[7]) / 4.0f;
		DirectX::XMStoreFloat4(&frustum.center[1], temp);
		temp = (frustumVertices[0] + frustumVertices[1] + frustumVertices[4] + frustumVertices[5]) / 4.0f;
		DirectX::XMStoreFloat4(&frustum.center[2], temp);
		temp = (frustumVertices[1] + frustumVertices[2] + frustumVertices[5] + frustumVertices[6]) / 4.0f;
		DirectX::XMStoreFloat4(&frustum.center[3], temp);
		temp = (frustumVertices[3] + frustumVertices[2] + frustumVertices[7] + frustumVertices[6]) / 4.0f;
		DirectX::XMStoreFloat4(&frustum.center[4], temp);
		temp = (frustumVertices[4] + frustumVertices[5] + frustumVertices[6] + frustumVertices[7]) / 4.0f;
		DirectX::XMStoreFloat4(&frustum.center[5], temp);
		//法線算出
		temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(frustumVertices[1] - frustumVertices[0], frustumVertices[2] - frustumVertices[0]));
		DirectX::XMStoreFloat4(&frustum.normal[0], temp);
		temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(frustumVertices[3] - frustumVertices[0], frustumVertices[4] - frustumVertices[0]));
		DirectX::XMStoreFloat4(&frustum.normal[1], temp);
		temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(frustumVertices[4] - frustumVertices[0], frustumVertices[1] - frustumVertices[0]));
		DirectX::XMStoreFloat4(&frustum.normal[2], temp);
		temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(frustumVertices[5] - frustumVertices[1], frustumVertices[2] - frustumVertices[1]));
		DirectX::XMStoreFloat4(&frustum.normal[3], temp);
		temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(frustumVertices[2] - frustumVertices[3], frustumVertices[7] - frustumVertices[3]));
		DirectX::XMStoreFloat4(&frustum.normal[4], temp);
		frustum.normal[5].x = -frustum.normal[0].x;
		frustum.normal[5].y = -frustum.normal[0].y;
		frustum.normal[5].z = -frustum.normal[0].z;
		frustum.normal[5].w = -frustum.normal[0].w;
	}
	DirectX::XMMATRIX Camera::CreateProjection(float fov_rad, float aspect, float near_z, float far_z) {
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(fov_rad, aspect, near_z, far_z);
		return projection;
	}
	DirectX::XMMATRIX Camera::CreateView(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& at, const DirectX::XMFLOAT4&up) {
		DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat4(&pos);
		DirectX::XMVECTOR eyeAt = DirectX::XMLoadFloat4(&at);
		DirectX::XMVECTOR eyeUp = DirectX::XMLoadFloat4(&up);
		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eyePos, eyeAt, eyeUp);
		return view;
	}
	DirectX::XMMATRIX Camera::CreateBillboardMat(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& at, const DirectX::XMFLOAT4&up) {
		DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat4(&pos);
		DirectX::XMVECTOR eyeAt = DirectX::XMLoadFloat4(&at);
		DirectX::XMVECTOR eyeUp = DirectX::XMLoadFloat4(&up);
		DirectX::XMVECTOR invAt = DirectX::XMVector4Normalize(eyePos - eyeAt);
		DirectX::XMVECTOR origin{ 0.0f, 0.0f, 0.0f, 0.0f };
		DirectX::XMMATRIX billboard = DirectX::XMMatrixLookAtLH(origin, invAt, eyeUp);
		DirectX::XMVECTOR arg = {};
		return DirectX::XMMatrixInverse(&arg, billboard);

	}
	//----------------------------------------End Viewport------------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Transformer
	//
	//--------------------------------------------------------------------------------------------------
	Transformer::Transformer() {
		world = scalling = rotation = translate = DirectX::XMMatrixIdentity();
	}
	void Transformer::CalcTranslateMatrix() { translate = DirectX::XMMatrixTranslation(position.x, position.y, position.z); }
	void Transformer::CalcScallingMatrix() {
		scalling = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		/*scalling.m[0][0] *= -1;*/
	}
	//移動
	void Transformer::Translation(const Vector3& pos) {
		position = pos;
		CalcTranslateMatrix();
	}
	void Transformer::Translation(float x, float y, float z) {
		position.x = x; position.y = y; position.z = z;
		CalcTranslateMatrix();
	}
	void Transformer::TranslationMove(const Vector3& move) {
		position += move;
		CalcTranslateMatrix();
	}
	void Transformer::TranslationMove(float x, float y, float z) {
		position.x += x; position.y += y; position.z += z;
		CalcTranslateMatrix();
	}
	//回転
	void Transformer::RotationQuaternion(const DirectX::XMVECTOR& quaternion) {
		rotation = DirectX::XMMatrixRotationQuaternion(quaternion);
		//ここでトランスフォームの回転にRPYの回転量算出
	}
	void Transformer::RotationAxis(const Vector3& axis, float rad) {
		rotation = DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ axis.x,axis.y,axis.z,1.0f }, rad);
		//ここでトランスフォームの回転にRPYの回転量算出
	}
	void Transformer::RotationRollPitchYaw(const Vector3& rpy) {
		this->rpy = rpy;
		rotation = DirectX::XMMatrixRotationRollPitchYaw(this->rpy.x, this->rpy.y, this->rpy.z);
	}
	void Transformer::RotationRollPitchYaw(float x, float y, float z) {
		this->rpy.x = y;	this->rpy.y = z; this->rpy.z = x;
		rotation = DirectX::XMMatrixRotationRollPitchYaw(this->rpy.x, this->rpy.y, this->rpy.z);
	}
	void Transformer::RotationYAxis(float rad) {
		this->rpy.x = 0.0f;	this->rpy.y = rad; this->rpy.z = 0.0f;
		rotation = DirectX::XMMatrixRotationY(this->rpy.y);
	}
	//拡縮
	void Transformer::Scalling(const Vector3& scale) {
		this->scale = scale;
		CalcScallingMatrix();
	}
	void Transformer::Scalling(float x, float y, float z) {
		this->scale.x = x; this->scale.y = y; this->scale.z = z;
		CalcScallingMatrix();
	}
	void Transformer::Scalling(float scale) {
		this->scale.x = scale; this->scale.y = scale; this->scale.z = scale;
		CalcScallingMatrix();
	}
	//更新処理
	void Transformer::CalcWorldMatrix() {
		//親子関係あるさいは自分のtransformは親から見たものになるが、ワールドの状態でも欲しいかな？
		world = scalling;
		world *= rotation;
		//ここは少し審議が必要
		world *= translate;
	}
	DirectX::XMMATRIX Transformer::GetTranslateMatrix() { return translate; }
	DirectX::XMMATRIX Transformer::CalcInverseTranslateMatrix() {
		DirectX::XMVECTOR arg = {};
		return DirectX::XMMatrixInverse(&arg, translate);
	}
	DirectX::XMMATRIX Transformer::GetScallingMatrix() { return scalling; }
	DirectX::XMMATRIX Transformer::CalcInverseScallingMatrix() {
		DirectX::XMVECTOR arg = {};
		return DirectX::XMMatrixInverse(&arg, scalling);
	}
	DirectX::XMMATRIX Transformer::GetRotationMatrix() { return rotation; }
	DirectX::XMMATRIX Transformer::CalcInverseRotationMatrix() {
		DirectX::XMVECTOR arg = {};
		return DirectX::XMMatrixInverse(&arg, rotation);
	}
	DirectX::XMMATRIX Transformer::GetWorldMatrixTranspose() { return DirectX::XMMatrixTranspose(world); }
	DirectX::XMMATRIX Transformer::GetWorldMatrix() { return world; }
	DirectX::XMMATRIX Transformer::CalcInverseWorldMatrix() {
		DirectX::XMVECTOR arg = {};
		return  DirectX::XMMatrixInverse(&arg, world);
	}
	const Vector3& Transformer::GetPos() { return position; }
	const Vector3& Transformer::GetScale() { return scale; }
	const Vector3& Transformer::GetRollPitchYaw() { return rpy; }
	//-------------------------------------End Transformer----------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Renderer
	//
	//--------------------------------------------------------------------------------------------------
	PolygonRenderer::PolygonRenderer(int struct_size, int vertex_count_max) :VertexBuffer(struct_size, vertex_count_max), STRUCT_SIZE(struct_size), VERTEX_COUNT_MAX(vertex_count_max) {
	}
	void PolygonRenderer::Render(int render_count, PrimitiveTopology topology) {
		Activate();
		Device::GetContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
		Device::GetContext()->Draw(render_count, 0);
	}
	void PolygonRenderer::RenderIndexed(int render_count, PrimitiveTopology topology) {
		Activate();
		Device::GetContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
		Device::GetContext()->DrawIndexed(render_count, 0, 0);
	}

	//Polygon3DRenderer::Polygon3DRenderer(int struct_size, int vertex_count_max) :VertexBuffer(struct_size, vertex_count_max), STRUCT_SIZE(struct_size), VERTEX_COUNT_MAX(vertex_count_max) {
	//}
	//void Polygon3DRenderer::Render(int render_count, PrimitiveTopology topology) {
	//	Activate();
	//	Device::GetContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
	//	Device::GetContext()->Draw(render_count, 0);
	//}
	//void Polygon3DRenderer::RenderIndexed(int render_count, PrimitiveTopology topology) {
	//	Activate();
	//	Device::GetContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
	//	Device::GetContext()->DrawIndexed(render_count, 0, 0);
	//}
	//std::unique_ptr<VertexBuffer> SpriteRenderer::buffer;
	//void SpriteRenderer::Initialize() {
	//	buffer = std::make_unique<VertexBuffer>(sizeof(Vertex), 4);
	//}
	//void SpriteRenderer::Render(std::shared_ptr<Texture>& texture, int x, int y, int width, int height, float rotation, int uv_begin_x, int uv_begin_y, int uv_end_x, int uv_end_y, int a, int r, int g, int b) {
	//	buffer->Begin();
	//	std::vector<float> vertices;
	//	buffer->End();
	//	buffer->Activate();
	//}
	//----------------------------------------End Renderer-----------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Particle
	//
	//--------------------------------------------------------------------------------------------------
	void GPUParticleSystem::Info::Update(float elapsed_time) {
		elapsedTime = elapsed_time;
	}
	GPUParticleSystem::RWByteAddressBuffer::RWByteAddressBuffer(void* init_buffer, UINT element_size, UINT element_count, bool is_vertex_buffer, bool is_index_buffer, bool is_indirect_args) {
		//バッファ作成
		CreateRWByteAddressBuffer(init_buffer, element_size, element_count, is_vertex_buffer, is_index_buffer, is_indirect_args);
		//UAV作成
		CreateUAV(uav);
	}
	GPUParticleSystem::RWByteAddressBuffer::~RWByteAddressBuffer() = default;
	void GPUParticleSystem::RWByteAddressBuffer::CreateRWByteAddressBuffer(void* init_buffer, UINT element_size, UINT element_count, bool is_vertex_buffer, bool is_index_buffer, bool is_indirect_args) {
		UINT bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		//頂点バッファとしてバインドするか否か
		if (is_vertex_buffer)bindFlags |= D3D11_BIND_VERTEX_BUFFER;
		//インデックスバッファとしてバインドするか否か
		if (is_index_buffer)bindFlags |= D3D11_BIND_INDEX_BUFFER;
		UINT miscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		//Indirect系の引数として使用するか否か
		if (is_indirect_args)miscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		//バッファ作成
		BufferBase::Create(Get().GetAddressOf(), init_buffer, static_cast<UINT>(element_size*element_count), D3D11_USAGE_DEFAULT, bindFlags, 0, element_size, miscFlags);
	}
	void GPUParticleSystem::RWByteAddressBuffer::CreateUAV(ComPtr<ID3D11UnorderedAccessView>& uav) {
		D3D11_BUFFER_DESC bufferDesc;
		//バッファの情報取得
		Get()->GetDesc(&bufferDesc);
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		if (bufferDesc.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) {
			//構造化バッファの場合
			//要素数
			uavDesc.Buffer.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		}
		else if (bufferDesc.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS || bufferDesc.MiscFlags&D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS) {
			//要素数(1要素はfloat又はunsigned intなので4で割る)
			uavDesc.Buffer.NumElements = bufferDesc.ByteWidth / 4UL;
			uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		}
		//上の情報をもとにUAV作成
		Device::Get()->CreateUnorderedAccessView(Get().Get(), &uavDesc, uav.GetAddressOf());
	}
	void GPUParticleSystem::RWByteAddressBuffer::ResourceUpdate(void* data_buffer, UINT element_size, UINT element_count) {
		//まだボックスの部分分かっていないので調査必要
		D3D11_BOX copyRange = { 0,0,0,element_size*element_count,1,1 };
		Device::GetContext()->UpdateSubresource(Get().Get(), 0, &copyRange, data_buffer, element_size, element_count);
	}
	//----------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	//	GPUParticle::Particle
	//----------------------------------------------------------------------------------------------------
	GPUParticleSystem::Particle::Particle(float pos_x, float pos_y, float pos_z, float move_x, float move_y, float move_z, float power_x, float power_y, float power_z, int texture_index, float uv_pos_x, float uv_pos_y, float uv_size_y, float uv_size_x, float alive_time, float fade_in_time, float fade_out_time, float start_scale, float end_scale, float start_rad, float end_rad, float color_r, float color_g, float color_b) :
		posX(pos_x), posY(pos_y), posZ(pos_z), moveX(move_x), moveY(move_y), moveZ(move_z), powerX(power_x), powerY(power_y), powerZ(power_z), textureIndex(texture_index), uvPosX(uv_pos_x), uvPosY(uv_pos_y), uvSizeX(uv_size_x), uvSizeY(uv_size_y),
		aliveTime(alive_time), elapsedTime(alive_time), fadeInTime(fade_in_time), fadeOutTime(fade_out_time), startScale(start_scale), endScale(end_scale), startRad(start_rad), endRad(end_rad), colorRed(color_r), colorGreen(color_g), colorBlue(color_b) {}
	GPUParticleSystem::Particle::Particle() : Particle(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {	}
	GPUParticleSystem::Particle::~Particle() = default;
	//----------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	//	GPUParticle
	//----------------------------------------------------------------------------------------------------
	const std::string GPUParticleSystem::BLEND_LIST[4] = { "COPY" ,"ADD" ,"SUB" ,"SCREEN" };
	//ブレンドプリセット
	GPUParticleSystem::GPUParticleSystem() :textureList{} {
		blendList[i_cast(BlendMode::COPY)] = std::make_shared<BlendState>(BlendPreset::COPY, true, false);
		blendList[i_cast(BlendMode::ADD)] = std::make_shared<BlendState>(BlendPreset::ADD, true, false);
		blendList[i_cast(BlendMode::SUB)] = std::make_shared<BlendState>(BlendPreset::SUB, true, false);
		blendList[i_cast(BlendMode::SCREEN)] = std::make_shared<BlendState>(BlendPreset::SCREEN, true, false);
		material = std::make_unique<Material>("Particle");
		material->ChangeBlendState(blendList[i_cast(BlendMode::ADD)]);
		material->ChangeSamplerState(std::make_shared<SamplerState>(SamplerPreset::POINT, 16));
		material->ChangeRasterizerState(std::make_shared<RasterizerState>(RasterizerPreset::NONE));
		material->ChangeDepthStencilState(std::make_shared<DepthStencilState>(DepthPreset::ALWAYS, false, StencilDesc{}, false));
		material->ChangeVS3DFile("Shader/GPUParticle/GPUParticle.hlsl", "GPUParticleVS", ShaderModel::_5_0, false);
		material->ChangePS3DFile("Shader/GPUParticle/GPUParticle.hlsl", "GPUParticlePS", ShaderModel::_5_0, false);
		//if (!blend)blend = blendList[i_cast(BlendMode::Add)];
		//if (!sampler)sampler = std::make_shared<SamplerState>(Graphics::SamplerPreset::POINT, 16);
		//if (!rasterizer)rasterizer = std::make_shared<RasterizerState>(Graphics::RasterizerPreset::NONE);
		//if (!depthStencil)depthStencil = std::make_shared<DepthStencilState>(Graphics::DepthPreset::ALWAYS, false, StencilDesc{}, false);
		//if (!vs)vs = std::make_shared<VertexShader>("Data/ShaderFile/GPUParticle/GPUParticle.hlsl", "GPUParticleVS", VertexShader::Model::VS_4_0);
		//if (!ps)ps = std::make_shared<PixelShader>("Data/ShaderFile/GPUParticle/GPUParticle.hlsl", "GPUParticlePS", PixelShader::Model::PS_5_0);
		//バッファ作成
		//ConstantBuffer(UINT slot, UINT stuct_size, byte activate_shader_elements);
		infoCBuffer = std::make_unique<ConstantBuffer>(0, sizeof(Info), s_cast<byte>(ShaderStageList::CS));
		appendData = std::make_unique<RWByteAddressBuffer>(appendParticles, s_cast<UINT>(sizeof(Particle)), APPEND_PARTICLE_MAX, false, false, false);
		UINT initIndexBuffer[GPU_PARTICLE_MAX] = {};
		for (int i = 0; i < GPU_PARTICLE_MAX; i++) {
			initIndexBuffer[i] = i;
		}
		indexBuffer = std::make_unique<RWByteAddressBuffer>(initIndexBuffer, s_cast<UINT>(sizeof(UINT)), GPU_PARTICLE_MAX, false, true, false);
		dataBuffer = std::make_unique<RWByteAddressBuffer>(nullptr, s_cast<UINT>(sizeof(Particle)), GPU_PARTICLE_MAX, true, false, false);
		UINT initArgsBuffer[5] = {};
		indirectArgs = std::make_unique<RWByteAddressBuffer>(initArgsBuffer, 4UL, 5UL, false, false, true);
		//ComputeShader作成
		appendCS = std::make_unique<ComputeShader>("Shader/GPUParticle/GPUParticle.hlsl", "AppendParticle");
		sortCS = std::make_unique<ComputeShader>("Shader/GPUParticle/GPUParticle.hlsl", "SortParticle");
		updateCS = std::make_unique<ComputeShader>("Shader/GPUParticle/GPUParticle.hlsl", "UpdateParticle");
		//GeometryShader作成
		gs = std::make_unique<GeometryShader>("Shader/GPUParticle/GPUParticle.hlsl", "GPUParticleGS");
		////リフレクション開始
		//std::unique_ptr<Reflection> reflector = std::make_unique<Reflection>(material->GetVS3D().get());
		////入力レイアウト作成
		//material->ChangeInputLayout3D(std::make_unique<InputLayout>(material->GetVS3D().get(), reflector.get()));
		timer = std::make_unique<Timer>();
		timer->Begin();
	}
	GPUParticleSystem::~GPUParticleSystem() = default;
	void GPUParticleSystem::LoadTexture(int slot, std::string& file_path) {
		if (slot >= TEXTURE_COUNT)Error::Message(ErrorCode::DXE90002);
		textureList[slot] = TextureManager::Load(file_path.c_str(), true);
	}
	float GPUParticleSystem::GetElapsedTime() { return timer->GetSecondResult(); }
	void GPUParticleSystem::Append(const Particle& particle) {
		if (info.appendCount >= APPEND_PARTICLE_MAX) Error::Message(ErrorCode::DXE90002);
		appendParticles[info.appendCount++] = particle;
	}
	void GPUParticleSystem::RunAppend() {
		//GPUのバッファにデータを移す
		appendData->ResourceUpdate(&appendParticles, sizeof(Particle), info.appendCount);
		//uavの設定
		ID3D11UnorderedAccessView* uavs[4] = { appendData->uav.Get(),indexBuffer->uav.Get(),dataBuffer->uav.Get() ,indirectArgs->uav.Get() };
		ComputeShader::SetUnorderedAccessView(0, 4, uavs);
		//GPUへのパーティクルの追加を実行
		appendCS->Run(1, 1, 1);
		info.appendCount = 0;
		ID3D11UnorderedAccessView* resetUavs[4] = {};
		ComputeShader::SetUnorderedAccessView(0, 4, resetUavs);
	}
	void GPUParticleSystem::RunSort() {
		//uavの設定
		ID3D11UnorderedAccessView* uavs[4] = { appendData->uav.Get(),indexBuffer->uav.Get(),dataBuffer->uav.Get() ,indirectArgs->uav.Get() };
		ComputeShader::SetUnorderedAccessView(0, 4, uavs);
		//分割要素数がパーティクル総数超えるまで回す
		for (UINT divideLevel = 2; divideLevel <= GPU_PARTICLE_MAX; divideLevel <<= 1) {
			for (UINT subDivision = divideLevel; 1 < subDivision; subDivision >>= 1) {
				info.compareInterval = subDivision >> 1;
				info.divideLevel = divideLevel;
				info.isBitonicFinal = (divideLevel == GPU_PARTICLE_MAX && subDivision == 2);
				//定数バッファの更新
				infoCBuffer->Activate(&info);
				//ソート実行
				sortCS->Run(GPU_PARTICLE_MAX / LOCAL_THREAD_COUNT, 1, 1);
			}
		}
		ID3D11UnorderedAccessView* resetUavs[4] = {};
		ComputeShader::SetUnorderedAccessView(0, 4, resetUavs);
	}
	void GPUParticleSystem::RunUpdate() {
		ID3D11UnorderedAccessView* uavs[4] = { appendData->uav.Get(),indexBuffer->uav.Get(),dataBuffer->uav.Get() ,indirectArgs->uav.Get() };
		ComputeShader::SetUnorderedAccessView(0, 4, uavs);
		//パーティクル更新処理
		updateCS->Run(GPU_PARTICLE_MAX / (LOCAL_THREAD_COUNT*THREAD_PER_COUNT), 1, 1);
		ID3D11UnorderedAccessView* resetUavs[4] = {};
		ComputeShader::SetUnorderedAccessView(0, 4, resetUavs);
	}
	void GPUParticleSystem::Update(float time_scale) {
		timer->End();
		//経過時間更新
		info.elapsedTime = timer->GetSecondResult()*time_scale;
		//info.elapsedTime = Application::GetInstance()->GetProcessTimeMili()*time_scale / 1000.0f;
		//定数バッファ更新
		infoCBuffer->Activate(&info);
		//パーティクル追加されていない場合はスルー GPUへのパーティクル追加実行
		if (info.appendCount > 0)RunAppend();
		//ソート実行
		RunSort();
		RunUpdate();
		timer->Begin();
	}
	void GPUParticleSystem::Render(BlendMode mode) {
		for (int i = 0; i < TEXTURE_COUNT; i++) {
			if (textureList[i])textureList[i]->Set(i + 10, ShaderStageList::PS);
		}
		UINT stride = sizeof(Particle);
		UINT offset = 0;
		//重要
		Device::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		Device::GetContext()->IASetVertexBuffers(0, 1, dataBuffer->Get().GetAddressOf(), &stride, &offset);
		Device::GetContext()->IASetIndexBuffer(indexBuffer->Get().Get(), DXGI_FORMAT_R32_UINT, offset);
		material->ChangeBlendState(blendList[i_cast(mode)]);
		material->ActivateState();
		material->ActivateShader3D();
		gs->Set();
		Device::GetContext()->DrawIndexedInstancedIndirect(indirectArgs->Get().Get(), 0);
		Device::GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, offset);
		ID3D11Buffer* resetBuffer = nullptr;
		Device::GetContext()->IASetVertexBuffers(0, 1, &resetBuffer, &stride, &offset);
		//GeometryShaderのクリア
		Device::GetContext()->GSSetShader(nullptr, nullptr, 0);
	}
	//----------------------------------------End Particle-----------------------------------------

	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Input
	//
	//--------------------------------------------------------------------------------------------------
	ComPtr<IDirectInput8> DirectInput::device;
	void DirectInput::Initialize() {
		HRESULT hr = S_OK;
		hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(device.GetAddressOf()), nullptr);
		if (FAILED(hr))Error::Message(ErrorCode::DXE00400);
	}
	ComPtr<IDirectInput8>& DirectInput::GetDevice() { return device; }

	void InputDevice::Initialize(HWND hwnd, const GUID& guid, const DIDATAFORMAT& format, bool fore_ground, bool exclusive) {
		this->hwnd = hwnd;
		decltype(auto) device = DirectInput::GetDevice();
		HRESULT hr = S_OK;
		hr = device->CreateDevice(guid, inputDevice.GetAddressOf(), nullptr);
		if (FAILED(hr))Error::Message(ErrorCode::DXE00401);
		hr = inputDevice->SetDataFormat(&format);
		if (FAILED(hr))Error::Message(ErrorCode::DXE00402);
		//協調レベルの設定
		DWORD cooperativeLevel = 0;
		if (fore_ground) cooperativeLevel |= DISCL_FOREGROUND;
		else cooperativeLevel |= DISCL_BACKGROUND;
		if (exclusive) cooperativeLevel |= DISCL_EXCLUSIVE;
		else cooperativeLevel |= DISCL_NONEXCLUSIVE;
		hr = inputDevice->SetCooperativeLevel(hwnd, cooperativeLevel);
		if (FAILED(hr)) Error::Message(ErrorCode::DXE00403);
	}
	bool InputDevice::Acquire() {
		HRESULT hr = inputDevice->Acquire();
		if (FAILED(hr))return false;
		return true;
	}
	ComPtr<IDirectInputDevice8>& InputDevice::Get() { return inputDevice; }
	HWND InputDevice::GetParentHundle() { return hwnd; }

	std::unique_ptr<InputDevice> Mouse::device;
	BYTE Mouse::buffer[3];
	float Mouse::move[2];
	int Mouse::wheel;
	float Mouse::clientPos[2];
	float Mouse::screenPos[2];
	int Mouse::pushKeyNo;

	void Mouse::Initialize(void* hwnd, bool fore_ground, bool exclusive) {
		device = std::make_unique<InputDevice>();
		device->Initialize(static_cast<HWND>(hwnd), GUID_SysMouse, c_dfDIMouse2, fore_ground, exclusive);
		//軸モードを相対値モードに設定
		DIPROPDWORD property = {};
		property.diph.dwSize = sizeof(DIPROPDWORD);
		property.diph.dwHeaderSize = sizeof(property.diph);
		property.diph.dwObj = 0;
		property.diph.dwHow = DIPH_DEVICE;
		property.dwData = DIPROPAXISMODE_REL;
		//絶対値モードの場合
		//property.dwData = DIPROPAXISMODE_ABS;
		HRESULT hr = device->Get()->SetProperty(DIPROP_AXISMODE, &property.diph);
		if (FAILED(hr))Error::Message(ErrorCode::DXE00404);
	}
	void Mouse::Update() {
		DIMOUSESTATE2 state = {};
		if (device->Acquire()) {
			HRESULT hr = device->Get()->GetDeviceState(sizeof(DIMOUSESTATE2), &state);
			if (FAILED(hr))Error::Message(ErrorCode::DXE00405);
		}
		pushKeyNo = -1;
		for (int i = 0; i < 3; i++) {
			buffer[i] <<= 1;
			buffer[i] |= (state.rgbButtons[i] != 0);
			if (pushKeyNo == -1 && buffer[i])pushKeyNo = i;
		}
		move[0] = f_cast(state.lX);
		move[1] = f_cast(state.lY);
		wheel = state.lZ;
		//マウス座標取得
		POINT mpos = {};
		GetCursorPos(&mpos);
		screenPos[0] = f_cast(mpos.x);
		screenPos[1] = f_cast(mpos.y);
		ScreenToClient(device->GetParentHundle(), &mpos);
		clientPos[0] = f_cast(mpos.x);
		clientPos[1] = f_cast(mpos.y);
	}
	BYTE Mouse::GetKey(int key_code) {
		if (static_cast<UINT>(key_code) > 3)Error::Message(ErrorCode::DXE00406);
		return buffer[key_code] & 3;
	}
	bool Mouse::IsPushAnyKey() { return (pushKeyNo != -1); }
	int Mouse::PushKeyNo() { return pushKeyNo; }
	float Mouse::GetMoveX() { return move[0]; }
	float Mouse::GetMoveY() { return move[1]; }
	int Mouse::GetWheel() { return wheel; }
	float Mouse::GetClientPosX() { return clientPos[0]; }
	float Mouse::GetClientPosY() { return clientPos[1]; }
	float Mouse::GetScreenPosX() { return screenPos[0]; }
	float Mouse::GetScreenPosY() { return screenPos[1]; }
	//----------------------------------------End Input-----------------------------------------
	//--------------------------------------------------------------------------------------------------
	//
	//		Begin Matrix Helper
	//
	//--------------------------------------------------------------------------------------------------
	//多少遅くなるけどめんどいので
	DirectX::XMFLOAT4X4 Store(const DirectX::XMMATRIX& matrix) {
		DirectX::XMFLOAT4X4 ret = {};
		DirectX::XMStoreFloat4x4(&ret, matrix);
		return ret;
	}
	std::vector<float> ConverteVector(float x, float y, float z) {
		std::vector<float> ret(3);
		ret[0] = x; ret[1] = y; ret[2] = z;
		return ret;
	}
	std::vector<float> GetAxisXDirection(const DirectX::XMMATRIX& matrix) {
		auto ret = Store(matrix);
		return ConverteVector(ret._11, ret._12, ret._13);
	}
	std::vector<float> GetAxisYDirection(const DirectX::XMMATRIX& matrix) {
		auto ret = Store(matrix);
		return ConverteVector(ret._21, ret._22, ret._23);
	}
	std::vector<float> GetAxisZDirection(const DirectX::XMMATRIX& matrix) {
		auto ret = Store(matrix);
		return ConverteVector(ret._31, ret._32, ret._33);
	}
	std::vector<float> GetPosition(const DirectX::XMMATRIX& matrix) {
		auto ret = Store(matrix);
		return ConverteVector(ret._41, ret._42, ret._43);
	}
	//-------------------------------------End Matrix Helper--------------------------------------

}

