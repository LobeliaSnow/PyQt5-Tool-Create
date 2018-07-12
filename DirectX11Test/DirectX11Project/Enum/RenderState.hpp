#pragma once
namespace Lobelia {
	enum class BlendPreset {
		MIN = -1,
		NONE,
		COPY,
		ADD,
		SUB,
		REPLACE,
		MULTIPLY,
		LIGHTENESS,
		DARKENESS,
		SCREEN,
		MAX
	};
	enum class SamplerPreset {
		MIN = -1,
		POINT,
		LINEAR,
		ANISOTROPIC,
		COMPARISON_POINT,
		COMPARISON_LINEAR,
		COMPARISON_ANISOTROPIC,
		MAX
	};
	enum class RasterizerPreset {
		MIN = -1,
		FRONT,
		BACK,
		NONE,
		MAX
	};
	enum class DepthPreset {
		MIN = -1, 
		NEVER, 
		LESS, 
		EQUAL, 
		LESS_EQUAL, 
		GREATER, 
		NOT_EQUAL, 
		GREATER_EQUAL, 
		ALWAYS, 
		MAX
	};

}