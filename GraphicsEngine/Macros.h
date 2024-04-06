#pragma once

#if IS_DEBUG && !PIX_DEBUG
#define STIMPLY_EXCEPTION(reason) { throw StimplyException(__LINE__, __FILEW__, reason); }
#define GRAPHICS_EXCEPTION(reason) { throw GraphicsException(__LINE__, __FILEW__, reason); }
#define GFX_THROW_FAILED(call) { (call); auto v = GraphicsDebug::GetErrors(); if (!v.empty()) { throw GraphicsException(__LINE__, __FILEW__, v); } }
#define HR_THROW_FAILED(hrcall) { HRESULT hr = (hrcall); if (FAILED(hr)) { throw HrException(__LINE__, __FILEW__, hr, GraphicsDebug::GetErrors()); } }
#define IMG_EXCEPTION(reason) { throw Texture::TextureImage::TextureImageLoadException(__LINE__, __FILEW__, reason); }
#else
#define STIMPLY_EXCEPTION(reason)
#define GRAPHICS_EXCEPTION(reason)
#define GFX_THROW_FAILED(hrcall) hrcall
#define IMG_EXCEPTION(reason)
#endif