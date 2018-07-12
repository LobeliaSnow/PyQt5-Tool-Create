#pragma once
namespace Lobelia {
	enum class PrimitiveTopology {
		POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		LINE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	};
}