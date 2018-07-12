#pragma once
namespace Lobelia {
	enum class ACCESS_FLAG {
		DEFAULT = D3D11_USAGE_DEFAULT,
		DYNAMIC = D3D11_USAGE_DYNAMIC,
		IMMUTABLE = D3D11_USAGE_IMMUTABLE,
		STAGING = D3D11_USAGE_STAGING
	};
}