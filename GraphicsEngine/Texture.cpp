#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <sstream>
#include <string>

#include "Macros.h"
#include "GraphicsFabric.h"

Texture::Texture(std::string_view texturePath) {
	TextureImage image(texturePath);

	GraphicsFabric::CreateTextureImage(&image);
}

void Texture::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	
}

Texture::TextureImage::TextureImage(std::string_view path)
	:
	path(path),
	imageData(nullptr),
	width(0),
	height(0),
	channels(0),
	converted(false) {
	imageData = stbi_load(path.data(), &width, &height, &channels, 0);
	if (!imageData) {
		std::wstringstream oss;
		oss << "Failed to load " << path.data() << "\n" << stbi_failure_reason();
		IMG_EXCEPTION(oss.str());
	}
	if (!ConvertPng()) {
		ConvertXXXA();
	}
}

Texture::TextureImage::~TextureImage() {
	if (imageData) {
		if (converted) {
			delete[] imageData;
		} else {
			stbi_image_free(imageData);
		}
	}
}

DXGI_FORMAT Texture::TextureImage::GetImageFormat() const noexcept(!IS_DEBUG) {
	switch (channels) {
		case 1:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case 4:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	IMG_EXCEPTION(L"Unknown image format");
	return (DXGI_FORMAT)-1;
}

UINT Texture::TextureImage::GetImagePitch() const noexcept {
	return (UINT)width * channels;
}

UINT Texture::TextureImage::GetWidth() const noexcept {
	return (UINT)width;
}

UINT Texture::TextureImage::GetHeight() const noexcept {
	return (UINT)height;
}

const void* Texture::TextureImage::GetImageRawData() const noexcept {
	return imageData;
}

SIZE_T Texture::TextureImage::GetTextureSize() const noexcept {
	return width * height * channels;
}

bool Texture::TextureImage::ConvertPng() noexcept(!IS_DEBUG) {
	if (channels == 3) {
		struct JPGColor {
			unsigned char r;
			unsigned char g;
			unsigned char b;
		};

		struct PNGColor {
			PNGColor(JPGColor pc)
				:
				r(pc.r),
				g(pc.g),
				b(pc.b),
				a(0) {}
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};

		size_t pngSize = width * height * sizeof(PNGColor);
		unsigned char* png = new unsigned char[pngSize];

		size_t pixelColorIndex = 0;

		for (int h = 0; h < height * 3; h += 3) {
			for (int w = 0; w < width * 3; w += 3) {
				size_t colorIndex = (h * height) + w;
				JPGColor* jc = (JPGColor*)&imageData[colorIndex];
				PNGColor pc(*jc);
				memcpy(&png[pixelColorIndex], &pc, sizeof(pc));
				pixelColorIndex += 4;
			}
		}

		channels = 4;
		converted = true;

		stbi_image_free(imageData);

		imageData = png;

		return true;
	} else {
		converted = false;
		return false;
	}
}

void Texture::TextureImage::ConvertXXXA() noexcept(!IS_DEBUG) {
	if (channels == 1 && !converted) {
		struct PNGColor {
			PNGColor(unsigned char a)
				:
				b(a),
				g(a),
				r(a),
				a(a) {}
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};

		size_t pngSize = width * height * sizeof(PNGColor);
		unsigned char* png = new unsigned char[pngSize];

		size_t pngIndex = 0;

		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				size_t colorIndex = (h * height) + w;
				PNGColor col = PNGColor(imageData[colorIndex]);
				memcpy(&png[pngIndex], &col, sizeof(PNGColor));
				pngIndex += 4;
			}
		}

		converted = true;
		channels = 4;

		stbi_image_free(imageData);

		imageData = png;
	}
}

Texture::TextureImage::TextureImageLoadException::TextureImageLoadException(int line, const wchar_t* file, std::wstring_view reason) noexcept
	:
	StimplyException(line, file, reason)
{}

const wchar_t* Texture::TextureImage::TextureImageLoadException::GetType() const noexcept {
	return L"Texture Image Load Exception";
}
