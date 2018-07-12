#pragma once
#include "Device/DirectXWrapper.hpp"

namespace Lobelia {
	class StaticField {
	public:
		static std::unique_ptr<SwapChain> swapChain;
		static std::unique_ptr<Viewport> viewport;
		static std::unique_ptr<Material> material;
		static std::unique_ptr<Polygon2DRenderer> renderer;
	};
}
