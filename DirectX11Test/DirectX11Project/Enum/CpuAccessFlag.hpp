#pragma once
namespace Lobelia {
	enum class CPU_ACCESS_FLAG {
		NONE = 0,
		READ = D3D11_CPU_ACCESS_READ,
		WRITE = D3D11_CPU_ACCESS_WRITE,
	};

}