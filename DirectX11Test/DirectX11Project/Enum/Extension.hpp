#pragma once
namespace Lobelia {
	enum class TextureExtension {
		NO_SUPPORT = -1,
		PNG,
		JPG,
		TGA,
		BMP,
	};
	__forceinline TextureExtension JudgeExtension(const char* file_path) {
		std::string filePath = file_path;
		std::string extension = FilePathControl::GetExtension(filePath);
		std::locale locale;
		std::transform(extension.cbegin(), extension.cend(), extension.begin(), tolower);
		if (extension == ".png")return TextureExtension::PNG;
		else if (extension == ".jpg")return TextureExtension::JPG;
		else if (extension == ".tga")return TextureExtension::TGA;
		else if (extension == ".bmp")return TextureExtension::BMP;
		return TextureExtension::NO_SUPPORT;

	}
}