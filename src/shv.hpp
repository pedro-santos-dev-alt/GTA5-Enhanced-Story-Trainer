#pragma once

#include <Windows.h>
#include <winver.h>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

namespace shv {

inline bool GetEnhancedBuild(const std::wstring& path, DWORD& build, DWORD& patch,
                             std::string& error) {
    DWORD ignored{};
    const DWORD infoSize = GetFileVersionInfoSizeW(path.c_str(), &ignored);
    if (infoSize == 0) {
        error = "Could not read file version (Win32 error " + std::to_string(GetLastError()) + ").";
        return false;
    }

    std::vector<BYTE> versionData(infoSize);
    if (!GetFileVersionInfoW(path.c_str(), 0, infoSize, versionData.data())) {
        error = "Could not read file version details (Win32 error " + std::to_string(GetLastError()) + ").";
        return false;
    }

    void* fixedInfoData{};
    UINT fixedInfoSize{};
    if (!VerQueryValueW(versionData.data(), L"\\",
                        &fixedInfoData, &fixedInfoSize) ||
        !fixedInfoData || fixedInfoSize < sizeof(VS_FIXEDFILEINFO)) {
        error = "File does not contain a readable fixed version resource.";
        return false;
    }

    const auto* fixedInfo = static_cast<const VS_FIXEDFILEINFO*>(fixedInfoData);
    build = HIWORD(fixedInfo->dwProductVersionLS);
    patch = LOWORD(fixedInfo->dwProductVersionLS);
    return true;
}

inline bool ModulePath(HMODULE module, std::wstring& path) {
    wchar_t buffer[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(module, buffer, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) return false;
    path.assign(buffer, length);
    return true;
}

inline bool FileInModuleDirectory(HMODULE module, const wchar_t* filename,
                                  std::wstring& path) {
    if (!ModulePath(module, path)) return false;
    const size_t separator = path.find_last_of(L"\\/");
    if (separator == std::wstring::npos) return false;
    path.resize(separator + 1);
    path += filename;
    return true;
}

using ScriptMain = void (*)();
using ScriptRegisterFn = void(__cdecl*)(HMODULE, ScriptMain);
using ScriptUnregisterFn = void(__cdecl*)(HMODULE);
using ScriptWaitFn = void(__cdecl*)(DWORD);
using NativeInitFn = void(__cdecl*)(UINT64);
using NativePush64Fn = void(__cdecl*)(UINT64);
using NativeCallFn = UINT64*(__cdecl*)();

struct Api {
    ScriptRegisterFn scriptRegister{};
    ScriptUnregisterFn scriptUnregister{};
    ScriptWaitFn scriptWait{};
    NativeInitFn nativeInit{};
    NativePush64Fn nativePush64{};
    NativeCallFn nativeCall{};

    bool Bind(HMODULE pluginModule, std::string& error, bool& loadedHook) {
        error.clear();
        loadedHook = false;
        HMODULE hook = GetModuleHandleW(L"ScriptHookV.dll");
        std::wstring hookPath;
        if (hook) {
            if (!ModulePath(hook, hookPath)) {
                error = "INCOMPATIBLE: Could not resolve the loaded ScriptHookV.dll path.";
                return false;
            }
        } else if (!FileInModuleDirectory(pluginModule, L"ScriptHookV.dll", hookPath)) {
            error = "Could not resolve ScriptHookV.dll in the trainer's game folder.";
            return false;
        }

        std::wstring gamePath;
        if (!ModulePath(nullptr, gamePath)) {
            error = "Could not resolve the running GTA V Enhanced executable path.";
            return false;
        }

        DWORD hookBuild{}, hookPatch{}, gameBuild{}, gamePatch{};
        std::string versionError;
        if (!GetEnhancedBuild(hookPath, hookBuild, hookPatch, versionError)) {
            error = "Could not verify ScriptHookV.dll compatibility: " + versionError;
            return false;
        }
        if (!GetEnhancedBuild(gamePath, gameBuild, gamePatch, versionError)) {
            error = "Could not verify GTA V Enhanced compatibility: " + versionError;
            return false;
        }
        if (hookBuild != gameBuild || hookPatch != gamePatch) {
            error = "INCOMPATIBLE: ScriptHookV supports GTA V Enhanced " +
                std::to_string(hookBuild) + "." + std::to_string(hookPatch) +
                ", but the installed game is " + std::to_string(gameBuild) + "." +
                std::to_string(gamePatch) + ". The hook was not loaded.";
            return false;
        }

        if (!hook) {
            hook = LoadLibraryW(hookPath.c_str());
            if (!hook) {
                error = "Could not load ScriptHookV.dll from the game folder; Win32 error " +
                    std::to_string(GetLastError()) + ".";
                return false;
            }
            loadedHook = true;
        }

        // ScriptHookV exposes its SDK functions with MSVC C++ decorated names.
        scriptRegister = reinterpret_cast<ScriptRegisterFn>(GetProcAddress(
            hook, "?scriptRegister@@YAXPEAUHINSTANCE__@@P6AXXZ@Z"));
        scriptUnregister = reinterpret_cast<ScriptUnregisterFn>(GetProcAddress(
            hook, "?scriptUnregister@@YAXPEAUHINSTANCE__@@@Z"));
        scriptWait = reinterpret_cast<ScriptWaitFn>(GetProcAddress(
            hook, "?scriptWait@@YAXK@Z"));
        nativeInit = reinterpret_cast<NativeInitFn>(GetProcAddress(
            hook, "?nativeInit@@YAX_K@Z"));
        nativePush64 = reinterpret_cast<NativePush64Fn>(GetProcAddress(
            hook, "?nativePush64@@YAX_K@Z"));
        nativeCall = reinterpret_cast<NativeCallFn>(GetProcAddress(
            hook, "?nativeCall@@YAPEA_KXZ"));

        if (!scriptRegister) error = "ScriptHookV export is missing: scriptRegister.";
        else if (!scriptUnregister) error = "ScriptHookV export is missing: scriptUnregister.";
        else if (!scriptWait) error = "ScriptHookV export is missing: scriptWait.";
        else if (!nativeInit) error = "ScriptHookV export is missing: nativeInit.";
        else if (!nativePush64) error = "ScriptHookV export is missing: nativePush64.";
        else if (!nativeCall) error = "ScriptHookV export is missing: nativeCall.";
        return error.empty();
    }

    template <typename T>
    static UINT64 Pack(T value) {
        static_assert(sizeof(T) <= sizeof(UINT64), "Native arguments are at most 64 bits");
        UINT64 packed = 0;
        std::memcpy(&packed, &value, sizeof(T));
        return packed;
    }

    template <typename T, typename... Args>
    T Call(UINT64 hash, Args... args) const {
        nativeInit(hash);
        (nativePush64(Pack(args)), ...);
        UINT64* result = nativeCall();
        if constexpr (std::is_void_v<T>) {
            return;
        } else {
            return *reinterpret_cast<T*>(result);
        }
    }

    template <typename... Args>
    void CallVoid(UINT64 hash, Args... args) const {
        nativeInit(hash);
        (nativePush64(Pack(args)), ...);
        nativeCall();
    }
};

struct Vec3 {
    float x;
    UINT32 padX;
    float y;
    UINT32 padY;
    float z;
    UINT32 padZ;
};

static_assert(sizeof(Vec3) == 0x18, "ScriptHookV stores vector return components in 8-byte slots");

}  // namespace shv
