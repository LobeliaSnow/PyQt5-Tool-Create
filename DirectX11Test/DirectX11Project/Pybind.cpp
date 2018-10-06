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

//x64 Release
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
	py::enum_<GPUParticleSystem::BlendMode>(m, "ParticleBlendMode")
		.value("COPY", GPUParticleSystem::BlendMode::COPY)
		.value("ADD", GPUParticleSystem::BlendMode::ADD)
		.value("SUB", GPUParticleSystem::BlendMode::SUB)
		.value("SCREEN", GPUParticleSystem::BlendMode::SCREEN);
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
		.def("Set", &VertexBuffer::Set<float>)
		.def("Set", &VertexBuffer::Set<int>)
		.def("End", &BufferBase::End);

	//継承関係
	py::class_<VertexBuffer, BufferBase>(m, "VertexBuffer")
		.def(py::init<int, int>())
		.def("GetMaxCount", &VertexBuffer::GetMaxCount);
	py::class_<IndexBuffer, BufferBase>(m, "IndexBuffer")
		.def(py::init<int>())
		.def("Activate", &IndexBuffer::Activate);
	py::class_<ConstantBuffer, BufferBase>(m, "ConstantBuffer")
		.def(py::init<UINT, UINT, byte>())
		.def("Activate", &ConstantBuffer::Activate);

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
	//py::class_<ComputeShader, Shader>(m, "ComputeShader")
	//	.def(py::init<const char*, const char*>())
	//	.def("Run", &ComputeShader::Run)
	//	.def_static("SetShaderResourceView", static_cast<void(*)(int, ID3D11ShaderResourceView*)>(&ComputeShader::SetShaderResourceView))
	//	.def_static("SetShaderResourceView", static_cast<void(*)(int, int, ID3D11ShaderResourceView**)>(&ComputeShader::SetShaderResourceView))
	//	.def_static("SetUnorderedAccessView", static_cast<void(*)(int, ID3D11UnorderedAccessView*)>(&ComputeShader::SetUnorderedAccessView))
	//	.def_static("SetUnorderedAccessView", static_cast<void(*)(int, int, ID3D11UnorderedAccessView**)>(&ComputeShader::SetUnorderedAccessView));
	py::class_<VertexShader, Shader>(m, "VertexShader")
		.def(py::init<const char*, const char*, ShaderModel, bool>())
		.def(py::init<const char*, int, const char*, ShaderModel, bool>())
		.def("Set", &VertexShader::Set);
	py::class_<PixelShader, Shader>(m, "PixelShader")
		.def(py::init<const char*, const char*, ShaderModel, bool>())
		.def(py::init<const char*, int, const char*, ShaderModel, bool>())
		.def("Set", &PixelShader::Set);
	py::class_<GeometryShader, Shader>(m, "GeometryShader")
		.def(py::init<const char*, const char*>())
		.def("Set", &GeometryShader::Set)
		.def_static("Clean", &GeometryShader::Clean);

	py::class_<InputLayout>(m, "InputLayout")
		.def(py::init<VertexShader*, Reflection*>())
		.def("Set", &InputLayout::Set);

	//とりあえずステートめんどいからマテリアルバインドでテスト
	py::class_<Material>(m, "Material")
		.def(py::init<const char*>())
		.def("ActivateState", &Material::ActivateState)
		.def("ActivateShader2D", &Material::ActivateShader2D)
		.def("ActivateShader3D", &Material::ActivateShader3D)
		.def("ChangeVS2DMemory", &Material::ChangeVS2DMemory)
		.def("ChangeVS2DFile", &Material::ChangeVS2DFile)
		.def("ChangePS2DMemory", &Material::ChangePS2DMemory)
		.def("ChangePS2DFile", &Material::ChangePS2DFile)
		.def("ChangeVS3DMemory", &Material::ChangeVS3DMemory)
		.def("ChangeVS3DFile", &Material::ChangeVS3DFile)
		.def("ChangePS3DMemory", &Material::ChangePS3DMemory)
		.def("ChangePS3DFile", &Material::ChangePS3DFile)
		.def("ChangeDiffuseTexture", &Material::ChangeDiffuseTexture);

	py::class_<Viewport>(m, "Viewport")
		.def(py::init<int, int, int, int>())
		.def("Activate", &Viewport::Activate);
	py::class_<Camera>(m, "Camera")
		.def(py::init<int, int, float, float, float>())
		.def("SetPos", &Camera::SetPos)
		.def("SetAt", &Camera::SetAt)
		.def("SetUp", &Camera::SetUp)
		.def("Activate", &Camera::Activate);

	py::class_<Transformer>(m, "Transformer")
		.def(py::init())
		.def("Translation", static_cast<void(Transformer::*)(const Vector3&)>(&Transformer::Translation))
		.def("Translation", static_cast<void(Transformer::*)(float, float, float)>(&Transformer::Translation))
		.def("TranslationMove", static_cast<void(Transformer::*)(const Vector3&)>(&Transformer::TranslationMove))
		.def("TranslationMove", static_cast<void(Transformer::*)(float, float, float)>(&Transformer::TranslationMove))
		.def("RotationQuaternion", &Transformer::RotationQuaternion)
		.def("RotationAxis", &Transformer::RotationAxis)
		.def("RotationRollPitchYaw", static_cast<void(Transformer::*)(const Vector3&)>(&Transformer::RotationRollPitchYaw))
		.def("RotationRollPitchYaw", static_cast<void(Transformer::*)(float, float, float)>(&Transformer::RotationRollPitchYaw))
		.def("RotationYAxis", &Transformer::RotationYAxis)
		.def("Scalling", static_cast<void(Transformer::*)(const Vector3&)>(&Transformer::Scalling))
		.def("Scalling", static_cast<void(Transformer::*)(float, float, float)>(&Transformer::Scalling))
		.def("Scalling", static_cast<void(Transformer::*)(float)>(&Transformer::Scalling))
		.def("CalcWorldMatrix", &Transformer::CalcWorldMatrix)
		.def("GetTranslateMatrix", &Transformer::GetTranslateMatrix)
		.def("CalcInverseTranslateMatrix", &Transformer::CalcInverseTranslateMatrix)
		.def("GetScallingMatrix", &Transformer::GetScallingMatrix)
		.def("CalcInverseScallingMatrix", &Transformer::CalcInverseScallingMatrix)
		.def("GetRotationMatrix", &Transformer::GetRotationMatrix)
		.def("CalcInverseRotationMatrix", &Transformer::CalcInverseRotationMatrix)
		.def("GetWorldMatrix", &Transformer::GetWorldMatrix)
		.def("GetWorldMatrixTranspose", &Transformer::GetWorldMatrixTranspose)
		.def("CalcInverseWorldMatrix", &Transformer::CalcInverseWorldMatrix)
		.def("GetPos", &Transformer::GetPos)
		.def("GetScale", &Transformer::GetScale)
		.def("GetRollPitchYaw", &Transformer::GetRollPitchYaw);

	py::class_<PolygonRenderer, VertexBuffer>(m, "PolygonRenderer")
		.def(py::init<int, int>())
		.def("Render", &PolygonRenderer::Render)
		.def("RenderIndexed", &PolygonRenderer::RenderIndexed);
	//py::class_<Polygon3DRenderer, VertexBuffer>(m, "Polygon3DRenderer")
	//	.def(py::init<int, int>())
	//	.def("Render", &Polygon3DRenderer::Render)
	py::class_<GPUParticleSystem::Particle>(m, "GPUParticle")
		.def(py::init<float, float, float, float, float, float, float, float, float, int, float, float, float, float, float, float, float, float, float, float, float, float, float, float>())
		.def(py::init())
		.def_readwrite("pos", &GPUParticleSystem::Particle::pos)
		.def_readwrite("move", &GPUParticleSystem::Particle::move)
		.def_readwrite("power", &GPUParticleSystem::Particle::power)
		.def_readwrite("textureIndex", &GPUParticleSystem::Particle::textureIndex)
		.def_readwrite("uvPos", &GPUParticleSystem::Particle::uvPos)
		.def_readwrite("uvSize", &GPUParticleSystem::Particle::uvSize)
		.def_readwrite("aliveTime", &GPUParticleSystem::Particle::aliveTime)
		.def_readwrite("elapsedTime", &GPUParticleSystem::Particle::elapsedTime)
		.def_readwrite("fadeInTime", &GPUParticleSystem::Particle::fadeInTime)
		.def_readwrite("fadeOutTime", &GPUParticleSystem::Particle::aliveTime)
		.def_readwrite("startScale", &GPUParticleSystem::Particle::startScale)
		.def_readwrite("endScale", &GPUParticleSystem::Particle::endScale)
		.def_readwrite("startRad", &GPUParticleSystem::Particle::startRad)
		.def_readwrite("endRad", &GPUParticleSystem::Particle::endRad)
		.def_readwrite("color", &GPUParticleSystem::Particle::color);
	py::class_<GPUParticleSystem>(m, "GPUParticleSystem")
		.def(py::init())
		.def("LoadTexture", &GPUParticleSystem::LoadTexture)
		.def("Append", &GPUParticleSystem::Append)
		.def("Update", &GPUParticleSystem::Update)
		.def("Render", &GPUParticleSystem::Render);

	py::class_<Error>(m, "Error")
		.def_static("ErrorBox", &Error::ErrorBox);
	//シェーダーのヘッダー情報
	m.attr("constantBufferInfo") = py::cast(constantBufferInfo);

	py::class_<DirectInput>(m, "DirectInput")
		.def_static("Initialize", &DirectInput::Initialize);
	py::class_<InputDevice>(m, "InputDevice")
		.def("Initialize", &InputDevice::Initialize)
		.def("Acquire", &InputDevice::Acquire)
		.def("GetParentHundle", &InputDevice::GetParentHundle);
	py::class_<Mouse>(m, "Mouse")
		.def_static("Initialize", &Mouse::Initialize)
		.def_static("Update", &Mouse::Update)
		.def_static("GetKey", &Mouse::GetKey)
		.def_static("IsPushAnyKey", &Mouse::IsPushAnyKey)
		.def_static("PushKeyNo", &Mouse::PushKeyNo)
		.def_static("GetMoveX", &Mouse::GetMoveX)
		.def_static("GetMoveY", &Mouse::GetMoveY)
		.def_static("GetWheel", &Mouse::GetWheel)
		.def_static("GetClientPosX", &Mouse::GetClientPosX)
		.def_static("GetClientPosY", &Mouse::GetClientPosY)
		.def_static("GetScreenPosX", &Mouse::GetScreenPosX)
		.def_static("GetScreenPosY", &Mouse::GetScreenPosY);

	py::class_<DirectX::XMMATRIX>(m, "Matrix");
	m.def("GetAxisXDirection", &GetAxisXDirection);
	m.def("GetAxisYDirection", &GetAxisYDirection);
	m.def("GetAxisZDirection", &GetAxisZDirection);
	m.def("GetPosition", &GetPosition);
}
#else
//テスト用コード x86 Debug
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
	StaticField::renderer->Render(3, PrimitiveTopology::LINE_STRIP);
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