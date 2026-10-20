using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using Rage;
using Rage.Attributes;
using Rage.Native;

[assembly: Plugin("GTA V Enhanced Story Trainer", Description = "Story Mode trainer for GTA V Enhanced", Author = "pedro-santos-dev-alt")]

namespace GTA5EnhancedStoryTrainer
{
    public static class EntryPoint
    {
        private enum Page { Main, Player, Weapons, Vehicles, World }
        private static readonly string[] Pages = { "Player", "Weapons", "Vehicles", "World", "Close menu" };
        private static readonly string[] Vehicles = { "adder", "zentorno", "t20", "kuruma2", "sultan3", "bati", "buzzard2", "hydra", "police2", "ambulance" };
        private static readonly string[] Weapons = { "WEAPON_PISTOL", "WEAPON_COMBATPISTOL", "WEAPON_SMG", "WEAPON_ASSAULTRIFLE", "WEAPON_CARBINERIFLE", "WEAPON_PUMPSHOTGUN", "WEAPON_SNIPERRIFLE", "WEAPON_RPG", "WEAPON_MINIGUN" };
        private static readonly string[] Weather = { "EXTRASUNNY", "CLEAR", "CLOUDS", "OVERCAST", "RAIN", "THUNDER", "FOGGY", "SNOW" };
        private static readonly string[] PlayerItems = { "God mode", "Restore health", "Armour 100", "Clear wanted level", "Never wanted", "Back" };
        private static readonly string[] WeaponItems = { "Weapon: ", "Give full loadout", "Infinite ammo clip", "Remove all weapons", "Back" };
        private static readonly string[] VehicleItems = { "Spawn: ", "Repair and clean", "Maximum upgrades", "Random paint", "Back" };
        private static readonly string[] WorldItems = { "Weather: ", "Set hour: ", "Freeze clock", "Back" };
        private static readonly Random Rng = new Random();
        private static string _logPath;
        private static Page _page;
        private static int _selected, _vehicleIndex, _weaponIndex, _weatherIndex, _hour = 12;
        private static bool _menuOpen = true, _godMode, _neverWanted, _infiniteAmmo, _freezeClock;
        private static bool _f10WasDown, _upWasDown, _downWasDown, _leftWasDown, _rightWasDown, _enterWasDown, _backWasDown;
        private static readonly dynamic N = NativeFunction.Natives;

        public static void Main()
        {
            _logPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "GTA5EnhancedStoryTrainer-RPH.log");
            Log("INFO", "RPH plugin loaded; trainer menu opened automatically. This plugin uses RAGE Plugin Hook, not ScriptHookV.");
            while (true)
            {
                try
                {
                    UpdateKeys();
                    if (!IsOnline())
                    {
                        ApplyContinuousOptions();
                        if (_menuOpen) DrawMenu();
                    }
                    else if (_menuOpen)
                    {
                        DrawText("STORY MODE ONLY - MENU DISABLED ONLINE", 0.29f, 0.90f, 0.32f, 255, 140, 140);
                    }
                    GameFiber.Yield();
                }
                catch (Exception ex)
                {
                    Log("ERROR", ex.ToString());
                    Game.DisplayNotification("~r~Story Trainer error. See GTA5EnhancedStoryTrainer-RPH.log");
                    GameFiber.Sleep(1000);
                }
            }
        }

        private static void UpdateKeys()
        {
            bool f10 = Game.IsKeyDown(Keys.F10), up = Game.IsKeyDown(Keys.Up), down = Game.IsKeyDown(Keys.Down);
            bool left = Game.IsKeyDown(Keys.Left), right = Game.IsKeyDown(Keys.Right), enter = Game.IsKeyDown(Keys.Enter), back = Game.IsKeyDown(Keys.Back);
            if (f10 && !_f10WasDown) { _menuOpen = !_menuOpen; Log("INFO", "F10 toggled menu=" + _menuOpen); }
            if (_menuOpen && !IsOnline())
            {
                string[] items = CurrentItems();
                if (up && !_upWasDown) _selected = (_selected + items.Length - 1) % items.Length;
                if (down && !_downWasDown) _selected = (_selected + 1) % items.Length;
                if (left && !_leftWasDown) Adjust(-1);
                if (right && !_rightWasDown) Adjust(1);
                if (enter && !_enterWasDown) Activate();
                if (back && !_backWasDown) Back();
            }
            _f10WasDown = f10; _upWasDown = up; _downWasDown = down; _leftWasDown = left;
            _rightWasDown = right; _enterWasDown = enter; _backWasDown = back;
        }

        private static string[] CurrentItems()
        {
            switch (_page)
            {
                case Page.Player: return PlayerItems;
                case Page.Weapons: return WeaponItems;
                case Page.Vehicles: return VehicleItems;
                case Page.World: return WorldItems;
                default: return Pages;
            }
        }

        private static string ItemLabel(int index)
        {
            switch (_page)
            {
                case Page.Weapons: return index == 0 ? WeaponItems[0] + Weapons[_weaponIndex] : WeaponItems[index];
                case Page.Vehicles: return index == 0 ? VehicleItems[0] + Vehicles[_vehicleIndex] : VehicleItems[index];
                case Page.World:
                    if (index == 0) return WorldItems[0] + Weather[_weatherIndex];
                    if (index == 1) return WorldItems[1] + _hour.ToString("00") + ":00";
                    return WorldItems[index];
                case Page.Player:
                    if (index == 0) return "God mode: " + (_godMode ? "ON" : "OFF");
                    if (index == 4) return "Never wanted: " + (_neverWanted ? "ON" : "OFF");
                    return PlayerItems[index];
                default: return Pages[index];
            }
        }

        private static void DrawMenu()
        {
            string[] items = CurrentItems();
            const float x = 0.055f, y = 0.16f, w = 0.30f, row = 0.035f;
            float h = 0.13f + items.Length * row;
            Rect(x + w / 2, y + h / 2, w, h, 12, 17, 27, 230);
            Rect(x + w / 2, y + 0.025f, w, 0.05f, 32, 105, 184, 245);
            DrawText("GTA V ENHANCED | STORY MODE", x + 0.012f, y + 0.009f, 0.30f);
            DrawText(_page.ToString().ToUpperInvariant(), x + 0.012f, y + 0.060f, 0.29f, 160, 210, 255);
            for (int i = 0; i < items.Length; i++)
            {
                float iy = y + 0.098f + i * row;
                if (i == _selected) Rect(x + w / 2, iy + 0.012f, w - 0.012f, row - 0.002f, 58, 87, 121, 235);
                DrawText(ItemLabel(i), x + 0.012f, iy, 0.28f);
            }
            DrawText("F10 close | Arrows move/change | Enter select | Backspace back", x + 0.012f, y + h - 0.027f, 0.18f, 190, 195, 205);
        }

        private static void Activate()
        {
            switch (_page)
            {
                case Page.Main:
                    if (_selected == 4) { _menuOpen = false; return; }
                    _page = (Page)(_selected + 1); _selected = 0; return;
                case Page.Player:
                    switch (_selected)
                    {
                        case 0: _godMode = !_godMode; Call("x3882114BDE571AD4", Ped(), _godMode); break;
                        case 1: Call("x6B76DC1F3AE6E6A3", Ped(), Int("x15D757606D170C3C", Ped()), 0); break;
                        case 2: Call("xCEA04D83135264CC", Ped(), 100); break;
                        case 3: Call("x39FF19C64EF7DA5B", PlayerId(), 0, false); Call("xE0A7D1E497FFCD6F", PlayerId(), false); break;
                        case 4: _neverWanted = !_neverWanted; Call("xAA5F02DB48D704B9", _neverWanted ? 0 : 5); break;
                        default: Back(); return;
                    }
                    break;
                case Page.Weapons:
                    switch (_selected)
                    {
                        case 0: GiveWeapon(Weapons[_weaponIndex]); break;
                        case 1: foreach (string weapon in Weapons) GiveWeapon(weapon); break;
                        case 2: _infiniteAmmo = !_infiniteAmmo; Call("x183DADC6AA953186", Ped(), _infiniteAmmo); break;
                        case 3: Call("xF25DF915FA38C5F3", Ped(), true); break;
                        default: Back(); return;
                    }
                    break;
                case Page.Vehicles:
                    switch (_selected)
                    {
                        case 0: SpawnVehicle(Vehicles[_vehicleIndex]); break;
                        case 1: WithVehicle(v => { Call("x115722B1B9C14C1C", v); Call("x79D3B596FE44EE8B", v, 0.0f); }); break;
                        case 2: WithVehicle(MaxMods); break;
                        case 3: WithVehicle(v => { Call("x7141766F91D15BEA", v, Rng.Next(256), Rng.Next(256), Rng.Next(256)); }); break;
                        default: Back(); return;
                    }
                    break;
                case Page.World:
                    if (_selected == 0) Call("xED712CA327900C8A", Weather[_weatherIndex]);
                    else if (_selected == 1) Call("x47C3B5848C3E45D8", _hour, 0, 0);
                    else if (_selected == 2) { _freezeClock = !_freezeClock; Call("xB02D61207FD9CB7D", _freezeClock, 12, 0); }
                    else { Back(); return; }
                    break;
            }
            string result = ItemLabel(_selected);
            Log("INFO", "Action executed on " + _page + ", item=" + _selected + " (" + result + ")");
            Game.DisplayNotification("~b~Story Trainer: ~s~" + result);
        }

        private static void Adjust(int d)
        {
            if (_selected != 0 && !(_page == Page.World && _selected == 1)) return;
            if (_page == Page.Weapons) _weaponIndex = (_weaponIndex + d + Weapons.Length) % Weapons.Length;
            else if (_page == Page.Vehicles) _vehicleIndex = (_vehicleIndex + d + Vehicles.Length) % Vehicles.Length;
            else if (_page == Page.World && _selected == 0) _weatherIndex = (_weatherIndex + d + Weather.Length) % Weather.Length;
            else if (_page == Page.World && _selected == 1) _hour = (_hour + d + 24) % 24;
        }

        private static void Back() { if (_page == Page.Main) _menuOpen = false; else { _page = Page.Main; _selected = 0; } }
        private static int Ped() { return Int("xD80958FC74E988A6"); }
        private static int PlayerId() { return Int("x4F8644AF03D0E0D6"); }
        private static int CurrentVehicle() { return Int("x9A9112A0FE9A4713", Ped(), false); }
        private static bool IsOnline() { return Bool("xD83C2B94E7508980"); }
        private static void GiveWeapon(string name) { uint hash = U("xD24D37CC275948CC", name); Call("xBF0FD6E56C964FCB", Ped(), hash, 9999, false, true); }

        private static void SpawnVehicle(string modelName)
        {
            uint model = U("xD24D37CC275948CC", modelName);
            if (!Bool("x35B9E0803292B641", model) || !Bool("x19AAC8F07BFEC53E", model)) { Notify("Invalid vehicle model"); return; }
            Call("x963D27A58DF860AC", model);
            int attempts = 0;
            while (!Bool("x98A4EB5D89A0C952", model) && attempts++ < 500) GameFiber.Yield();
            if (!Bool("x98A4EB5D89A0C952", model)) { Notify("Vehicle model timed out"); return; }
            int ped = Ped();
            dynamic coords = N.x3FEF770D40960D5A(ped, true);
            float heading = F("xE83D4F9BA2A38914", ped) * 0.01745329252f;
            float px = Convert.ToSingle(coords.X) + (float)Math.Sin(heading) * 5f;
            float py = Convert.ToSingle(coords.Y) + (float)Math.Cos(heading) * 5f;
            float pz = Convert.ToSingle(coords.Z) + 0.5f;
            int vehicle = Int("xAF35D0D2583051B0", model, px, py, pz, F("xE83D4F9BA2A38914", ped), false, false, false);
            if (vehicle != 0) { Call("x49733E92263139D1", vehicle); Call("xF75B0D629E1C063D", ped, vehicle, -1); Call("x1F2AA07F00B3217A", vehicle, 0); }
            Call("xE532F5D78798DAAB", model);
        }

        private static void MaxMods(int v)
        {
            Call("x1F2AA07F00B3217A", v, 0);
            for (int slot = 0; slot < 50; slot++) { int count = Int("xE38E9162A2500646", v, slot); if (count > 0) Call("x6AF0636DDEDCB6DD", v, slot, count - 1, false); }
            foreach (int slot in new[] { 17, 18, 19, 20, 21, 22 }) Call("x2A1F4F37F95BAD08", v, slot, true);
        }
        private static void WithVehicle(Action<int> action) { int v = CurrentVehicle(); if (v == 0) { Notify("Enter a vehicle first"); return; } action(v); }
        private static void ApplyContinuousOptions() { int ped = Ped(); if (_godMode) Call("x3882114BDE571AD4", ped, true); if (_infiniteAmmo) Call("x183DADC6AA953186", ped, true); if (_neverWanted) { Call("x39FF19C64EF7DA5B", PlayerId(), 0, false); Call("xE0A7D1E497FFCD6F", PlayerId(), false); } }

        private static object Call(string h, params object[] a)
        {
            // RPH's dynamic NativeFunction API resolves these generated members by GTA native hash.
            switch (h)
            {
                case "x3882114BDE571AD4": return N.x3882114BDE571AD4(a[0], a[1]);
                case "x6B76DC1F3AE6E6A3": return N.x6B76DC1F3AE6E6A3(a[0], a[1], a[2]);
                case "x15D757606D170C3C": return N.x15D757606D170C3C(a[0]);
                case "xCEA04D83135264CC": return N.xCEA04D83135264CC(a[0], a[1]);
                case "x39FF19C64EF7DA5B": return N.x39FF19C64EF7DA5B(a[0], a[1], a[2]);
                case "xE0A7D1E497FFCD6F": return N.xE0A7D1E497FFCD6F(a[0], a[1]);
                case "xAA5F02DB48D704B9": return N.xAA5F02DB48D704B9(a[0]);
                case "xF25DF915FA38C5F3": return N.xF25DF915FA38C5F3(a[0], a[1]);
                case "x183DADC6AA953186": return N.x183DADC6AA953186(a[0], a[1]);
                case "xD80958FC74E988A6": return N.xD80958FC74E988A6();
                case "x4F8644AF03D0E0D6": return N.x4F8644AF03D0E0D6();
                case "x9A9112A0FE9A4713": return N.x9A9112A0FE9A4713(a[0], a[1]);
                case "xD83C2B94E7508980": return N.xD83C2B94E7508980();
                case "xD24D37CC275948CC": return N.xD24D37CC275948CC(a[0]);
                case "xBF0FD6E56C964FCB": return N.xBF0FD6E56C964FCB(a[0], a[1], a[2], a[3], a[4]);
                case "x35B9E0803292B641": return N.x35B9E0803292B641(a[0]);
                case "x19AAC8F07BFEC53E": return N.x19AAC8F07BFEC53E(a[0]);
                case "x963D27A58DF860AC": return N.x963D27A58DF860AC(a[0]);
                case "x98A4EB5D89A0C952": return N.x98A4EB5D89A0C952(a[0]);
                case "x3FEF770D40960D5A": return N.x3FEF770D40960D5A(a[0], a[1]);
                case "xE83D4F9BA2A38914": return N.xE83D4F9BA2A38914(a[0]);
                case "xAF35D0D2583051B0": return N.xAF35D0D2583051B0(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
                case "x49733E92263139D1": return N.x49733E92263139D1(a[0]);
                case "xF75B0D629E1C063D": return N.xF75B0D629E1C063D(a[0], a[1], a[2]);
                case "x1F2AA07F00B3217A": return N.x1F2AA07F00B3217A(a[0], a[1]);
                case "xE532F5D78798DAAB": return N.xE532F5D78798DAAB(a[0]);
                case "xE38E9162A2500646": return N.xE38E9162A2500646(a[0], a[1]);
                case "x6AF0636DDEDCB6DD": return N.x6AF0636DDEDCB6DD(a[0], a[1], a[2], a[3]);
                case "x2A1F4F37F95BAD08": return N.x2A1F4F37F95BAD08(a[0], a[1], a[2]);
                case "x115722B1B9C14C1C": return N.x115722B1B9C14C1C(a[0]);
                case "x79D3B596FE44EE8B": return N.x79D3B596FE44EE8B(a[0], a[1]);
                case "x7141766F91D15BEA": return N.x7141766F91D15BEA(a[0], a[1], a[2], a[3]);
                case "xED712CA327900C8A": return N.xED712CA327900C8A(a[0]);
                case "x47C3B5848C3E45D8": return N.x47C3B5848C3E45D8(a[0], a[1], a[2]);
                case "xB02D61207FD9CB7D": return N.xB02D61207FD9CB7D(a[0], a[1], a[2]);
                case "x3A618A217E5154F0": return N.x3A618A217E5154F0(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
                case "x66E0276CC5F6B9DA": return N.x66E0276CC5F6B9DA(a[0]);
                case "x07C837F9A01C34C9": return N.x07C837F9A01C34C9(a[0], a[1]);
                case "xBE6B23FFA53FB442": return N.xBE6B23FFA53FB442(a[0], a[1], a[2], a[3]);
                case "x2513DFB0FB8400FE": return N.x2513DFB0FB8400FE();
                case "x25FBB336DF1804CB": return N.x25FBB336DF1804CB(a[0]);
                case "x6C188BE134E074AA": return N.x6C188BE134E074AA(a[0]);
                case "xCD015E5BB0D96A57": return N.xCD015E5BB0D96A57(a[0], a[1]);
                default: throw new NotSupportedException("Unknown game native: " + h);
            }
        }
        private static int Int(string h, params object[] a) { return Convert.ToInt32(Call(h, a)); }
        private static uint U(string h, params object[] a) { return unchecked((uint)Convert.ToInt32(Call(h, a))); }
        private static float F(string h, params object[] a) { return Convert.ToSingle(Call(h, a)); }
        private static bool Bool(string h, params object[] a) { return Convert.ToBoolean(Call(h, a)); }

        private static void Rect(float x, float y, float w, float h, int r, int g, int b, int a) { Call("x3A618A217E5154F0", x, y, w, h, r, g, b, a); }
        private static void DrawText(string s, float x, float y, float scale, int r = 238, int g = 242, int b = 248, int a = 255)
        {
            Call("x66E0276CC5F6B9DA", 0); Call("x07C837F9A01C34C9", 0.0f, scale); Call("xBE6B23FFA53FB442", r, g, b, a); Call("x2513DFB0FB8400FE"); Call("x25FBB336DF1804CB", "STRING"); Call("x6C188BE134E074AA", s); Call("xCD015E5BB0D96A57", x, y);
        }
        private static void Notify(string s) { Game.DisplayNotification("~b~Story Trainer: ~s~" + s); Log("INFO", s); }
        private static void Log(string level, string message) { try { File.AppendAllText(_logPath ?? Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "GTA5EnhancedStoryTrainer-RPH.log"), DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") + " [" + level + "] " + message + Environment.NewLine, Encoding.UTF8); } catch { } }
    }
}
