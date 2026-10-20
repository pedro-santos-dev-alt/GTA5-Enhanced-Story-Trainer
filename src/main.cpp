#include "shv.hpp"

#include <Windows.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <vector>

namespace {

constexpr UINT64 NETWORK_IS_GAME_IN_PROGRESS = 0xD83C2B94E7508980;
constexpr UINT64 PLAYER_PED_ID = 0xD80958FC74E988A6;
constexpr UINT64 PLAYER_ID = 0x4F8644AF03D0E0D6;
constexpr UINT64 GET_ENTITY_COORDS = 0x3FEF770D40960D5A;
constexpr UINT64 GET_ENTITY_HEADING = 0xE83D4F9BA2A38914;
constexpr UINT64 GET_ENTITY_MAX_HEALTH = 0x15D757606D170C3C;
constexpr UINT64 SET_ENTITY_HEALTH = 0x6B76DC1F3AE6E6A3;
constexpr UINT64 SET_ENTITY_INVINCIBLE = 0x3882114BDE571AD4;
constexpr UINT64 SET_PED_ARMOUR = 0xCEA04D83135264CC;
constexpr UINT64 SET_PLAYER_WANTED_LEVEL = 0x39FF19C64EF7DA5B;
constexpr UINT64 SET_PLAYER_WANTED_LEVEL_NOW = 0xE0A7D1E497FFCD6F;
constexpr UINT64 SET_MAX_WANTED_LEVEL = 0xAA5F02DB48D704B9;
constexpr UINT64 GIVE_WEAPON_TO_PED = 0xBF0FD6E56C964FCB;
constexpr UINT64 SET_PED_INFINITE_AMMO_CLIP = 0x183DADC6AA953186;
constexpr UINT64 REMOVE_ALL_PED_WEAPONS = 0xF25DF915FA38C5F3;
constexpr UINT64 SET_CURRENT_PED_WEAPON = 0xADF692B254977C0C;
constexpr UINT64 GET_HASH_KEY = 0xD24D37CC275948CC;
constexpr UINT64 IS_MODEL_IN_CDIMAGE = 0x35B9E0803292B641;
constexpr UINT64 IS_MODEL_A_VEHICLE = 0x19AAC8F07BFEC53E;
constexpr UINT64 REQUEST_MODEL = 0x963D27A58DF860AC;
constexpr UINT64 HAS_MODEL_LOADED = 0x98A4EB5D89A0C952;
constexpr UINT64 SET_MODEL_AS_NO_LONGER_NEEDED = 0xE532F5D78798DAAB;
constexpr UINT64 CREATE_VEHICLE = 0xAF35D0D2583051B0;
constexpr UINT64 SET_PED_INTO_VEHICLE = 0xF75B0D629E1C063D;
constexpr UINT64 GET_VEHICLE_PED_IS_IN = 0x9A9112A0FE9A4713;
constexpr UINT64 SET_VEHICLE_ON_GROUND_PROPERLY = 0x49733E92263139D1;
constexpr UINT64 SET_VEHICLE_MOD_KIT = 0x1F2AA07F00B3217A;
constexpr UINT64 GET_NUM_VEHICLE_MODS = 0xE38E9162A2500646;
constexpr UINT64 SET_VEHICLE_MOD = 0x6AF0636DDEDCB6DD;
constexpr UINT64 TOGGLE_VEHICLE_MOD = 0x2A1F4F37F95BAD08;
constexpr UINT64 FIX_VEHICLE = 0x115722B1B9C14C1C;
constexpr UINT64 SET_VEHICLE_COLOURS = 0x4F1D4BE3A7F24601;
constexpr UINT64 SET_VEHICLE_CUSTOM_PRIMARY_COLOUR = 0x7141766F91D15BEA;
constexpr UINT64 SET_VEHICLE_CUSTOM_SECONDARY_COLOUR = 0x36CED73BFED89754;
constexpr UINT64 SET_VEHICLE_DIRT_LEVEL = 0x79D3B596FE44EE8B;
constexpr UINT64 SET_VEHICLE_NUMBER_PLATE_TEXT = 0x95A88F0B409CDA47;
constexpr UINT64 PAUSE_CLOCK = 0xB02D61207FD9CB7D;
constexpr UINT64 SET_CLOCK_TIME = 0x47C3B5848C3E45D8;
constexpr UINT64 SET_WEATHER_TYPE_NOW_PERSIST = 0xED712CA327900C8A;
constexpr UINT64 CLEAR_WEATHER_TYPE_PERSIST = 0x338D2E3477711050;
constexpr UINT64 DRAW_RECT = 0x3A618A217E5154F0;
constexpr UINT64 SET_TEXT_FONT = 0x66E0276CC5F6B9DA;
constexpr UINT64 SET_TEXT_SCALE = 0x07C837F9A01C34C9;
constexpr UINT64 SET_TEXT_COLOUR = 0xBE6B23FFA53FB442;
constexpr UINT64 SET_TEXT_OUTLINE = 0x2513DFB0FB8400FE;
constexpr UINT64 BEGIN_TEXT_COMMAND_DISPLAY_TEXT = 0x25FBB336DF1804CB;
constexpr UINT64 ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME = 0x6C188BE134E074AA;
constexpr UINT64 END_TEXT_COMMAND_DISPLAY_TEXT = 0xCD015E5BB0D96A57;

shv::Api g_api;
HMODULE g_module{};
std::wstring g_logPath;
std::array<bool, 256> g_previousKeyDown{};

enum class Page { Main, Player, Weapons, Vehicle, World };

struct State {
    Page page{Page::Main};
    int selected{};
    int vehicleIndex{};
    int weaponIndex{};
    int modType{};
    int modLevel{};
    int weatherIndex{};
    int hour{12};
    int protectedVehicle{};
    int primaryRed{40};
    int primaryGreen{140};
    int primaryBlue{255};
    int secondaryRed{20};
    int secondaryGreen{30};
    int secondaryBlue{45};
    bool menuOpen{};
    bool godMode{};
    bool neverWanted{};
    bool infiniteAmmo{};
    bool vehicleGodMode{};
    bool freezeClock{};
    bool statusIsError{};
    DWORD statusUntil{};
    std::string status;
} g_state;

constexpr std::array<const char*, 34> kVehicles{
    "adder", "zentorno", "t20", "osiris", "krieger", "emerus", "ignus",
    "comet7", "buffalo4", "kuruma2", "sultan3", "elegy2", "bati",
    "hakuchou2", "oppressor2", "deluxo", "vigilante", "toreador",
    "insurgent3", "nightshark", "buzzard2", "savage", "hydra", "raiju",
    "rhino", "police", "police2", "police3", "police4", "police5",
    "sheriff", "firetruk", "ambulance", "cargoplane"
};

constexpr std::array<const char*, 26> kWeapons{
    "WEAPON_PISTOL", "WEAPON_COMBATPISTOL", "WEAPON_APPISTOL", "WEAPON_PISTOL50",
    "WEAPON_SNSPISTOL", "WEAPON_HEAVYPISTOL", "WEAPON_MICROSMG", "WEAPON_SMG",
    "WEAPON_ASSAULTSMG", "WEAPON_ASSAULTRIFLE", "WEAPON_CARBINERIFLE",
    "WEAPON_SPECIALCARBINE", "WEAPON_BULLPUPRIFLE", "WEAPON_MG", "WEAPON_COMBATMG",
    "WEAPON_PUMPSHOTGUN", "WEAPON_ASSAULTSHOTGUN", "WEAPON_BULLPUPSHOTGUN",
    "WEAPON_SNIPERRIFLE", "WEAPON_HEAVYSNIPER", "WEAPON_GRENADELAUNCHER",
    "WEAPON_RPG", "WEAPON_HOMINGLAUNCHER", "WEAPON_MINIGUN", "WEAPON_GRENADE",
    "WEAPON_STICKYBOMB"
};

constexpr std::array<const char*, 14> kWeather{
    "EXTRASUNNY", "CLEAR", "CLOUDS", "OVERCAST", "RAIN", "CLEARING", "THUNDER",
    "SMOG", "FOGGY", "XMAS", "SNOW", "SNOWLIGHT", "BLIZZARD", "HALLOWEEN"
};

template <typename T, typename... Args>
T Native(UINT64 hash, Args... args) {
    return g_api.Call<T>(hash, args...);
}

template <typename... Args>
void NativeVoid(UINT64 hash, Args... args) {
    g_api.CallVoid(hash, args...);
}

void Log(const char* level, const std::string& message) {
    if (g_logPath.empty()) return;

    SYSTEMTIME now{};
    GetLocalTime(&now);
    char line[2048]{};
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
    FlushFileBuffers(file);
    CloseHandle(file);
}

bool PollKey(int key) {
    const size_t index = static_cast<size_t>(key);
    const bool down = (GetAsyncKeyState(key) & 0x8000) != 0;
    const bool pressed = down && !g_previousKeyDown[index];
    g_previousKeyDown[index] = down;
    return pressed;
}

struct KeyPresses {
    bool f10{};
    bool up{};
    bool down{};
    bool back{};
    bool left{};
    bool right{};
    bool enter{};
};

KeyPresses PollKeys() {
    // Sample all controls every game tick, even when another key also changed.
    return {PollKey(VK_F10), PollKey(VK_UP), PollKey(VK_DOWN), PollKey(VK_BACK),
            PollKey(VK_LEFT), PollKey(VK_RIGHT), PollKey(VK_RETURN)};
}

bool InitializeLogger() {
    wchar_t modulePath[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(g_module, modulePath, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) return false;

    wchar_t* separator = wcsrchr(modulePath, L'\\');
    if (!separator) separator = wcsrchr(modulePath, L'/');
    if (!separator) return false;
    *(separator + 1) = L'\0';
    if (wcscat_s(modulePath, L"GTA5EnhancedStoryTrainer.log") != 0) return false;
    g_logPath = modulePath;

    const HANDLE file = CreateFileW(g_logPath.c_str(), GENERIC_WRITE,
        FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        g_logPath.clear();
        return false;
    }
    CloseHandle(file);
    return true;
}

void SetStatus(const std::string& message, bool error = false) {
    g_state.status = message;
    g_state.statusIsError = error;
    g_state.statusUntil = GetTickCount() + 3000;
    Log(error ? "WARN" : "INFO", message);
}

bool InOnlineSession() {
    return Native<BOOL>(NETWORK_IS_GAME_IN_PROGRESS) != FALSE;
}

int PlayerPed() {
    return Native<int>(PLAYER_PED_ID);
}

int CurrentVehicle() {
    const int ped = PlayerPed();
    return Native<int>(GET_VEHICLE_PED_IS_IN, ped, FALSE);
}

shv::Vec3 PlayerCoords() {
    return Native<shv::Vec3>(GET_ENTITY_COORDS, PlayerPed(), TRUE);
}

void DrawRect(float x, float y, float width, float height, int r, int g, int b, int a) {
    NativeVoid(DRAW_RECT, x, y, width, height, r, g, b, a);
}

void DrawText(const std::string& text, float x, float y, float scale,
              int r = 238, int g = 242, int b = 248, int a = 255) {
    NativeVoid(SET_TEXT_FONT, 0);
    NativeVoid(SET_TEXT_SCALE, 0.0f, scale);
    NativeVoid(SET_TEXT_COLOUR, r, g, b, a);
    NativeVoid(SET_TEXT_OUTLINE);
    NativeVoid(BEGIN_TEXT_COMMAND_DISPLAY_TEXT, "STRING");
    NativeVoid(ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME, text.c_str());
    NativeVoid(END_TEXT_COMMAND_DISPLAY_TEXT, x, y);
}

std::string PageTitle() {
    switch (g_state.page) {
        case Page::Player: return "PLAYER";
        case Page::Weapons: return "WEAPONS";
        case Page::Vehicle: return "VEHICLE";
        case Page::World: return "WORLD";
        default: return "STORY TRAINER";
    }
}

struct MenuItem {
    std::string label;
    std::function<void()> activate;
    std::function<void(int)> adjust;
    bool hasAdjust{};
};

void OpenPage(Page page) {
    g_state.page = page;
    g_state.selected = 0;
}

bool RequireVehicle(int& vehicle) {
    vehicle = CurrentVehicle();
    if (vehicle == 0) {
        SetStatus("Enter a vehicle first.", true);
        return false;
    }
    return true;
}

void SpawnSelectedVehicle() {
    const char* modelName = kVehicles[static_cast<size_t>(g_state.vehicleIndex)];
    const UINT32 model = Native<UINT32>(GET_HASH_KEY, modelName);

    if (!Native<BOOL>(IS_MODEL_IN_CDIMAGE, model) ||
        !Native<BOOL>(IS_MODEL_A_VEHICLE, model)) {
        SetStatus(std::string("Invalid vehicle model: ") + modelName, true);
        return;
    }

    NativeVoid(REQUEST_MODEL, model);
    const DWORD started = GetTickCount();
    while (!Native<BOOL>(HAS_MODEL_LOADED, model)) {
        if (GetTickCount() - started > 8000) {
            NativeVoid(SET_MODEL_AS_NO_LONGER_NEEDED, model);
            SetStatus("Vehicle model timed out.", true);
            return;
        }
        g_api.scriptWait(0);
    }

    const int ped = PlayerPed();
    const shv::Vec3 coords = PlayerCoords();
    const float heading = Native<float>(GET_ENTITY_HEADING, ped);
    const float headingRadians = heading * 0.01745329252f;
    const float spawnX = coords.x + std::sin(headingRadians) * 5.0f;
    const float spawnY = coords.y + std::cos(headingRadians) * 5.0f;
    const int vehicle = Native<int>(CREATE_VEHICLE, model, spawnX, spawnY,
                                    coords.z + 0.5f, heading, FALSE, FALSE, FALSE);
    if (vehicle == 0) {
        NativeVoid(SET_MODEL_AS_NO_LONGER_NEEDED, model);
        SetStatus("Vehicle creation failed.", true);
        return;
    }

    NativeVoid(SET_VEHICLE_ON_GROUND_PROPERLY, vehicle);
    NativeVoid(SET_PED_INTO_VEHICLE, ped, vehicle, -1);
    NativeVoid(SET_VEHICLE_MOD_KIT, vehicle, 0);
    NativeVoid(SET_MODEL_AS_NO_LONGER_NEEDED, model);
    SetStatus(std::string("Spawned: ") + modelName);
}

void GiveWeaponLoadout() {
    const int ped = PlayerPed();
    for (const char* weapon : kWeapons) {
        const UINT32 hash = Native<UINT32>(GET_HASH_KEY, weapon);
        NativeVoid(GIVE_WEAPON_TO_PED, ped, hash, 9999, FALSE, FALSE);
    }
    const UINT32 pistol = Native<UINT32>(GET_HASH_KEY, "WEAPON_PISTOL");
    NativeVoid(SET_CURRENT_PED_WEAPON, ped, pistol, TRUE);
    SetStatus("Full weapon loadout added.");
}

void GiveSelectedWeapon() {
    const char* weaponName = kWeapons[static_cast<size_t>(g_state.weaponIndex)];
    const UINT32 weapon = Native<UINT32>(GET_HASH_KEY, weaponName);
    NativeVoid(GIVE_WEAPON_TO_PED, PlayerPed(), weapon, 9999, FALSE, TRUE);
    SetStatus(std::string("Weapon added: ") + weaponName);
}

void MaxVehicleMods() {
    int vehicle{};
    if (!RequireVehicle(vehicle)) return;

    NativeVoid(SET_VEHICLE_MOD_KIT, vehicle, 0);
    for (int modType = 0; modType < 50; ++modType) {
        const int count = Native<int>(GET_NUM_VEHICLE_MODS, vehicle, modType);
        if (count > 0) NativeVoid(SET_VEHICLE_MOD, vehicle, modType, count - 1, FALSE);
    }
    for (const int modType : {17, 18, 19, 20, 21, 22}) {
        NativeVoid(TOGGLE_VEHICLE_MOD, vehicle, modType, TRUE);
    }
    SetStatus("Maximum available vehicle upgrades applied.");
}

void RandomVehicleColour() {
    int vehicle{};
    if (!RequireVehicle(vehicle)) return;
    const int primary = rand() % 160;
    const int secondary = rand() % 160;
    NativeVoid(SET_VEHICLE_COLOURS, vehicle, primary, secondary);
    NativeVoid(SET_VEHICLE_CUSTOM_PRIMARY_COLOUR, vehicle, rand() % 256,
               rand() % 256, rand() % 256);
    NativeVoid(SET_VEHICLE_CUSTOM_SECONDARY_COLOUR, vehicle, rand() % 256,
               rand() % 256, rand() % 256);
    SetStatus("Random custom colours applied.");
}

void ApplyVehiclePaint() {
    int vehicle{};
    if (!RequireVehicle(vehicle)) return;
    NativeVoid(SET_VEHICLE_CUSTOM_PRIMARY_COLOUR, vehicle,
               g_state.primaryRed, g_state.primaryGreen, g_state.primaryBlue);
    NativeVoid(SET_VEHICLE_CUSTOM_SECONDARY_COLOUR, vehicle,
               g_state.secondaryRed, g_state.secondaryGreen, g_state.secondaryBlue);
    SetStatus("Custom paint applied.");
}

void ApplySelectedVehicleMod() {
    int vehicle{};
    if (!RequireVehicle(vehicle)) return;
    NativeVoid(SET_VEHICLE_MOD_KIT, vehicle, 0);
    const int count = Native<int>(GET_NUM_VEHICLE_MODS, vehicle, g_state.modType);
    if (count <= 0) {
        SetStatus("This vehicle has no parts for that mod slot.", true);
        return;
    }
    g_state.modLevel = std::clamp(g_state.modLevel, 0, count - 1);
    NativeVoid(SET_VEHICLE_MOD, vehicle, g_state.modType, g_state.modLevel, FALSE);
    SetStatus("Vehicle part applied.");
}

void RepairVehicle() {
    int vehicle{};
    if (!RequireVehicle(vehicle)) return;
    NativeVoid(FIX_VEHICLE, vehicle);
    NativeVoid(SET_VEHICLE_DIRT_LEVEL, vehicle, 0.0f);
    SetStatus("Vehicle repaired and cleaned.");
}

std::vector<MenuItem> BuildItems() {
    switch (g_state.page) {
        case Page::Main:
            return {
                {"Player", [] { OpenPage(Page::Player); }},
                {"Weapons", [] { OpenPage(Page::Weapons); }},
                {"Vehicle", [] { OpenPage(Page::Vehicle); }},
                {"World", [] { OpenPage(Page::World); }},
                {"Close menu", [] { g_state.menuOpen = false; }}
            };

        case Page::Player:
            return {
                {std::string("God mode: ") + (g_state.godMode ? "ON" : "OFF"), [] {
                    g_state.godMode = !g_state.godMode;
                    NativeVoid(SET_ENTITY_INVINCIBLE, PlayerPed(), g_state.godMode);
                    SetStatus(g_state.godMode ? "God mode enabled." : "God mode disabled.");
                }},
                {"Restore health", [] {
                    const int ped = PlayerPed();
                    const int maxHealth = Native<int>(GET_ENTITY_MAX_HEALTH, ped);
                    NativeVoid(SET_ENTITY_HEALTH, ped, maxHealth, 0);
                    SetStatus("Health restored.");
                }},
                {"Set armour to 100", [] {
                    NativeVoid(SET_PED_ARMOUR, PlayerPed(), 100);
                    SetStatus("Armour set to 100.");
                }},
                {"Clear wanted level", [] {
                    const int player = Native<int>(PLAYER_ID);
                    NativeVoid(SET_PLAYER_WANTED_LEVEL, player, 0, FALSE);
                    NativeVoid(SET_PLAYER_WANTED_LEVEL_NOW, player, FALSE);
                    SetStatus("Wanted level cleared.");
                }},
                {std::string("Never wanted: ") + (g_state.neverWanted ? "ON" : "OFF"), [] {
                    g_state.neverWanted = !g_state.neverWanted;
                    NativeVoid(SET_MAX_WANTED_LEVEL, g_state.neverWanted ? 0 : 5);
                    if (g_state.neverWanted) {
                        const int player = Native<int>(PLAYER_ID);
                        NativeVoid(SET_PLAYER_WANTED_LEVEL, player, 0, FALSE);
                        NativeVoid(SET_PLAYER_WANTED_LEVEL_NOW, player, FALSE);
                    }
                    SetStatus(g_state.neverWanted ? "Never wanted enabled." : "Never wanted disabled.");
                }}
            };

        case Page::Weapons:
            return {
                {std::string("Weapon: ") + kWeapons[static_cast<size_t>(g_state.weaponIndex)],
                 GiveSelectedWeapon,
                 [](int direction) {
                     const int count = static_cast<int>(kWeapons.size());
                     g_state.weaponIndex = (g_state.weaponIndex + direction + count) % count;
                 }, true},
                {"Give full weapon loadout", GiveWeaponLoadout},
                {std::string("Infinite ammo clip: ") + (g_state.infiniteAmmo ? "ON" : "OFF"), [] {
                    g_state.infiniteAmmo = !g_state.infiniteAmmo;
                    NativeVoid(SET_PED_INFINITE_AMMO_CLIP, PlayerPed(), g_state.infiniteAmmo);
                    SetStatus(g_state.infiniteAmmo ? "Infinite ammo enabled." : "Infinite ammo disabled.");
                }},
                {"Remove all weapons", [] {
                    NativeVoid(REMOVE_ALL_PED_WEAPONS, PlayerPed(), TRUE);
                    SetStatus("All weapons removed.");
                }}
            };

        case Page::Vehicle:
            return {
                {std::string("Spawn: ") + kVehicles[static_cast<size_t>(g_state.vehicleIndex)],
                 SpawnSelectedVehicle,
                 [](int direction) {
                     const int count = static_cast<int>(kVehicles.size());
                     g_state.vehicleIndex = (g_state.vehicleIndex + direction + count) % count;
                 }, true},
                {"Repair and clean current vehicle", RepairVehicle},
                {"Max available upgrades", MaxVehicleMods},
                {"Mod slot: " + std::to_string(g_state.modType), [] {},
                 [](int direction) {
                     g_state.modType = (g_state.modType + direction + 50) % 50;
                     g_state.modLevel = 0;
                 }, true},
                {"Mod level: " + std::to_string(g_state.modLevel), ApplySelectedVehicleMod,
                 [](int direction) {
                     g_state.modLevel = std::clamp(g_state.modLevel + direction, 0, 20);
                 }, true},
                {"Random custom paint", RandomVehicleColour},
                {"Apply custom paint", ApplyVehiclePaint},
                {"Primary red: " + std::to_string(g_state.primaryRed), ApplyVehiclePaint,
                 [](int direction) { g_state.primaryRed = std::clamp(g_state.primaryRed + direction * 16, 0, 255); }, true},
                {"Primary green: " + std::to_string(g_state.primaryGreen), ApplyVehiclePaint,
                 [](int direction) { g_state.primaryGreen = std::clamp(g_state.primaryGreen + direction * 16, 0, 255); }, true},
                {"Primary blue: " + std::to_string(g_state.primaryBlue), ApplyVehiclePaint,
                 [](int direction) { g_state.primaryBlue = std::clamp(g_state.primaryBlue + direction * 16, 0, 255); }, true},
                {"Secondary red: " + std::to_string(g_state.secondaryRed), ApplyVehiclePaint,
                 [](int direction) { g_state.secondaryRed = std::clamp(g_state.secondaryRed + direction * 16, 0, 255); }, true},
                {"Secondary green: " + std::to_string(g_state.secondaryGreen), ApplyVehiclePaint,
                 [](int direction) { g_state.secondaryGreen = std::clamp(g_state.secondaryGreen + direction * 16, 0, 255); }, true},
                {"Secondary blue: " + std::to_string(g_state.secondaryBlue), ApplyVehiclePaint,
                 [](int direction) { g_state.secondaryBlue = std::clamp(g_state.secondaryBlue + direction * 16, 0, 255); }, true},
                {"Set plate to STORY", [] {
                    int vehicle{};
                    if (!RequireVehicle(vehicle)) return;
                    NativeVoid(SET_VEHICLE_NUMBER_PLATE_TEXT, vehicle, "STORY");
                    SetStatus("Plate changed to STORY.");
                }},
                {std::string("Vehicle god mode: ") + (g_state.vehicleGodMode ? "ON" : "OFF"), [] {
                    int vehicle{};
                    if (!RequireVehicle(vehicle)) return;
                    g_state.vehicleGodMode = !g_state.vehicleGodMode;
                    g_state.protectedVehicle = g_state.vehicleGodMode ? vehicle : 0;
                    NativeVoid(SET_ENTITY_INVINCIBLE, vehicle, g_state.vehicleGodMode);
                    SetStatus(g_state.vehicleGodMode ? "Vehicle god mode enabled." : "Vehicle god mode disabled.");
                }}
            };

        case Page::World:
            return {
                {"Weather: " + std::string(kWeather[static_cast<size_t>(g_state.weatherIndex)]),
                 [] {
                     NativeVoid(SET_WEATHER_TYPE_NOW_PERSIST,
                                kWeather[static_cast<size_t>(g_state.weatherIndex)]);
                     SetStatus(std::string("Weather: ") + kWeather[static_cast<size_t>(g_state.weatherIndex)]);
                 },
                 [](int direction) {
                     const int count = static_cast<int>(kWeather.size());
                     g_state.weatherIndex = (g_state.weatherIndex + direction + count) % count;
                     NativeVoid(SET_WEATHER_TYPE_NOW_PERSIST,
                                kWeather[static_cast<size_t>(g_state.weatherIndex)]);
                 }, true},
                {"Clear persistent weather", [] {
                    NativeVoid(CLEAR_WEATHER_TYPE_PERSIST);
                    SetStatus("Persistent weather cleared.");
                }},
                {"Hour: " + std::to_string(g_state.hour), [] {
                    NativeVoid(SET_CLOCK_TIME, g_state.hour, 0, 0);
                    SetStatus("Game time updated.");
                },
                 [](int direction) {
                     g_state.hour = (g_state.hour + direction + 24) % 24;
                     NativeVoid(SET_CLOCK_TIME, g_state.hour, 0, 0);
                 }, true},
                {std::string("Freeze clock: ") + (g_state.freezeClock ? "ON" : "OFF"), [] {
                    g_state.freezeClock = !g_state.freezeClock;
                    NativeVoid(PAUSE_CLOCK, g_state.freezeClock);
                    SetStatus(g_state.freezeClock ? "Clock frozen." : "Clock resumed.");
                }}
            };
    }
    return {};
}

void DrawMenu(const std::vector<MenuItem>& items) {
    constexpr float left = 0.055f;
    constexpr float top = 0.16f;
    constexpr float width = 0.29f;
    constexpr float rowHeight = 0.034f;
    const float height = 0.085f + rowHeight * static_cast<float>(items.size()) + 0.045f;

    DrawRect(left + width * 0.5f, top + height * 0.5f, width, height, 14, 19, 29, 225);
    DrawRect(left + width * 0.5f, top + 0.025f, width, 0.05f, 34, 113, 190, 245);
    DrawText("GTA V ENHANCED  |  STORY MODE", left + 0.012f, top + 0.009f, 0.30f);
    DrawText(PageTitle(), left + 0.012f, top + 0.060f, 0.30f, 155, 202, 255);

    for (size_t i = 0; i < items.size(); ++i) {
        const float y = top + 0.094f + rowHeight * static_cast<float>(i);
        const bool selected = static_cast<int>(i) == g_state.selected;
        if (selected) DrawRect(left + width * 0.5f, y + 0.012f, width - 0.012f, rowHeight - 0.002f, 58, 87, 121, 235);
        DrawText(items[i].label, left + 0.012f, y, 0.28f,
                 selected ? 255 : 220, selected ? 255 : 226, selected ? 255 : 235);
        if (selected && items[i].hasAdjust) {
            DrawText("<   >", left + width - 0.060f, y, 0.25f, 155, 202, 255);
        }
    }

    const float footerY = top + height - 0.034f;
    DrawText("F10 open/close   Arrows move   Enter select   Backspace back/close",
             left + 0.012f, footerY, 0.19f, 170, 180, 195);

    if (GetTickCount() < g_state.statusUntil && !g_state.status.empty()) {
        DrawRect(0.5f, 0.94f, 0.45f, 0.045f, 10, 14, 22, 220);
        DrawText(g_state.status, 0.285f, 0.927f, 0.30f,
                 g_state.statusIsError ? 255 : 210,
                 g_state.statusIsError ? 125 : 235,
                 g_state.statusIsError ? 125 : 255);
    }
}

void HandleMenuInput(const std::vector<MenuItem>& items, const KeyPresses& keys) {
    if (items.empty()) return;

    if (keys.up) {
        g_state.selected = (g_state.selected + static_cast<int>(items.size()) - 1) % static_cast<int>(items.size());
    } else if (keys.down) {
        g_state.selected = (g_state.selected + 1) % static_cast<int>(items.size());
    } else if (keys.back) {
        if (g_state.page == Page::Main) g_state.menuOpen = false;
        else OpenPage(Page::Main);
    } else if (keys.left && items[static_cast<size_t>(g_state.selected)].hasAdjust) {
        items[static_cast<size_t>(g_state.selected)].adjust(-1);
    } else if (keys.right && items[static_cast<size_t>(g_state.selected)].hasAdjust) {
        items[static_cast<size_t>(g_state.selected)].adjust(1);
    } else if (keys.enter) {
        if (items[static_cast<size_t>(g_state.selected)].activate) {
            items[static_cast<size_t>(g_state.selected)].activate();
        }
    }
}

void ApplyContinuousOptions() {
    const int ped = PlayerPed();
    if (g_state.godMode) NativeVoid(SET_ENTITY_INVINCIBLE, ped, TRUE);
    if (g_state.infiniteAmmo) NativeVoid(SET_PED_INFINITE_AMMO_CLIP, ped, TRUE);
    if (g_state.neverWanted) {
        const int player = Native<int>(PLAYER_ID);
        NativeVoid(SET_PLAYER_WANTED_LEVEL, player, 0, FALSE);
        NativeVoid(SET_PLAYER_WANTED_LEVEL_NOW, player, FALSE);
    }
    if (g_state.vehicleGodMode) {
        const int vehicle = CurrentVehicle();
        if (vehicle != g_state.protectedVehicle) {
            if (g_state.protectedVehicle != 0) {
                NativeVoid(SET_ENTITY_INVINCIBLE, g_state.protectedVehicle, FALSE);
            }
            if (vehicle != 0) NativeVoid(SET_ENTITY_INVINCIBLE, vehicle, TRUE);
            g_state.protectedVehicle = vehicle;
        }
    } else if (g_state.protectedVehicle != 0) {
        NativeVoid(SET_ENTITY_INVINCIBLE, g_state.protectedVehicle, FALSE);
        g_state.protectedVehicle = 0;
    }
}

void ScriptMain() {
    std::srand(GetTickCount());
    const bool loggerReady = InitializeLogger();
    if (loggerReady) Log("INFO", "ScriptHookV started the trainer script.");
    else OutputDebugStringW(L"GTA5 Enhanced Story Trainer: could not create its log file.\n");
    g_state.menuOpen = true;
    SetStatus("Trainer ready. The menu opened automatically; press F10 to close or reopen.");
    Log("INFO", "Menu auto-opened when the trainer script started.");

    DWORD lastHeartbeat = GetTickCount();
    bool hasPreviousSessionState = false;
    bool previousOnlineState = false;
    for (;;) {
        const KeyPresses keys = PollKeys();
        if (keys.f10) {
            g_state.menuOpen = !g_state.menuOpen;
            Log("INFO", g_state.menuOpen ? "F10 detected: menu opened." : "F10 detected: menu closed.");
        }

        const bool online = InOnlineSession();
        if (!hasPreviousSessionState || online != previousOnlineState) {
            Log("INFO", online ? "Game session state: online." : "Game session state: story/offline.");
            hasPreviousSessionState = true;
            previousOnlineState = online;
        }

        const DWORD now = GetTickCount();
        if (now - lastHeartbeat >= 30000) {
            Log("INFO", std::string("Heartbeat: script active; menu=") +
                (g_state.menuOpen ? "open" : "closed") +
                (online ? "; session=online." : "; session=story/offline."));
            lastHeartbeat = now;
        }

        if (online) {
            if (g_state.menuOpen) {
                DrawRect(0.5f, 0.90f, 0.48f, 0.06f, 35, 12, 15, 230);
                DrawText("STORY MODE ONLY - MENU DISABLED ONLINE", 0.29f, 0.883f, 0.32f, 255, 150, 150);
            }
        } else {
            ApplyContinuousOptions();
            if (g_state.menuOpen) {
                const std::vector<MenuItem> items = BuildItems();
                HandleMenuInput(items, keys);
                DrawMenu(items);
            }
        }
        // Poll once per game tick so brief hotkey presses are not missed while the menu is closed.
        g_api.scriptWait(0);
    }
}

}  // namespace

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_module = module;
        DisableThreadLibraryCalls(module);
        if (!g_api.Bind()) {
            OutputDebugStringW(L"GTA5 Enhanced Story Trainer: ScriptHookV API binding failed.\n");
            return FALSE;
        }
        g_api.scriptRegister(module, ScriptMain);
    } else if (reason == DLL_PROCESS_DETACH) {
        if (g_api.scriptUnregister) g_api.scriptUnregister(module);
    }
    return TRUE;
}
