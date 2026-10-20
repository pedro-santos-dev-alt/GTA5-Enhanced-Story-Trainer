#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

namespace shv {

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

    bool Bind(std::string& error) {
        error.clear();
        const HMODULE hook = GetModuleHandleW(L"ScriptHookV.dll");
        if (!hook) {
            error = "ScriptHookV.dll is not loaded in the game process.";
            return false;
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
