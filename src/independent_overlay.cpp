#define IMGUI_DISABLE_INCLUDE_IMCONFIG_H

#include <imgui.h>
#include <reshade.hpp>

#include <Windows.h>
#include <winver.h>

#include <atomic>
#include <cstdio>
#include <iterator>
#include <string>
#include <vector>

namespace {

constexpr char kOverlayTitle[] = "GTA V Story Trainer";
HMODULE g_module{};
std::wstring g_logPath;
std::string g_gameVersion = "unknown";
std::atomic_bool g_registered{false};
std::atomic_bool g_overlayOpenedLogged{false};

void Log(const char* level, const std::string& message) {
    if (g_logPath.empty()) return;

    SYSTEMTIME now{};
    GetLocalTime(&now);
    char line[1024]{};
    const int length = sprintf_s(line, sizeof(line),
        "%04u-%02u-%02u %02u:%02u:%02u.%03u [%s] %s\r\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, now.wMilliseconds, level, message.c_str());
    if (length <= 0) return;

    const HANDLE file = CreateFileW(g_logPath.c_str(), FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    DWORD written{};
    WriteFile(file, line, static_cast<DWORD>(length), &written, nullptr);
    CloseHandle(file);
}

void InitializeLogPath(HMODULE module) {
    wchar_t path[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(module, path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) return;
    wchar_t* separator = wcsrchr(path, L'\\');
    if (!separator) return;
    *(separator + 1) = L'\0';
    if (wcscat_s(path, L"GTA5EnhancedStoryTrainerIndependent.log") != 0) return;
    g_logPath = path;
}

bool ReadGameVersion(std::string& version, std::string& error) {
    wchar_t path[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        error = "Could not resolve the running game executable path.";
        return false;
    }

    DWORD ignored{};
    const DWORD size = GetFileVersionInfoSizeW(path, &ignored);
    if (size == 0) {
        error = "Could not read the game executable version resource.";
        return false;
    }

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW(path, 0, size, data.data())) {
        error = "Could not read the game executable version details.";
        return false;
    }

    void* fixedData{};
    UINT fixedSize{};
    if (!VerQueryValueW(data.data(), L"\\", &fixedData, &fixedSize) ||
        !fixedData || fixedSize < sizeof(VS_FIXEDFILEINFO)) {
        error = "The game executable has no readable fixed version resource.";
        return false;
    }

    const auto* fixed = static_cast<const VS_FIXEDFILEINFO*>(fixedData);
    char formatted[64]{};
    sprintf_s(formatted, "%u.%u.%u.%u",
        HIWORD(fixed->dwProductVersionMS), LOWORD(fixed->dwProductVersionMS),
        HIWORD(fixed->dwProductVersionLS), LOWORD(fixed->dwProductVersionLS));
    version = formatted;
    return true;
}

void DrawUnavailableList(const char* const* items, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        ImGui::BulletText("%s  [indisponivel]", items[i]);
    }
}

void DrawOverlay(reshade::api::effect_runtime*) {
    if (!g_overlayOpenedLogged.exchange(true)) {
        Log("INFO", "Trainer overlay opened from the ReShade add-on menu.");
    }

    ImGui::TextUnformatted("GTA V ENHANCED  |  STORY MODE");
    ImGui::Separator();
    ImGui::Text("Jogo detetado: %s", g_gameVersion.c_str());
    ImGui::TextUnformatted("ScriptHookV: nao utilizado");
    ImGui::TextUnformatted("Camada visual: ReShade Add-on API");
    ImGui::TextUnformatted("Adaptador de jogo: indisponivel para esta build");
    ImGui::Spacing();
    ImGui::TextWrapped(
        "Este menu e independente do ScriptHookV. A interface esta ativa, mas as "
        "acoes de jogo ficam bloqueadas ate existir um adaptador nativo validado "
        "para a versao detetada. O ReShade desenha o menu; nao fornece as funcoes "
        "internas do GTA.");
    ImGui::Spacing();

    if (!ImGui::BeginTabBar("##story-trainer-tabs")) return;

    if (ImGui::BeginTabItem("Jogador")) {
        static constexpr const char* items[]{
            "Invencibilidade", "Recuperar vida", "Colete", "Limpar procurado"
        };
        DrawUnavailableList(items, std::size(items));
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Armas")) {
        static constexpr const char* items[]{
            "Adicionar arma", "Conjunto completo", "Municao infinita", "Remover armas"
        };
        DrawUnavailableList(items, std::size(items));
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Veiculos")) {
        static constexpr const char* items[]{
            "Criar veiculo", "Reparar", "Melhorias", "Pintura personalizada"
        };
        DrawUnavailableList(items, std::size(items));
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Mundo")) {
        static constexpr const char* items[]{
            "Alterar meteorologia", "Definir hora", "Parar relogio"
        };
        DrawUnavailableList(items, std::size(items));
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Diagnostico")) {
        ImGui::Text("Build detetada: %s", g_gameVersion.c_str());
        ImGui::TextWrapped("Log: GTA5EnhancedStoryTrainerIndependent.log na pasta do jogo.");
        ImGui::TextWrapped(
            "A cada atualizacao do GTA, a versao e detetada automaticamente. "
            "As funcoes de jogo so devem ser ativadas depois de validar um adaptador "
            "especifico para esse build.");
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
}

} // namespace

extern "C" __declspec(dllexport) const char* NAME = "GTA V Story Trainer Independent";
extern "C" __declspec(dllexport) const char* DESCRIPTION =
    "ScriptHookV-independent ReShade overlay and build diagnostics for GTA V Enhanced story mode.";

extern "C" __declspec(dllexport) bool AddonInit(HMODULE addonModule, HMODULE reshadeModule) {
    g_module = addonModule;
    InitializeLogPath(addonModule);
    Log("INFO", "Independent trainer add-on initialization started.");

    std::string versionError;
    if (ReadGameVersion(g_gameVersion, versionError)) {
        Log("INFO", "Detected GTA V Enhanced product version " + g_gameVersion + ".");
    } else {
        Log("WARN", versionError);
    }

    if (!reshade::register_addon(addonModule, reshadeModule)) {
        Log("ERROR", "Could not register with ReShade. Check that full add-on support is enabled.");
        return false;
    }

    reshade::register_overlay(kOverlayTitle, DrawOverlay);
    g_registered.store(true);
    Log("INFO", "Registered the independent trainer overlay with ReShade.");
    return true;
}

extern "C" __declspec(dllexport) void AddonUninit(HMODULE addonModule, HMODULE) {
    if (g_registered.exchange(false)) {
        reshade::unregister_addon(addonModule);
        Log("INFO", "Independent trainer add-on unloaded cleanly.");
    }
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
    }
    return TRUE;
}
