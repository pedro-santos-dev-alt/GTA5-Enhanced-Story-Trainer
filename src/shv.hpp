#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>
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

    bool Bind() {
        const HMODULE hook = GetModuleHandleW(L"ScriptHookV.dll");
        if (!hook) return false;

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

        return scriptRegister && scriptUnregister && scriptWait && nativeInit &&
               nativePush64 && nativeCall;
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
