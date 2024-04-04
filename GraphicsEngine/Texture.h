#pragma once

#include "D3DIncls.inl"
#include "Bindables.inl"
#include "StimplyException.h"

class Texture : public IBindable {
public:
	class TextureImage {
	private:
		class TextureImageLoadException : public StimplyException {
		public:
			TextureImageLoadException(int line, const wchar_t* file, std::wstring_view reason) noexcept;
			virtual const wchar_t* GetType() const noexcept;
		};
	public:
		TextureImage(std::string_view path);
		~TextureImage();

		DXGI_FORMAT GetImageFormat() const noexcept(!IS_DEBUG);
		UINT GetImagePitch() const noexcept;
		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
		const void* GetImageRawData() const noexcept;
		bool HasAlpha() const noexcept { return !converted; }
		SIZE_T GetTextureSize() const noexcept;
	private:
		bool ConvertPng() noexcept(!IS_DEBUG);
		void ConvertXXXA() noexcept(!IS_DEBUG);
	private:
		std::string path;
		unsigned char* imageData;
		int width;
		int height;
		int channels;
		bool converted;
	};
public:
	Texture(std::string_view texturePath);
	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept override;

};

