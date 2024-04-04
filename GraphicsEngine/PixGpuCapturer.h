#pragma once
#ifdef PIX_DEBUG

class PixGpuCapturer {
public:
	PixGpuCapturer();

private:
	const wchar_t* FindWinPixCapturerPath() const;
};
#endif