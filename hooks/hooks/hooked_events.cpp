#include "..\hooks.hpp"
#include "..\..\cheats\misc\logs.h"
#include "..\..\cheats\visuals\other_esp.h"
#include "..\..\cheats\visuals\bullet_tracers.h"
#include "..\..\Hack\ragebot.h"
#include "..\..\Hack\Resolver_Remake.h"
#include "../../cheats/visuals/player_esp.h"
#pragma comment(lib, "Winmm.lib")


float duration;
CBoneAccessor* at;
IVDebugOverlay* se;
IClientEntity* pEntity;
/*
short   m_nUserID		user ID who was hurt
short	attacker	user ID who attacked
byte	health		remaining health points
byte	armor		remaining armor points
string	weapon		weapon name attacker used, if not the world
short	dmg_health	damage done to health
byte	dmg_armor	damage done to armor
byte	hitgroup	hitgroup that was damaged
*/
enum {
	KEVLARHELM,
	DEFUSE,
	ZEUS
};

enum {
	galil,
	famas,
	ak47,
	m4a4,
	m4a1,
	ssg,
	aug,
	sg556,
	awp,
	scar20,
	g3sg1,
	nova,
	xm1014,
	mag7,
	m249,
	negev,
	mac10,
	mp9,
	mp7,
	ump45,
	p90,
	bizon
};

enum {
	glock,
	hkp2000,
	usp_silencer,
	elite,
	p250,
	tec9,
	five7,
	deagle
};

/*
		{ false, "glock" },
		{ false, "hkp2000" },
		{ false, "usp_silencer" },
		{ false, "elite" },
		{ false, "p250" },
		{ false, "tec9" },
		{ false, "fiveseven" },
		{ false, "deagle" }

		{ false, "galilar" },
		{ false, "famas" },
		{ false, "ak47" },
		{ false, "m4a1" },
		{ false, "m4a1_silencer" },
		{ false, "ssg08" },
		{ false, "aug" },
		{ false, "sg556" },
		{ false, "awp" },
		{ false, "scar20" },
		{ false, "g3sg1" },
		{ false, "nova" },
		{ false, "xm1014" },
		{ false, "mag7" },
		{ false, "m249" },
		{ false, "negev" },
		{ false, "mac10" },
		{ false, "mp9" },
		{ false, "mp7" },
		{ false, "ump45" },
		{ false, "p90" },
		{ false, "bizon" }
*/
void C_HookedEvents::FireGameEvent(IGameEvent* event) {
	static ConVar* sv_showimpacts = g_csgo.m_cvar()->FindVar("sv_showimpacts");
	static auto hit_sound = [&event]() -> void {
		if (!g_ctx.available())
			return;

		/*
		weapons/mortar/mortar_explode1.wav
		weapons/mortar/mortar_explode2.wav
		weapons/mortar/mortar_explode3.wav
		*/
		/*
		if (g_Settings::FakeMedia && attacker == pLocal)
		{
			if (!pEvent->GetBool("headshot"))
			pEvent->SetInt("headshot", 1);
}

		*/
		auto attacker = event->GetInt("attacker");
		auto user = event->GetInt("userid");
		int dmg = event->GetInt("dmg_health");
		bool headshot = event->GetBool("headshot");

		if (g_cfg.esp.hitsound) {
			if (g_csgo.m_engine()->GetPlayerForUserID(user) != g_csgo.m_engine()->GetLocalPlayer() && g_csgo.m_engine()->GetPlayerForUserID(attacker) == g_csgo.m_engine()->GetLocalPlayer()) {
				if (dmg >= 100)
				{
					g_csgo.m_surface()->PlaySound_("physics\\wood\\wood_box_impact_hard5.wav");
				}
				else if (headshot)
				{
					g_csgo.m_surface()->PlaySound_("physics\\wood\\wood_box_impact_hard3.wav");
				}
				else
					g_csgo.m_surface()->PlaySound_("buttons\\arena_switch_press_02.wav");
			}
		}
	};

	static auto trashtalk = [&event]() -> void {//owo i see an empty section, lets change that c;
		if (!g_ctx.available()) return;
		auto attacker = event->GetInt("attacker");
		auto user = event->GetInt("userid");
		auto assist = event->GetInt("assister");
		auto penetrated = event->GetInt("penetrated");
		bool headshot = event->GetBool("headshot");
		player_info_t player;
		if (g_csgo.m_engine()->GetPlayerForUserID(user) != g_csgo.m_engine()->GetLocalPlayer() && g_csgo.m_engine()->GetPlayerForUserID(attacker) == g_csgo.m_engine()->GetLocalPlayer())
		{
			char chattt[250];
			int diff = math::random_float(1, 10);
			switch (diff) {
			case 1:
			{
				g_csgo.m_engine()->ExecuteClientCmd("say 1");
			}break;
			case 2:
			{
				g_csgo.m_engine()->ExecuteClientCmd("say u bot?");
			}break;
			case 3:
			{
				g_csgo.m_engine()->ExecuteClientCmd("say buy sectum.tk");
			}break;
			case 4:
			{
				g_csgo.m_engine()->ExecuteClientCmd("say ohhh");
			}break;
			case 5:
			{
				g_csgo.m_engine()->ExecuteClientCmd("say haahahha meme");
			}break;
			case 6:
			{
				g_csgo.m_engine()->ExecuteClientCmd("say tap 1 bots");
			}break;
			case 7:
			{
				g_csgo.m_engine()->ExecuteClientCmd("say 1w");
			}break;
			case 8:
			{
				g_csgo.m_engine()->ExecuteClientCmd("say - u");
			}break;
			}
		}
	};
	static auto resolvershots = [&event]() -> void {
		if (!g_ctx.available())
			return;
		auto attacker = event->GetInt("attacker");
		auto user = event->GetInt("userid");
		int dmg = event->GetInt("dmg_health");
		bool headshot = event->GetBool("headshot");
		if (g_csgo.m_engine()->GetPlayerForUserID(user) != g_csgo.m_engine()->GetLocalPlayer() && g_csgo.m_engine()->GetPlayerForUserID(attacker) == g_csgo.m_engine()->GetLocalPlayer())
		{
			auto player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetPlayerForUserID(user)));
			g_ctx.m_globals.hit[player->EntIndex()] ++;
			if (g_ctx.m_globals.hit[player->EntIndex()] > 3)
				g_ctx.m_globals.hit[player->EntIndex()] = 0;
		}
	};
	//g_ctx.m_globals.shots
	auto event_name = event->GetName();

	if (!strcmp(event_name, "round_prestart"))
		antiaim::get().freeze_check = true;
	else if (!strcmp(event_name, "round_freeze_end"))
		antiaim::get().freeze_check = false;

	if (!strcmp(event_name, "round_start"))
	{
		g_ctx.m_globals.roundstart = true;

		if (g_cfg.BuyBot.enable)
		{
#pragma region extras
			if (g_cfg.BuyBot.extras[KEVLARHELM].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy vesthelm; buy vest;");
			}
			if (g_cfg.BuyBot.extras[DEFUSE].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy defuser;");
			}
			if (g_cfg.BuyBot.extras[ZEUS].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy taser;");
			}

#pragma endregion

#pragma region primary

			if (g_cfg.BuyBot.primary[galil].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy galilar;");
			}
			if (g_cfg.BuyBot.primary[famas].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy famas;");
			}
			if (g_cfg.BuyBot.primary[ak47].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy ak47;");
			}
			if (g_cfg.BuyBot.primary[m4a4].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy m4a1;");
			}
			if (g_cfg.BuyBot.primary[m4a1].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy m4a1_silencer;");
			}
			if (g_cfg.BuyBot.primary[ssg].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy ssg08;");
			}
			if (g_cfg.BuyBot.primary[aug].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy aug;");
			}
			if (g_cfg.BuyBot.primary[sg556].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy sg556;");
			}
			if (g_cfg.BuyBot.primary[awp].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy awp;");
			}
			if (g_cfg.BuyBot.primary[scar20].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy scar20;");
			}
			if (g_cfg.BuyBot.primary[g3sg1].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy g3sg1;");
			}
			if (g_cfg.BuyBot.primary[nova].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy nova;");
			}
			if (g_cfg.BuyBot.primary[xm1014].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy xm1014;");
			}
			if (g_cfg.BuyBot.primary[m249].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy m249;");
			}
			if (g_cfg.BuyBot.primary[negev].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy negev;");
			}
			if (g_cfg.BuyBot.primary[mac10].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy mac10;");
			}
			if (g_cfg.BuyBot.primary[mp9].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy mp9;");
			}
			if (g_cfg.BuyBot.primary[mp7].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy mp7;");
			}
			if (g_cfg.BuyBot.primary[ump45].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy ump45;");
			}
			if (g_cfg.BuyBot.primary[p90].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy p90;");
			}
			if (g_cfg.BuyBot.primary[bizon].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy bizon;");
			}
#pragma endregion

#pragma region secondary

			if (g_cfg.BuyBot.secondary[glock].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy glock;");
			}
			if (g_cfg.BuyBot.secondary[hkp2000].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy hkp2000;");
			}
			if (g_cfg.BuyBot.secondary[usp_silencer].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy usp_silencer;");
			}
			if (g_cfg.BuyBot.secondary[elite].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy elite;");
			}
			if (g_cfg.BuyBot.secondary[p250].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy p250;");
			}
			if (g_cfg.BuyBot.secondary[tec9].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy tec9;");
			}
			if (g_cfg.BuyBot.secondary[five7].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy fn57;");
			}
			if (g_cfg.BuyBot.secondary[deagle].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy deagle;");
			}

#pragma endregion

#pragma region nades
			if (g_cfg.BuyBot.nades[NADES_HE].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy hegrenade;");
			}
			if (g_cfg.BuyBot.nades[NADES_SMOKE].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy smokegrenade;");
			}
			if (g_cfg.BuyBot.nades[NADES_MOLLY].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy molotov;");
			}
			if (g_cfg.BuyBot.nades[NADES_FLASH].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy flashbang;");
			}
			else if (g_cfg.BuyBot.nades[NADES_FLASH2].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy flashbang; buy flashbang;");
			}
			if (g_cfg.BuyBot.nades[NADES_DECOY].enabled)
			{
				g_csgo.m_engine()->ClientCmd("buy decoy;");
			}
#pragma endregion
		}
	}
	else
	{
		g_ctx.m_globals.roundstart = false;
	}

	/*
	  glock
	  hkp2000
	  usp_silencer
	  elite
	  p250
	  tec9
	  fn57
	  deagle
	  galilar
	  famas
	  ak47
	  m4a1
	  m4a1_silencer
	  ssg08
	  aug
	  sg556
	  awp
	  scar20
	  g3sg1
	  nova
	  xm1014
	  mag7
	  m249
	  negev
	  mac10
	  mp9
	  mp7
	  ump45
	  p90
	  bizon
	  vest
	  vesthelm
	  taser
	  defuser
	  heavyarmor
	  molotov
	  incgrenade
	  decoy
	  flashbang
	  hegrenade
	  smokegrenade
	  tagrenade
				*/

				//buy ssg08 buy vesthelm; buy vest; g_cfg.BuyBot.zeus buy defuser;
				//buy incgrenade; buy molotov; buy decoy; buy flashbang; buy hegrenade; buy smokegrenade;
	if (g_cfg.ragebot.resolver_experimental)
		ResolverOvermake::resolver.EventCallback(event);

	if (g_cfg.esp.hitsound) {
		if (!strcmp(event_name, "player_hurt"))
			hit_sound();
	}
	if (!strcmp(event_name, "round_prestart"))
		g_ctx.m_globals.NewRound = true;
	if (g_cfg.ragebot.resolver_experimental)
		resolvershots();

	if (g_cfg.misc.trashtalker)
		if (!strcmp(event_name, "player_death"))
			trashtalk();

	if (g_cfg.esp.hitmarker[HITMARKER_STATIC].enabled || g_cfg.esp.hitmarker[HITMARKER_DYNAMIC].enabled)
		otheresp::get().hitmarker_event(event);

	if (g_cfg.esp.bullet_tracer || g_cfg.esp.bullet_tracer_enemy)
		bullettracers::get().events(event);

	if (g_cfg.esp.bullet_tracer)
		sv_showimpacts->SetValue(1);
	else
		sv_showimpacts->SetValue(0);

	eventlogs::get().events(event);

	g_ctx.m_globals.remove_shot(event);

	IClientEntity* hurt = (IClientEntity*)g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetPlayerForUserID(event->GetInt("userid")));

	if (g_cfg.esp.drawhits) {
		//if (!strcmp(event_name, "player_hurt"))
			//playeresp::get().draw_hitboxes(hurt, 220, 220, 220, 255, 1, 0);
	}

}

int C_HookedEvents::GetEventDebugID(void) {
	return EVENT_DEBUG_ID_INIT;
}

void C_HookedEvents::RegisterSelf() {
	m_iDebugId = EVENT_DEBUG_ID_INIT;
	g_csgo.m_eventmanager()->AddListener(this, "player_hurt", false);
	g_csgo.m_eventmanager()->AddListener(this, "item_purchase", false);
	g_csgo.m_eventmanager()->AddListener(this, "bomb_beginplant", false);
	g_csgo.m_eventmanager()->AddListener(this, "bomb_begindefuse", false);
	g_csgo.m_eventmanager()->AddListener(this, "bullet_impact", false);
	g_csgo.m_eventmanager()->AddListener(this, "weapon_fire", false);
	g_csgo.m_eventmanager()->AddListener(this, "round_prestart", false);
	g_csgo.m_eventmanager()->AddListener(this, "round_start", false);
	g_csgo.m_eventmanager()->AddListener(this, "round_freeze_end", false);
}

void C_HookedEvents::RemoveSelf() {
	g_csgo.m_eventmanager()->RemoveListener(this);
}