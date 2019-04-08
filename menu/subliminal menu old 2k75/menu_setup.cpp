#include "cmenu.hpp"
#include "..\cheats\misc\logs.h"
#include <cctype>
#include <shlobj.h>
#include <shlwapi.h>
#include <algorithm>
#define NOMINMAX
#include <Windows.h>
#include <chrono>
struct hud_weapons_t
{
	std::int32_t* get_weapon_count()
	{
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};
template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(util::FindSignature("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);
	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(util::FindSignature("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}
void KnifeApplyCallbk()
{
	static auto clear_hud_weapon_icon_fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(util::FindSignature("client_panorama.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C"));
	auto element = FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");
	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);
	if (hud_weapons == nullptr) return;
	if (!*hud_weapons->get_weapon_count()) return;
	for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++) i = clear_hud_weapon_icon_fn(hud_weapons, i);
	typedef void(*ForceUpdate) (void);
}
std::string get_config_dir() {
	std::string folder;
	static TCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path))) {
		folder = std::string(path) + "\\sectumgang\\";
	}
	CreateDirectory(folder.c_str(), NULL);
	return folder;
}
#define BUILDSTAMP ( __DATE__ )
void load_config() {
	if (!cfg_manager->files.size()) { return; }
	eventlogs::get().add("Loaded " + cfg_manager->files[g_cfg.selected_config], Color::White);
	cfg_manager->load(cfg_manager->files[g_cfg.selected_config]);
}
void save_config() {
	if (!cfg_manager->files.size()) { return; }
	eventlogs::get().add("Saved " + cfg_manager->files[g_cfg.selected_config], Color::White);
	cfg_manager->save(cfg_manager->files[g_cfg.selected_config]);
	cfg_manager->config_files();
}
void remove_config() {
	if (!cfg_manager->files.size()) { return; }
	cfg_manager->remove(cfg_manager->files[g_cfg.selected_config]);
	cfg_manager->config_files();
	if (g_cfg.selected_config > cfg_manager->files.size() - 1) {
		g_cfg.selected_config = cfg_manager->files.size() - 1;
	}
}
void add_config() {
	if (g_cfg.new_config_name.find(".cc") == -1) g_cfg.new_config_name += ".cc";
	cfg_manager->save(g_cfg.new_config_name.c_str());
	cfg_manager->config_files();
	g_cfg.new_config_name = "";
	g_cfg.selected_config = cfg_manager->files.size() - 1;
}
void unload_cheat() {
	unload = true;
}
bool Contains(const std::string &word, const std::string &sentence) {
	if (word == "" || sentence == "") return true;
	return sentence.find(word) != std::string::npos;
}
std::string ToLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int))std::tolower);
	return str;
}
void setup_main_menu() {
	static auto set_sub = [](int sub) -> void {
		g_cfg.menu.group_sub = sub;
	};
	static auto set_tabsub = [](int sub) -> void {
		g_cfg.menu.tab_sub = sub;
	};
	/*auto legit_tab = new C_Tab(&menu::get(), 0, "e", "Legit"); {
		auto legit_aimbot = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			auto aim_assist = new C_CheckBox("Aim Assist", &g_cfg.legit.aimassist);
			auto aim_assist_key = new C_KeyBind("", &g_cfg.legit.aim_assist_bind);
			auto fov = new C_SliderFloat("Field Of View", &g_cfg.legit.fov, 0, 180, "°");
			auto smoothing = new C_SliderFloat("Smoothing", &g_cfg.legit.smoothing, 0, 15, "°");
		}
		auto legit_backtrack = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto backtracking = new C_CheckBox("Backtracking Exploit", &g_cfg.legit.backtracking);
			auto max_time = new C_SliderFloat("Maximum Backtracking Time", &g_cfg.legit.backtracking_max_time, 0.00f, 0.20f, " s");
			auto visualize = new C_CheckBox("Visualize Backtracking", &g_cfg.legit.backtracking_visualize,
				{ new C_ColorSelector(&g_cfg.legit.backtracking_visualize_color) });
		}
	}*/
	auto rage_tab = new C_Tab(&menu::get(), 0, "A", "Ragebot", {

		//C_Tab::subtab_t("Main", 0, fonts[TABFONT]),
		//C_Tab::subtab_t("Pistol", 1, fonts[TABFONT]),
		//C_Tab::subtab_t("Deagle", 2, fonts[TABFONT]),
		//C_Tab::subtab_t("Rifles", 3, fonts[TABFONT]),
		//C_Tab::subtab_t("Smgs", 4, fonts[TABFONT]),
		//C_Tab::subtab_t("Snipers", 5, fonts[TABFONT]),
		//C_Tab::subtab_t("Autos", 6, fonts[TABFONT]),
		//C_Tab::subtab_t("Shotguns", 7, fonts[TABFONT])


		C_Tab::subtab_t("G", 0, fonts[AIMTABICON]),
		C_Tab::subtab_t("E", 1, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("A", 2, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("W", 3, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("O", 4, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("a", 5, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("Y", 6, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("Z", 7, fonts[SUBTABWEAPONS])

		}); {
		set_tabsub(0);
		auto rage = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto enable = new C_CheckBox("Enable", &g_cfg.ragebot.enable);
			auto fov = new C_SliderInt("Field Of View", &g_cfg.ragebot.field_of_view, 0, 360, "°");
			auto selection_type = new C_Dropdown("Selection Type", &g_cfg.ragebot.selection_type, { "Field Of View", "Lowest Health", "Closest Distance" });
			auto silent_aim = new C_CheckBox("Silent Aim", &g_cfg.ragebot.silent_aim);
			auto hsonly = new C_CheckBox("Headshot Only", &g_cfg.ragebot.hsonly);
			auto anti_recoil = new C_CheckBox("Remove Recoil", &g_cfg.ragebot.anti_recoil);
			auto anti_spread = new C_CheckBox("Remove Spread", &g_cfg.ragebot.anti_spread);
			auto zeus_bot = new C_CheckBox("Auto Zeus", &g_cfg.ragebot.zeus_bot);
			auto auto_revolver = new C_CheckBox("Auto Revolver", &g_cfg.ragebot.autorevolver);
			auto autoscope = new C_CheckBox("Auto Scope", &g_cfg.ragebot.autoscope);
			auto extrapolation = new C_CheckBox("Extrapolation", &g_cfg.ragebot.extrapolation);
			auto aim_assist = new C_CheckBox("Aim Assist", &g_cfg.legit.aimassist);
			auto aim_assist_key = new C_KeyBind("", &g_cfg.legit.aim_assist_bind);
			//auto fov = new C_SliderFloat("Fov", &g_cfg.legit.fov, 0, 180, "°");
			auto smoothing = new C_SliderFloat("Smoothing", &g_cfg.legit.smoothing, 0, 15, "°");
		}
		auto rage2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			new C_CheckBox("Resolver", &g_cfg.ragebot.resolver_experimental);
			new C_Dropdown("Resolver Mode", &g_cfg.ragebot.resolver_experimentalmode, { "Calculation", "Animation Fix" });
			//auto override_key = new C_KeyBind("Override Resolver Key", &g_cfg.ragebot.override_key);
			//new C_CheckBox("use alternative Resolver", &g_cfg.ragebot.usedesyncresolvemdoe);
			//new C_SliderInt("Bruteforce after x Shots", &g_cfg.ragebot.bruteshots, 0, 10, " shots");

			auto aimstep = new C_CheckBox("Aim Step", &g_cfg.ragebot.aimstep);
			auto lagcomp = new C_CheckBox("Lag Compensation", &g_cfg.ragebot.lagcomp);
			//auto lagcomp_time = new C_SliderFloat("", &g_cfg.ragebot.lagcomp_time, 0, 0.20, "s"); useless
			auto lag_compensation_type = new C_Dropdown("Compensation Type", &g_cfg.ragebot.lag_compensation_type, { "Refine Shot", "Prediction" });
			new C_CheckBox("Falsch Ducker", &g_cfg.antiaim.fakecourouch);
			//i smashed my keyboard while typing the bool fakekrautsh
			new C_CheckBox("Fake Duck", &g_cfg.antiaim.fakekrautsh);
			auto backtracking = new C_CheckBox("Backtracking Exploit", &g_cfg.legit.backtracking);
			auto max_time = new C_SliderFloat("Maximum Backtracking Time", &g_cfg.legit.backtracking_max_time, 0.00f, 0.20f, " s");
			auto visualize = new C_CheckBox("Visualize Backtracking", &g_cfg.legit.backtracking_visualize,
				{ new C_ColorSelector(&g_cfg.legit.backtracking_visualize_color) });
		}
		for (int i = 0; i < 7; i++) {
			set_tabsub(i + 1);
			auto weapon = new C_GroupBox(GROUP_LEFT, 8, "", 2); {
				for (int s = 0; s < 2; s++) {
					set_sub(s);
					if (s == 0)
					{
						auto autoshoot = new C_CheckBox("Auto Fire", &g_cfg.ragebot.autoshoot[i]);
						auto autowall = new C_CheckBox("Auto Wall", &g_cfg.ragebot.autowall[i]);
						auto minimum_damage = new C_SliderInt("", &g_cfg.ragebot.minimum_damage[i], 1, 100, "");
						auto hitchance = new C_CheckBox("Hitchance", &g_cfg.ragebot.hitchance[i]);
						auto hitchance_amount = new C_SliderInt("", &g_cfg.ragebot.hitchance_amount[i], 0, 100, "%");
						auto hitscan = new C_MultiDropdown("Hitboxes", &g_cfg.ragebot.hitscan[i]);
						auto hitscan_history = new C_MultiDropdown("Hitbox History", &g_cfg.ragebot.hitscan_history[i]);
						auto multipoint = new C_MultiDropdown("Multipoint", &g_cfg.ragebot.multipoint_hitboxes[i]);
					}
					else if (s == 1)
					{
						new C_SliderFloat("Head", &g_cfg.ragebot.pointscalehead[i], 0, 0.3, "");
						new C_SliderFloat("Neck", &g_cfg.ragebot.pointscaleneck[i], 0, 0.3, "");
						new C_SliderFloat("Chest", &g_cfg.ragebot.pointscalechest[i], 0, 0.3, "");
						new C_SliderFloat("Stomach", &g_cfg.ragebot.pointscalestomach[i], 0, 0.3, "");
						new C_SliderFloat("Pelvis", &g_cfg.ragebot.pointscalepelvis[i], 0, 0.3, "");
						new C_SliderFloat("Arms", &g_cfg.ragebot.pointscalearms[i], 0, 0.3, "");
						new C_SliderFloat("Legs", &g_cfg.ragebot.pointscalelegs[i], 0, 0.3, "");
						new C_SliderFloat("Other", &g_cfg.ragebot.pointscale[i], 0, 0.3, "");
					}
				}
			}
			auto weapon2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
				new C_MultiDropdown("Body Aim", &g_cfg.ragebot.baim_settings[i]);
				new C_CheckBox("Quick Stop", &g_cfg.ragebot.quickstop[i]);
				new C_CheckBox("Auto Stop", &g_cfg.ragebot.autostopmovement[i]);
				new C_CheckBox("Auto Fakewalk", &g_cfg.ragebot.autofakewalk[i]);
				new C_CheckBox("P Walk", &g_cfg.fastwalk.enabled);
				new C_KeyBind("P Walk Key", &g_cfg.fastwalk.key);
				new C_Dropdown("P Walk Mode", &g_cfg.fastwalk.speedmode, { "Normal Speed", "Dynamic Speed" , "Single Speed" });
				if (g_cfg.fastwalk.speedmode == 2)
					continue;
				new C_SliderInt("P Walk Speed", &g_cfg.fastwalk.speed, 20, 80, " units");
			}
		}
	}
	auto antiaim_tab = new C_Tab(&menu::get(), 1, "B", "Anti-Aims"); {
		auto antiaim = new C_GroupBox(GROUP_LEFT, 8, "", 3); {
			char * type[4] = { "Standing", "Moving", "In Air", "Edge" };
			for (int i = 0; i < 3; i++) {
				set_sub(i);
				auto antiaim_type = new C_TextSeperator(type[i], SEPERATOR_BOLD);
				auto base_angle = new C_Dropdown("Base Angle", &g_cfg.antiaim.base_angle[i], { "Static", "Crosshair", "At Targets" });
				std::vector< std::string > pitchx{ "Off", "Down", "Up", "Minimal" };
				auto pitch = new C_Dropdown("Pitch", &g_cfg.antiaim.pitch[i], pitchx);
				std::vector< std::string > reals{ "Off", "Sideways", "Backwards", "Rotate", "Jitter", "Local View", "Lowerbody", "Auto Direction", "180Z" , "LBY + Delta" };
				auto yaw = new C_Dropdown("Real Yaw", &g_cfg.antiaim.yaw[i], reals);
				auto yaw_offset = new C_SliderInt("", &g_cfg.antiaim.yaw_offset[i], -180.f, 180.f, "°");
				std::vector< std::string > fakes{ "Off", "Static", "Jitter", "Spin", "Shuffle", "Jitter between MaxDelta", "180 名" };
				auto fakeyaw = new C_Dropdown("Desync Yaw", &g_cfg.antiaim.fake_yaw[i], fakes);
				auto fakeyaw_offset = new C_SliderInt("", &g_cfg.antiaim.fake_yaw_offset[i], -59.f, 59.f, "°");
				if (i != 0) continue;
				auto lby_breaker = new C_CheckBox("LowerBodyYaw Breaker", &g_cfg.antiaim.lby_breaker);
				auto lby_delta = new C_SliderInt("", &g_cfg.antiaim.lby_delta, -180.f, 180.f, "°");
				auto flick_up = new C_CheckBox("Flick Up", &g_cfg.antiaim.pitch_flick);
				auto pitchchange = new C_CheckBox("Change Pitch On Land", &g_cfg.antiaim.hittinggroundpitch);
				auto balance_breaker = new C_CheckBox("Balance Breaker", &g_cfg.antiaim.break_adjustment);
				auto animation_fix = new C_CheckBox("Animation Fix", &g_cfg.antiaim.animfix);
				new C_CheckBox("P2C Breaker", &g_cfg.antiaim.p2cbreaker);
				new C_CheckBox("Break 979", &g_cfg.antiaim.ninesevenlbybreaker);
			}
		}
		auto antiaim_2 = new C_GroupBox(GROUP_RIGHT, 8, "", 3); {
			char * type2[3] = { "Manual", "Fakelag", "Other" };
			for (int i = 0; i < 3; i++) {
				set_sub(i);
				auto antiaim_page2 = new C_TextSeperator(type2[i], SEPERATOR_BOLD);
				if (i == 0)
				{
					auto manualkey_left = new C_KeyBind("Manual Key Left", &g_cfg.antiaim.left_manualkey);
					auto manualkey_right = new C_KeyBind("Manual Key Right", &g_cfg.antiaim.right_manualkey);
					auto manualkey_back = new C_KeyBind("Manual key Back", &g_cfg.antiaim.back_manualkey);
					auto manualaafakes = new C_Dropdown("Manual AntiAim Fakes", &g_cfg.antiaim.manualaafakes, { "Jitter around Reals", "Jitter between max delta", "Spin around Reals" });
				}
				else if (i == 1)
				{
					new C_CheckBox("FakeLag Chams", &g_cfg.antiaim.fakelagchams);
					new C_CheckBox("Auto Fakelag", &g_cfg.antiaim.auto_fakelag);
					new C_TextSeperator("", SEPERATOR_NORMAL);
					new C_SliderInt("", &g_cfg.antiaim.fakelag_amountstanding, 1, 14, " Stand");
					new C_SliderInt("", &g_cfg.antiaim.fakelag_amountmoving, 1, 14, " Move");
					new C_SliderInt("", &g_cfg.antiaim.fakelag_amountinair, 1, 14, " Air");
					new C_CheckBox("Break Lagcomp", &g_cfg.antiaim.breaklagcomp);
					new C_CheckBox("Fakelag Spike On Shoot", &g_cfg.antiaim.fl_spike_on_shoot);
					new C_CheckBox("Fakelag Spike On Peek Of Jump", &g_cfg.antiaim.fl_spike_on_jump);
					new C_CheckBox("Fakelag Spike On Peek", &g_cfg.antiaim.fl_spike_on_peek);
					new C_KeyBind("Fakelag On Key", &g_cfg.antiaim.fakelagonkey);
				}
				else if (i == 2)
				{
					new C_TextSeperator("Freestand:", SEPERATOR_NORMAL);
					new C_CheckBox("Use 2nd Freestand Method", &g_cfg.antiaim.use2ndfreestanding);
					new C_CheckBox("Jitter Between Second Best", &g_cfg.antiaim.jitterbetweensecondbestangle);
					new C_SliderInt("Delay Between Changes", &g_cfg.antiaim.freestanddelay, 1, 400, "ms");
					new C_SliderInt("Jitter Range", &g_cfg.antiaim.freestandjitterrange, 1.f, 180.f, "°");
					new C_CheckBox("Freestanding On Dormants", &g_cfg.antiaim.freestanddormant);
					new C_CheckBox("Panic Jitter", &g_cfg.antiaim.panicjitter);
					new C_Dropdown("Panic Jitter Mode", &g_cfg.antiaim.panicjittermode, { "Away From Desync", "Away From LBY", "Total Panic" });
					new C_SliderInt("Panic Jitter Range", &g_cfg.antiaim.panicjitterrange, 0, 180, "°");
					new C_SliderInt("Force Panic if HP < x", &g_cfg.antiaim.panicjitterhp, 1, 100, "hp");
					new C_TextSeperator("Desync:", SEPERATOR_NORMAL);
					new C_SliderInt("Jitter Range (Shuffle & Jitter)", &g_cfg.antiaim.desyncjitterrange, 10, 59, "°");
				}
			}
		}
	}
	auto esp_tab = new C_Tab(&menu::get(), 2, "D", "Visuals", {
		C_Tab::subtab_t("Player", 0, fonts[TABFONT]),
		C_Tab::subtab_t("ESP", 1, fonts[TABFONT]),
		C_Tab::subtab_t("Other", 2, fonts[TABFONT])
		}); {
		set_tabsub(0);
		auto player = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto enable = new C_CheckBox("Enable", &g_cfg.player.enable);
			auto alpha = new C_SliderInt("Alpha", &g_cfg.player.alpha, 0, 255, "");
			auto pov_arrow = new C_CheckBox("Offscreen Esp", &g_cfg.player.out_pov_arrow,
				{ new C_ColorSelector(&g_cfg.player.out_pov_arrow_color) });
			auto pov_distance = new C_SliderInt("", &g_cfg.player.out_pov_distance, 25, 400, "");
			auto box = new C_CheckBox("Bounding Box", &g_cfg.player.box,
				{ new C_ColorSelector(&g_cfg.player.box_color) });
			auto health = new C_CheckBox("Health", &g_cfg.player.health);
			auto name = new C_CheckBox("Name", &g_cfg.player.name,
				{ new C_ColorSelector(&g_cfg.player.name_color) });
			auto weaponselect = new C_Dropdown("Weapon", &g_cfg.player.weapon, { "Text", "Icon" });
			auto weapon_сolor = new C_TextSeperator("Weapon Color Bar", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.weapon_color) });
			auto ammo = new C_Dropdown("Ammo", &g_cfg.player.ammo, { "Off", "Bar" });
			auto ammobar_color = new C_TextSeperator("Ammo Color Bar", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.ammobar_color) });
			auto lby_bar = new C_CheckBox("LBY Bar", &g_cfg.player.lby,
				{ new C_ColorSelector(&g_cfg.player.lbybar_color) });
			auto flags = new C_MultiDropdown("Flags", &g_cfg.player.flags);
			auto skeleton = new C_MultiDropdown("Skeleton", &g_cfg.player.skeleton);
		}
		auto player_2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			auto glow = new C_MultiDropdown("Glow", &g_cfg.player.glow);
			auto glow_color_enemy = new C_TextSeperator("Enemy Color", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.glow_color_enemy) });
			auto glow_color_teammate = new C_TextSeperator("Teammate Color", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.glow_color_teammate) });
			auto glow_color_local = new C_TextSeperator("Local Color", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.glow_color_local) });
			auto glow_opacity = new C_SliderInt("", &g_cfg.player.glowopacity, 1, 100, "%%");
			auto glow_bloom = new C_SliderInt("", &g_cfg.player.glowbloom, 1, 100, "%%");
			auto glow_type = new C_Dropdown("", &g_cfg.player.glow_type, { "Outline Outer", "Cover", "Outline Inner" });
			auto chams = new C_MultiDropdown("Chams", &g_cfg.player.chams);
			auto chams_color_visible = new C_TextSeperator("Visible Color", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.chams_color) });
			auto chams_color_invisible = new C_TextSeperator("Invisible Color", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.xqz_color) });
			auto chams_opacity = new C_SliderInt("", &g_cfg.player.chams_opacity, 0, 100, "%%");
			auto chams_type = new C_Dropdown("", &g_cfg.player.chams_type, { "Regular", "Flat", "Rubber", "Metallic" });
			auto backtrack_chams = new C_CheckBox("Backtrack Model", &g_cfg.player.backtrack_chams,
				{ new C_ColorSelector(&g_cfg.player.backtrack_chams_color) });
			auto backtrack_chams_opacity = new C_SliderInt("", &g_cfg.player.backtrack_chams_opacity, 0, 100, "%%");
		}
		set_tabsub(1);
		auto esp = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto fov = new C_SliderInt("Override Fov", &g_cfg.esp.fov, 0, 89, "°");
			auto override_fov_scoped = new C_CheckBox("Override Fov When Scoped", &g_cfg.esp.override_fov_scoped);
			auto transparency_scoped = new C_CheckBox("Transparency When Scoped", &g_cfg.esp.transparency_scoped);
			auto transparency_scoped_alpha = new C_SliderInt("", &g_cfg.esp.transparency_scoped_alpha, 0, 50, "%%");
			auto spectator_list = new C_CheckBox("Spectator List", &g_cfg.esp.spectator_list);
			auto espbomba = new C_CheckBox("Esp Bomb", &g_cfg.esp.espbomb);
			auto show_spread = new C_CheckBox("Draw Spread", &g_cfg.esp.show_spread,
				{ new C_ColorSelector(&g_cfg.esp.show_spread_color) });
			auto show_spread_type = new C_Dropdown("", &g_cfg.esp.show_spread_type, { "Regular", "Rainbow", "Seed" });
			auto penetration_reticle = new C_CheckBox("Penetration Reticle", &g_cfg.esp.penetration_reticle);
			auto antiaim_indicator = new C_CheckBox("AntiAim Indicator", &g_cfg.esp.antiaim_indicator);
			new C_CheckBox("Indicators", &g_cfg.esp.indicators);
			new C_MultiDropdown("Indicators", &g_cfg.esp.indicatorsmulti);
			auto ghost_chams = new C_CheckBox("Ghost Chams", &g_cfg.esp.ghost_chams,
				{ new C_ColorSelector(&g_cfg.esp.ghost_chams_color) });
			auto local_chams = new C_CheckBox("Local Chams", &g_cfg.esp.local_chams,
				{ new C_ColorSelector(&g_cfg.esp.local_chams_color) });
			auto bullet_tracer = new C_CheckBox("Bullet Tracers", &g_cfg.esp.bullet_tracer,
				{ new C_ColorSelector(&g_cfg.esp.bullet_tracer_color) });
			auto bullet_tracer_enemy = new C_CheckBox("Enemy Bullet Tracers", &g_cfg.esp.bullet_tracer_enemy,
				{ new C_ColorSelector(&g_cfg.esp.bullet_tracer_color_enemy) });
			auto bullet_tracerlifespan = new C_SliderInt("Tracer Time", &g_cfg.esp.bullet_tracer_duration, 1, 10, "");
			auto bullet_tracersize = new C_SliderInt("Tracer Size", &g_cfg.esp.bullet_tracer_size, 1, 10, "");
		}
		auto esp_2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			auto dropped_weapon = new C_CheckBox("Dropped Weapons", &g_cfg.esp.dropped_weapons,
				{ new C_ColorSelector(&g_cfg.esp.dropped_weapons_color) });
			auto projectiles = new C_CheckBox("Grenades", &g_cfg.esp.projectiles,
				{ new C_ColorSelector(&g_cfg.esp.projectiles_color) });
			auto planted_bomb = new C_CheckBox("Planted Explosive", &g_cfg.esp.planted_bomb,
				{ new C_ColorSelector(&g_cfg.esp.planted_bomb_color) });
			auto bomb_timer = new C_CheckBox("Explosive Timer", &g_cfg.esp.bomb_timer);
			auto dont_render_teammates = new C_CheckBox("Invisible Teammates", &g_cfg.esp.dont_render_teammates);
			auto nightmode = new C_CheckBox("Nightmode", &g_cfg.esp.nightmode);
			auto show_spread_type = new C_Dropdown("Sky Box", &g_cfg.esp.skybox,
				{ "Tibet",
				"Baggage",
				"Monastery",
				"Italy/OldInferno",
				"Aztec",
				"Vertigo",
				"Daylight",
				"Daylight (2)",
				"Clouds",
				"Clouds (2)",
				"Gray",
				"Clear",
				"Canals",
				"Cobblestone",
				"Assault",
				"Clouds (Dark)",
				"Night",
				"Night (2)",
				"Night (Flat)",
				"Dusty",
				"Rainy",
				"Custom: Sunrise",
				"Custom: Galaxy",
				"Custom: Galaxy (2)",
				"Custom: Galaxy (3)",
				"Custom: Clouds (Night)",
				"Custom: Ocean",
				"Custom: Grimm Night",
				"Custom: Heaven",
				"Custom: Heaven (2)",
				"Custom: Clouds",
				"Custom: Night (Blue)" });
			auto hitmarker = new C_MultiDropdown("Hitmarker", &g_cfg.esp.hitmarker);
			auto hitsound = new C_CheckBox("Hitsound", &g_cfg.esp.hitsound);
			auto thirdperson = new C_KeyBind("Thirdperson Key", &g_cfg.misc.thirdperson_toggle);
			auto events_to_log = new C_MultiDropdown("Event Log", &g_cfg.misc.events_to_log);
			auto crosshair = new C_CheckBox("Crosshair", &g_cfg.esp.crosshair);
			auto sniper_crosshair = new C_CheckBox("Sniper Crosshair", &g_cfg.misc.sniper_crosshair);
			new C_CheckBox("Preserve kill feed(unstable)", &g_cfg.esp.bright);
			new C_CheckBox("Full bright", &g_cfg.esp.fullbright);
		}
		set_tabsub(2);
		auto owo = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto removescope = new C_CheckBox("Remove Scope", &g_cfg.esp.removescope, { new C_ColorSelector(&g_cfg.menu.removescopecol[0]) });
			//auto scopecol = new C_TextSeperator("Scope Color", SEPERATOR_NORMAL, { new C_ColorSelector(&g_cfg.menu.removescopecol[0]) });
			auto removescopesense = new C_CheckBox("Remove Scope Sensitivity", &g_cfg.esp.removescopesensitivity);
			auto removezoom = new C_CheckBox("Remove Zoom Effect", &g_cfg.esp.removezoom);
			auto removesmoke = new C_CheckBox("Remove Smokes", &g_cfg.esp.removesmoke);
			auto removeflash = new C_CheckBox("Remove Flashbang Effect", &g_cfg.esp.removeflash);
			auto removerecoil = new C_CheckBox("Remove Visual Recoil", &g_cfg.esp.removerecoil);
		}
		auto owo2 = new C_GroupBox(GROUP_RIGHT, 8, "");
		{
			new C_Dropdown("Hand Chams", &g_cfg.esp.handchams, { "off", "simple", "wireframe", "golden", "glass" },
				{ new C_ColorSelector(&g_cfg.esp.handchamscolor) });
			new C_Dropdown("Gun Chams", &g_cfg.esp.gunchams, { "off", "simple", "wireframe", "golden", "glass", "rainbow" , "test" },
				{ new C_ColorSelector(&g_cfg.esp.gunchamscolor) });
			auto drawhots = new C_CheckBox("Draw Hitboxes Hit", &g_cfg.esp.drawhits);
			auto footesp = new C_CheckBox("Foot Step", &g_cfg.esp.footstep);
		}
	}
	auto misc_tab = new C_Tab(&menu::get(), 3, "G", "Misc"); {
		auto miscellaneous = new C_GroupBox(GROUP_LEFT, 8, "misc"); {
			auto watermark = new C_CheckBox("Watermark", &g_cfg.misc.watermark);
			auto bunnyhop = new C_CheckBox("bhop", &g_cfg.misc.bunnyhop);
			auto airstrafe = new C_CheckBox("Auto Strafe", &g_cfg.misc.airstrafe);
			auto circle_stafer = new C_KeyBind("Circle Strafe", &g_cfg.misc.circle_strafe);
			auto clantag = new C_TextSeperator("ClanTag", TextSeperatorType::SEPERATOR_NORMAL);
			auto clantag_spammer = new C_CheckBox("Rotating ClanTag", &g_cfg.misc.clantag_spammer);
			new C_Dropdown("ClanTag Modes", &g_cfg.misc.clantag_modes, { "sectum.tk", "Debugger", "WeelchairGang", "NN", "Cummies", "Custom Input" });
			new C_TextField("Custom Input", &g_cfg.misc.customclantagyo);
			auto clantag_changerprefix = new C_CheckBox("ClanTag Prefix", &g_cfg.misc.clantagprefix);
			auto clantag_changerprefix2 = new C_Dropdown("Prefix Modes", &g_cfg.misc.clantagprefixmodes, { "先輩", "⚡", "♕" , "♿", "180 名" });
		}
		auto miscellaneous_2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			auto exaggerated_ragdoll = new C_CheckBox("Exaggerated Ragdolls", &g_cfg.misc.exaggerated_ragdolls);
			new C_SliderInt("Exaggerated Ragdolls Multiplier", &g_cfg.misc.exaggerated_ragdolls_multiplier, 2, 100, "x");
			auto ping_spike = new C_CheckBox("Fake Latency", &g_cfg.misc.ping_spike);
			auto ping_spike_value = new C_SliderInt("", &g_cfg.misc.ping_spike_value, -10, 999, "ms");
			auto buybot_separator = new C_TextSeperator("Buy Weapones Bot", SEPERATOR_BOLD);
			auto buybot = new C_CheckBox("Enable", &g_cfg.BuyBot.enable);
			auto buybot_primary = new C_MultiDropdown("Primary", &g_cfg.BuyBot.primary);
			auto buybot_secondary = new C_MultiDropdown("Secondary", &g_cfg.BuyBot.secondary);
			auto buybot_nades = new C_MultiDropdown("Grenades", &g_cfg.BuyBot.nades);
			auto buybot_extras = new C_MultiDropdown("Extras", &g_cfg.BuyBot.extras);

			//ADD CUSTOM COMMAND HERE

			//		auto fakewalk = new C_CheckBox("Fakewalk", &g_cfg.misc.fakewalk);
			//		auto fakewalkmode = new C_Dropdown("Modes", &g_cfg.misc.fakewalkmodes, { "Normal", "Experimental" });
			//		auto fakewalk_key = new C_KeyBind("", &g_cfg.misc.fakewalk_key);
			//		auto fakewalktext = new C_TextSeperator("Experimental Mode Settings", SEPERATOR_NORMAL);
			//		auto fakewalkspeed = new C_SliderInt("Fakewalk Speed", &g_cfg.misc.fakewalkspeed, 1, 10, "");
		}
	}
	auto skinchanger_tab = new C_Tab(&menu::get(), 4, "H", "Skinchanger", {
		C_Tab::subtab_t("W", 0, fonts[WEAPONICONS]),
		C_Tab::subtab_t("4", 1, fonts[WEAPONICONS])
		}); {
		set_tabsub(0);
		auto knifechanger = new C_GroupBox(GROUP_LEFT, 12, "Skin changer");
		{
			auto skinson = new C_CheckBox("Enable Skinchanger", &g_cfg.skins.skinenabled);
			auto awpns = new C_Dropdown("Awp skins", &g_cfg.skins.AWPSkin,
				{ "none",
				"Asiimov",
				"Dragon Lore",
				"Fever Dream",
				"Medusa",
				"HyperBeast",
				"Boom",
				"Lightning Strike",
				"Pink DDpat",
				"Corticera",
				"Redline",
				"Manowar",
				"Graphite",
				"Electric Hive",
				"Sun in Leo",
				"Hyper Beast",
				"Pit viper",
				"Phobos",
				"Elite Build",
				"Worm God",
				"Oni Taiji",
				"Fever Dream" });
			auto scoutns = new C_Dropdown("SSG08 skins", &g_cfg.skins.SSG08Skin,
				{ "none",
				"Lichen Dashed",
				"Dark Water",
				"Blue Spruce",
				"Sand Dune",
				"Palm",
				"Mayan Dreams",
				"Blood in the Water",
				"Tropical Storm",
				"Acid Fade",
				"Slashed",
				"Detour",
				"Abyss",
				"Big Iron",
				"Necropos",
				"Ghost Crusader",
				"Dragonfire" });
			auto scarns = new C_Dropdown("Scar skins", &g_cfg.skins.SCAR20Skin,
				{ "none",
				"Splash Jam",
				"Storm",
				"Contractor",
				"Carbon Fiber",
				"Sand Mesh",
				"Palm",
				"Crimson Web",
				"Cardiac",
				"Army Sheen",
				"Cyrex",
				"Grotto",
				"Bloodsport" });
			auto glockns = new C_Dropdown("Glock skins", &g_cfg.skins.GlockSkin,
				{ "none",
				"Fade",
				"Dragon Tattoo",
				"Twilight Galaxy",
				"Wasteland Rebel",
				"Water Elemental",
				"Off World",
				"Weasel",
				"Royal Legion",
				"Grinder",
				"Steel Disruption",
				"Brass",
				"Ironwork",
				"Bunsen Burner",
				"Reactor" });
			auto G3sg1ns = new C_Dropdown("G3sg1 skins", &g_cfg.skins.G3sg1Skin,
				{ "none",
				"Hunter",
				"The Executioner",
				"Terrace",
				"Neon Kimono",
				"Orange Kimono",
				"Predator",
				"Chronos"
				});
			auto dualsns = new C_Dropdown("Duals skins", &g_cfg.skins.DualSkin,
				{ "none",
				"Panther",
				"Dualing Dragons",
				"Cobra Strike",
				"Royal Consorts",
				"Duelist" });
			auto deaglens = new C_Dropdown("Deagle skins", &g_cfg.skins.DeagleSkin,
				{ "none",
				"Blaze",
				"Kumicho Dragon",
				"Oxide Blaze",
				"Golden Koi",
				"Cobalt Disruption",
				"Directive" });
			auto uspns = new C_Dropdown("USP skins", &g_cfg.skins.USPSkin,
				{ "none",
				"Neo-Noir",
				"Cyrex",
				"Orion",
				"Kill Confirmed",
				"Overgrowth",
				"Caiman",
				"Serum",
				"Guardian",
				"Road Rash" });
		}
		auto knifechanger2222222 = new C_GroupBox(GROUP_RIGHT, 12, "");
		{
			auto ak = new C_Dropdown("AK47 skins", &g_cfg.skins.AK47Skin,
				{ "none",
				"Fire serpent",
				"Fuel Injector",
				"Bloodsport",
				"Vulcan",
				"Case hardened",
				"Hydroponic",
				"Aquamarine Revenge",
				"Frontside Misty",
				"Point Disarray",
				"Neon Revolution",
				"Red laminate",
				"Redline",
				"Jaguar",
				"Jetset",
				"Wasteland rebel",
				"The Empress",
				"Elite Build" });
			auto P2000 = new C_Dropdown("P2000 skins", &g_cfg.skins.P2000Skin,
				{ "none",
				"Handgun",
				"Fade",
				"Corticera",
				"Ocean Foam",
				"Fire Elemental",
				"Asterion",
				"Pathfinder",
				"Imperial",
				"Oceanic",
				"Imperial Dragon",
				});
			auto uspns = new C_Dropdown("Revolver skins", &g_cfg.skins.RevolverSkin,
				{ "none",
				"Llama Cannon",
				"Fade",
				"Crimson Web",
				"Reboot",
				"Nitro",
				"Bone Mask"
				});
			auto forceupdate = new C_Button("Full Update", []() { KnifeApplyCallbk(); });
			set_tabsub(1);
			auto knifechanger2 = new C_GroupBox(GROUP_LEFT, 12, "KnifeChanger");
			{
				auto knif = new C_Dropdown("Knife Model", &g_cfg.skins.Knife, { "Default",
					"Bayonet",
					"Flip Knife",
					"Gut Knife",
					"Karambit",
					"M9 Bayonet",
					"Huntsman Knife",
					"Butterfly Knife",
					"Falchion Knife",
					"Shadow Daggers",
					"Bowie Knife",
					"Navaja Knife",
					"Stiletto Knife",
					"Ursus Knife",
					"Talon Knife" });
				auto knifskin = new C_Dropdown("Knife Skin", &g_cfg.skins.KnifeSkin, { "none",
					"Crimson Web",
					"Bone Mask",
					"Fade",
					"Night",
					"Blue Steel",
					"Stained",
					"Case Hardened",
					"Slaughter",
					"Safari Mesh",
					"Boreal Forest",
					"Ultraviolet",
					"Urban Masked",
					"Scorched",
					"Rust Coat",
					"Tiger Tooth",
					"Damascus Steel",
					"Damascus Steel",
					"Marble Fade",
					"Rust Coat",
					"Doppler Ruby",
					"Doppler Sapphire",
					"Doppler Blackpearl",
					"Doppler Phase 1",
					"Doppler Phase 2",
					"Doppler Phase 3",
					"Doppler Phase 4",
					"Gamma Doppler Phase 1",
					"Gamma Doppler Phase 2",
					"Gamma Doppler Phase 3",
					"Gamma Doppler Phase 4",
					"Gamma Doppler Emerald",
					"Lore",
					"Black Laminate",
					"Autotronic",
					"Freehand" });
				auto forceupdate = new C_Button("Full Update", []() { KnifeApplyCallbk(); });
			}
			auto knifechanger3 = new C_GroupBox(GROUP_RIGHT, 12, "GloveChanger(soon)");
			{
				auto forceupdate = new C_Button("Full Update", []() { KnifeApplyCallbk(); });
			}
			auto configs_tab = new C_Tab(&menu::get(), 5, "I", "Settings"); {
				auto config = new C_GroupBox(GROUP_LEFT, 8, "settings"); {
					new C_TextSeperator("Menu Color", SEPERATOR_NORMAL, { new C_ColorSelector(&g_cfg.menu.menu_theme[0]) });
					//new C_CheckBox("Rainbow Bar", &g_cfg.misc.RainbowBarraight);
					//new C_SliderFloat("Rainbow Speed", &g_cfg.misc.rainbowspeedraight, 1, 10, "");
					new C_CheckBox("Rainbow Menu", &g_cfg.misc.RainbowBar);
					new C_SliderFloat("Rainbow Speed", &g_cfg.misc.rainbowspeed, 1, 10, "");
					new C_KeyBind("Toggle Menu Key", &g_cfg.menu.menu_bind);
					new C_CheckBox("Anti Unstrusted", &g_cfg.misc.anti_untrusted);
					new C_CheckBox("TrashTalk", &g_cfg.misc.trashtalker);
					new C_CheckBox("Auto Defuse", &g_cfg.misc.autodefuse);
					new C_CheckBox("Custom Viewmodel", &g_cfg.misc.customviewmodel);
					new C_SliderFloat("> X", &g_cfg.misc.CustomViewModelX, -10, 10, "");
					new C_SliderFloat("> Y", &g_cfg.misc.CustomViewModelY, -10, 10, "");
					new C_SliderFloat("> Z", &g_cfg.misc.CustomViewModelZ, -10, 10, "");
				}
				auto config_2 = new C_GroupBox(GROUP_RIGHT, 8, "cfg"); {
					auto config_dropdown = new C_Dropdown("", &g_cfg.selected_config, &cfg_manager->files);
					auto function_seperator = new C_TextSeperator("Configuration Settings", SEPERATOR_BOLD);
					auto load = new C_Button("Load Configuration", []() { load_config(); });
					auto save = new C_Button("Save Configuration", []() { save_config(); });
					auto remove = new C_Button("Delete Configuration", []() { remove_config(); });
					new C_TextField("Configuration Name", &g_cfg.new_config_name);
					auto add = new C_Button("Add New Configuration", []() { add_config(); });
					new C_Button("Unload Hack", []() { unload_cheat(); });
				}
			}
			/*auto exploit_tab = new C_Tab(&menu::get(), 7, "g", "Exploits"); {
			auto Exploits = new C_GroupBox(GROUP_LEFT, 12, "Exploits"); {
			new C_CheckBox("P Walk", &g_cfg.fastwalk.enabled);
			new C_CheckBox("Lag Spike", &g_cfg.lagspike.enabled);
			}
			auto Exploits2 = new C_GroupBox(GROUP_RIGHT, 12, ""); {
			auto seperator2 = new C_TextSeperator("", SEPERATOR_NORMAL);
			auto LagSpiketext1 = new C_TextSeperator("Lag Spike", SEPERATOR_NORMAL);
			new C_KeyBind("Lag Spike Key", &g_cfg.lagspike.key);
			}
			}*/

			/*auto model_tab = new C_Tab(&menu::get(), 7, "f", "Modelchanger"); {
				auto models12 = new C_GroupBox(GROUP_LEFT, 12, "models");
				{
					bool mdl1 = false;
					auto override_fov_scoped = new C_CheckBox("Enable ModelsChanger", &mdl1);

				}
			}*/
		}
	}
}