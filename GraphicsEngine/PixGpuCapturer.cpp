#ifdef PIX_DEBUG
#include "PixGpuCapturer.h"
#include <shlobj.h>
#include <strsafe.h>

PixGpuCapturer::PixGpuCapturer() {
    const wchar_t* pixPath = FindWinPixCapturerPath();
    bool isPixAlreadyLoaded = GetModuleHandleW(L"WinPixGpuCapturer.dll") != 0;

    if (!isPixAlreadyLoaded && pixPath != nullptr) {
        if (LoadLibrary(pixPath)) {
            printf("PIX Loaded!\n");
        }
    }

    delete[] pixPath;
}

const wchar_t* PixGpuCapturer::FindWinPixCapturerPath() const {

    wchar_t* programFilesPath = nullptr;
    SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

    wchar_t pixSearchPath[MAX_PATH];
    int written = _snwprintf_s(pixSearchPath, MAX_PATH, L"%ws%ws", programFilesPath, L"\\Microsoft PIX\\*");

    WIN32_FIND_DATA findData;
    bool foundPixInstallation = false;
    wchar_t newestVersionFound[MAX_PATH];

    HANDLE hFind = FindFirstFile(pixSearchPath, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
                (findData.cFileName[0] != '.')) {
                if (!foundPixInstallation || wcscmp(newestVersionFound, findData.cFileName) <= 0) {
                    foundPixInstallation = true;
                    StringCchCopy(newestVersionFound, _countof(newestVersionFound), findData.cFileName);
                }
            }
        } while (FindNextFile(hFind, &findData) != 0);
    }

    FindClose(hFind);

    if (!foundPixInstallation) {
        return nullptr;
    }

    wchar_t* output = new wchar_t[MAX_PATH];
    memset(output, 0, MAX_PATH);

    pixSearchPath[written - 1] = '\0';

    _snwprintf_s(output, MAX_PATH, MAX_PATH, L"%ws%ws%ws", pixSearchPath, newestVersionFound, L"\\WinPixGpuCapturer.dll");

    return &output[0];
}
#endif