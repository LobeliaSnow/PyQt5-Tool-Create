#undef min
#undef max
#include <pybind11/pybind11.h>
//STLを自動でpythonのlistやmapに変換
#include <pybind11/stl.h>
#include <pycapsule.h>
#include "Device/DirectXWrapper.hpp"
#include "Error/ErrorCode.hpp"

//この先PyBind
using namespace Lobelia;
namespace py = pybind11;

#ifndef _DEBUG 
#define OUTPUT_PYTHON
#endif
#ifdef OUTPUT_PYTHON
PYBIND11_MODULE(DirectX11, m) {
	m.doc() = "DirectX11WrpperLibrary";
	//enum
	//必要になった時にのみ追加していくスタンスで
	py::enum_<TextureFormat>(m, "TextureBufferFormat")
		.value("R32G32B32A32_FLOAT", TextureFormat::R32G32B32A32_FLOAT)
		.export_values();

	py::enum_<ACCESS_FLAG>(m, "ACCESS_FLAG")
		.value("DEFAULT", ACCESS_FLAG::DEFAULT)
		.value("DYNAMIC", ACCESS_FLAG::DYNAMIC)
		.value("IMMUTABLE", ACCESS_FLAG::IMMUTABLE)
		.value("STAGING", ACCESS_FLAG::STAGING)
		.export_values();

	py::enum_<CPU_ACCESS_FLAG>(m, "TextureFormat")
		.value("NONE", CPU_ACCESS_FLAG::NONE)
		.value("READ", CPU_ACCESS_FLAG::READ)
		.value("WRITE", CPU_ACCESS_FLAG::WRITE)
		.export_values();

	py::enum_<ShaderStageList>(m, "ShaderStageList")
		.value("VS", ShaderStageList::VS)
		.value("PS", ShaderStageList::PS)
		.value("HS", ShaderStageList::HS)
		.value("GS", ShaderStageList::GS)
		.value("DS", ShaderStageList::DS)
		.value("CS", ShaderStageList::CS)
		.export_values();

	py::enum_<ShaderModel>(m, "ShaderModel")
		.value("_4_0", ShaderModel::_4_0)
		.value("_5_0", ShaderModel::_5_0)
		.value("_5_1", ShaderModel::_5_1)
		.export_values();

	py::enum_<BlendPreset>(m, "BlendPreset")
		.value("MIN", BlendPreset::MIN)
		.value("NONE", BlendPreset::NONE)
		.value("COPY", BlendPreset::COPY)
		.value("ADD", BlendPreset::ADD)
		.value("SUB", BlendPreset::SUB)
		.value("REPLACE", BlendPreset::REPLACE)
		.value("MULTIPLY", BlendPreset::MULTIPLY)
		.value("LIGHTENESS", BlendPreset::LIGHTENESS)
		.value("DARKENESS", BlendPreset::DARKENESS)
		.value("SCREEN", BlendPreset::SCREEN)
		.value("MAX", BlendPreset::MAX)
		.export_values();
	py::enum_<SamplerPreset>(m, "SamplerPreset")
		.value("MIN", SamplerPreset::MIN)
		.value("POINT", SamplerPreset::POINT)
		.value("LINEAR", SamplerPreset::LINEAR)
		.value("ANISOTROPIC", SamplerPreset::ANISOTROPIC)
		.value("COMPARISON_POINT", SamplerPreset::COMPARISON_POINT)
		.value("COMPARISON_LINEAR", SamplerPreset::COMPARISON_LINEAR)
		.value("COMPARISON_ANISOTROPIC", SamplerPreset::COMPARISON_ANISOTROPIC)
		.value("MAX", SamplerPreset::MAX)
		.export_values();
	py::enum_<RasterizerPreset>(m, "RasterizerPreset")
		.value("MIN", RasterizerPreset::MIN)
		.value("FRONT", RasterizerPreset::FRONT)
		.value("BACK", RasterizerPreset::BACK)
		.value("NONE", RasterizerPreset::NONE)
		.value("MAX", RasterizerPreset::MAX)
		.export_values();
	py::enum_<DepthPreset>(m, "DepthPreset")
		.value("MIN", DepthPreset::MIN)
		.value("NEVER", DepthPreset::NEVER)
		.value("LESS", DepthPreset::LESS)
		.value("EQUAL", DepthPreset::EQUAL)
		.value("LESS_EQUAL", DepthPreset::LESS_EQUAL)
		.value("GREATER", DepthPreset::GREATER)
		.value("NOT_EQUAL", DepthPreset::NOT_EQUAL)
		.value("GREATER_EQUAL", DepthPreset::GREATER_EQUAL)
		.value("ALWAYS", DepthPreset::ALWAYS)
		.value("MAX", DepthPreset::MAX)
		.export_values();

	py::enum_<PrimitiveTopology>(m, "PrimitiveTopology")
		.value("POINT_LIST", PrimitiveTopology::POINT_LIST)
		.value("LINE_STRIP", PrimitiveTopology::LINE_STRIP)
		.value("LINE_LIST", PrimitiveTopology::LINE_LIST)
		.value("TRIANGLE_STRIP", PrimitiveTopology::TRIANGLE_STRIP)
		.value("TRIANGLE_LIST", PrimitiveTopology::TRIANGLE_LIST)
		.export_values();

	py::enum_<StencilDesc::Face::Operation>(m, "SFaceOperation")
		.value("KEEP", StencilDesc::Face::Operation::KEEP)
		.value("ZERO", StencilDesc::Face::Operation::ZERO)
		.value("REPLACE", StencilDesc::Face::Operation::REPLACE)
		.value("INCR_CLAMP", StencilDesc::Face::Operation::INCR_CLAMP)
		.value("DECR_CLAMP", StencilDesc::Face::Operation::DECR_CLAMP)
		.value("INVERT", StencilDesc::Face::Operation::INVERT)
		.value("INCR_WRAP", StencilDesc::Face::Operation::INCR_WRAP)
		.value("DECR_WRAP", StencilDesc::Face::Operation::DECR_WRAP)
		.export_values();
	py::enum_<StencilDesc::Face::Function>(m, "SFaceFunction")
		.value("ALWAYS_FAIL", StencilDesc::Face::Function::ALWAYS_FAIL)
		.value("LESS", StencilDesc::Face::Function::LESS)
		.value("EQUAL", StencilDesc::Face::Function::EQUAL)
		.value("LESS_EQUAL", StencilDesc::Face::Function::LESS_EQUAL)
		.value("GREATER", StencilDesc::Face::Function::GREATER)
		.value("NOT_EQUAL", StencilDesc::Face::Function::NOT_EQUAL)
		.value("GREATER_EQUAL", StencilDesc::Face::Function::GREATER_EQUAL)
		.value("ALWAYS_PASS", StencilDesc::Face::Function::ALWAYS_PASS)
		.export_values();

	//class
	//デバイスクラスを登録
	py::class_<Device>(m, "Device")
		.def_static("Create", &Device::Create)
		.def_static("Destroy", &Device::Destroy);

	py::class_<RTSampleDesc>(m, "RTSampleDesc")
		.def(py::init())
		.def_readwrite("count", &RTSampleDesc::count)
		.def_readwrite("quality", &RTSampleDesc::quality);

	py::class_<Texture>(m, "Texture")
		.def(py::init<int, int, TextureFormat, UINT, RTSampleDesc, ACCESS_FLAG, CPU_ACCESS_FLAG, int>())
		.def("Set", &Texture::Set)
		.def("GetWidth", &Texture::GetWidth)
		.def("GetHeight", &Texture::GetHeight)
		.def_static("Clean", &Texture::Clean);

	py::class_<TextureManager>(m, "TextureManager")
		.def_static("Load", &TextureManager::Load)
		.def_static("Save", &TextureManager::Save)
		.def_static("CreateNormalMap", &TextureManager::CreateNormalMap);

	py::class_<RenderTarget>(m, "RenderTarget")
		.def(py::init<int, int, TextureFormat, RTSampleDesc, int>())
		.def("Clear", &RenderTarget::Clear)
		.def("Activate", &RenderTarget::Activate);

	py::class_<SwapChain>(m, "SwapChain")
		.def(py::init<void*, int, int, RTSampleDesc>())
		.def("GetRenderTarget", &SwapChain::GetRenderTarget)
		.def("Clear", &SwapChain::Clear)
		.def("Present", &SwapChain::Present);

	py::class_<BufferBase>(m, "BufferBase")
		.def(py::init())
		.def("Begin", &BufferBase::Begin)
		.def("Set", &VertexBuffer::Set)
		.def("End", &BufferBase::End);

	//継承関係
	py::class_<VertexBuffer, BufferBase>(m, "VertexBuffer")
		.def(py::init<int, int>())
		.def("GetMaxCount", &VertexBuffer::GetMaxCount);

	//バインドできない、直さないと。。。
	py::class_<BlendState>(m, "BlendState")
		.def(py::init<BlendPreset, bool, bool>())
		.def("Set", &BlendState::Set);
	py::class_<SamplerState>(m, "SamplerState")
		.def(py::init<SamplerPreset, int, bool>())
		.def("Set", &SamplerState::Set);
	py::class_<RasterizerState>(m, "RasterizerState")
		.def(py::init<RasterizerPreset, bool>())
		.def("Set", &RasterizerState::Set);

	py::class_<StencilDesc::Face>(m, "Face")
		.def(py::init())
		.def_readwrite("faile", &StencilDesc::Face::faile)
		.def_readwrite("depthFail", &StencilDesc::Face::depthFail)
		.def_readwrite("pass", &StencilDesc::Face::pass)
		.def_readwrite("testFunc", &StencilDesc::Face::testFunc);

	py::class_<StencilDesc>(m, "StencilDesc")
		.def(py::init())
		.def_readwrite("readMask", &StencilDesc::readMask)
		.def_readwrite("writeMask", &StencilDesc::writeMask)
		.def_readwrite("front", &StencilDesc::front)
		.def_readwrite("back", &StencilDesc::back);

	py::class_<DepthStencilState>(m, "DepthStencilState")
		.def(py::init<DepthPreset, bool, StencilDesc, bool>())
		.def("Set", &DepthStencilState::Set);

	py::class_<ShaderLinkageInstance>(m, "ShaderLinkageInstance")
		.def(py::init())
		.def("GetName", &ShaderLinkageInstance::GetName);
	py::class_<ShaderLinkage>(m, "ShaderLinkage")
		.def(py::init())
		.def("CreateInstance", &ShaderLinkage::CreateInstance)
		.def("GetInstance", &ShaderLinkage::GetInstance);
	py::class_<Shader>(m, "Shader")
		.def(py::init<const char*, const char*, const char*, ShaderLinkage*>())
		.def("GetLinkage", &Shader::GetLinkage)
		.def("SetLinkage", &Shader::SetLinkage);
	py::class_<VertexShader, Shader>(m, "VertexShader")
		.def(py::init<const char*, const char*, ShaderModel, bool>())
		.def("Set", &VertexShader::Set);
	py::class_<PixelShader, Shader>(m, "PixelShader")
		.def(py::init<const char*, const char*, ShaderModel, bool>())
		.def("Set", &PixelShader::Set);

	py::class_<InputLayout>(m, "InputLayout")
		.def(py::init<VertexShader*, Reflection*>())
		.def("Set", &InputLayout::Set);

	//とりあえずステートめんどいからマテリアルバインドでテスト
	py::class_<Material>(m, "Material")
		.def(py::init<const char*>())
		.def("Activate", &Material::Activate);

	py::class_<Viewport>(m, "Viewport")
		.def(py::init<int, int, int, int>())
		.def("Activate", &Viewport::Activate);

	py::class_<Polygon2DRenderer, VertexBuffer>(m, "Polygon2DRenderer")
		.def(py::init<int, int>())
		.def("Render", &Polygon2DRenderer::Render);

	py::class_<Error>(m, "Error")
		.def_static("ErrorBox", &Error::ErrorBox);
}
#else
//テスト用コード
#include "CPPDLLHeader.hpp"
std::unique_ptr<SwapChain> StaticField::swapChain;
std::unique_ptr<Viewport> StaticField::viewport;
std::unique_ptr<Material> StaticField::material;
std::unique_ptr<Polygon2DRenderer> StaticField::renderer;
#define DLLEXPORT __declspec(dllexport)
DLLEXPORT void Initialize(HWND hwnd, int x, int y) {
	Device::Create(D3D11_CREATE_DEVICE_DEBUG);
	RTSampleDesc sample = { 1,0 };
	StaticField::swapChain = std::make_unique<SwapChain>(hwnd, x, y, sample);
	StaticField::viewport = std::make_unique<Viewport>(0, 0, x, y);
	StaticField::material = std::make_unique<Material>("Simple");
	StaticField::renderer = std::make_unique<Polygon2DRenderer>(8, 3);
	StaticField::renderer->Begin();
	std::vector<float> triangle;
	triangle.resize(6);
	triangle[0] = 0.5f;
	triangle[1] = 0.0f;
	triangle[2] = 0.5f;
	triangle[3] = 0.5f;
	triangle[4] = 0.0f;
	triangle[5] = 0.0f;
	StaticField::renderer->Set(0, triangle);
	StaticField::renderer->End();
}
DLLEXPORT void Update() {
	StaticField::swapChain->Clear(0.0f, 0.0f, 0.0f, 0.0f);
	StaticField::viewport->Activate();
	StaticField::material->Activate();
	StaticField::renderer->Render(3, PrimitiveTopology::POINT_LIST);
	StaticField::swapChain->Present(1);
}
DLLEXPORT void Finalize() {
	StaticField::swapChain.reset();
	StaticField::viewport.reset();
	StaticField::material.reset();
	StaticField::renderer.reset();
	Device::Destroy();
}
#endif