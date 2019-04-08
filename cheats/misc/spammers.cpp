#include "spammers.h"

void spammers::clan_tag() {
	auto apply = [](const char * name) -> void {
		using Fn = int(__fastcall *)(const char *, const char *);
		static auto fn = reinterpret_cast<Fn>(util::pattern_scan("engine.dll", "53 56 57 8B DA 8B F9 FF 15"));
		fn(name, name);
	};

	if (g_cfg.misc.clantag_modes == 0)
	{
		static std::string clantagstring = "moneysense ";
		static float last_time = 0;
		if (g_csgo.m_globals()->m_curtime > last_time) {
			clantagstring += clantagstring.at(0);
			clantagstring.erase(0, 1);
			std::string clanprefix;
			if (g_cfg.misc.clantagprefix)
			{
				if (g_cfg.misc.clantagprefixmodes == 0) clanprefix = u8"先輩|";
				else if (g_cfg.misc.clantagprefixmodes == 1) clanprefix = u8"⚡|";
				else if (g_cfg.misc.clantagprefixmodes == 2) clanprefix = u8"♕|";
				else if (g_cfg.misc.clantagprefixmodes == 3) clanprefix = u8"♿|";
				else if (g_cfg.misc.clantagprefixmodes == 4) clanprefix = u8"180 名|";
				else if (g_cfg.misc.clantagprefixmodes == 5) clanprefix = u8"☭ ";
			}
			else clanprefix = u8"";
			clanprefix += clantagstring;
			apply(clanprefix.c_str());
			last_time = g_csgo.m_globals()->m_curtime + 0.9f;
		}
		if (fabs(last_time - g_csgo.m_globals()->m_curtime) > 1.f) last_time = g_csgo.m_globals()->m_curtime;
	}
	else {
		static std::string clantagstring;

		if (g_cfg.misc.clantag_modes == 1)
		{
			static std::string clantagstring = "moneybot ";
			static float last_time = 0;
			if (g_csgo.m_globals()->m_curtime > last_time) {
				clantagstring += clantagstring.at(0);
				clantagstring.erase(0, 1);
				std::string clanprefix;
				if (g_cfg.misc.clantagprefix)
				{
					if (g_cfg.misc.clantagprefixmodes == 0) clanprefix = u8"先輩|";
					else if (g_cfg.misc.clantagprefixmodes == 1) clanprefix = u8"⚡|";
					else if (g_cfg.misc.clantagprefixmodes == 2) clanprefix = u8"♕|";
					else if (g_cfg.misc.clantagprefixmodes == 3) clanprefix = u8"♿|";
					else if (g_cfg.misc.clantagprefixmodes == 4) clanprefix = u8"180 名|";
					else if (g_cfg.misc.clantagprefixmodes == 5) clanprefix = u8"☭ ";
				}
				else clanprefix = u8"";
				clanprefix += clantagstring;
				apply(clanprefix.c_str());
				last_time = g_csgo.m_globals()->m_curtime + 0.9f;
			}
			if (fabs(last_time - g_csgo.m_globals()->m_curtime) > 1.f) last_time = g_csgo.m_globals()->m_curtime;
		}
		else if (g_cfg.misc.clantag_modes == 2)
		{
			static std::string clantagstring = "gamesense ";
			static float last_time = 0;
			if (g_csgo.m_globals()->m_curtime > last_time) {
				clantagstring += clantagstring.at(0);
				clantagstring.erase(0, 1);
				std::string clanprefix;
				if (g_cfg.misc.clantagprefix)
				{
					if (g_cfg.misc.clantagprefixmodes == 0) clanprefix = u8"先輩|";
					else if (g_cfg.misc.clantagprefixmodes == 1) clanprefix = u8"⚡|";
					else if (g_cfg.misc.clantagprefixmodes == 2) clanprefix = u8"♕|";
					else if (g_cfg.misc.clantagprefixmodes == 3) clanprefix = u8"♿|";
					else if (g_cfg.misc.clantagprefixmodes == 4) clanprefix = u8"180 名|";
					else if (g_cfg.misc.clantagprefixmodes == 5) clanprefix = u8"☭ ";
				}
				else clanprefix = u8"";
				clanprefix += clantagstring;
				apply(clanprefix.c_str());
				last_time = g_csgo.m_globals()->m_curtime + 0.9f;
			}
			if (fabs(last_time - g_csgo.m_globals()->m_curtime) > 1.f) last_time = g_csgo.m_globals()->m_curtime;
		}
		else if (g_cfg.misc.clantag_modes == 3)
		{
			clantagstring = u8"																																							";
		}
		else if (g_cfg.misc.clantag_modes == 4)
		{
			clantagstring = u8" cummies ";
		}
		else if (g_cfg.misc.clantag_modes == 5) {
			clantagstring = g_cfg.misc.customclantagyo.c_str();
		}
		else {
			clantagstring = u8"";
		}
		static float last_time = 0;
		if (g_csgo.m_globals()->m_curtime > last_time) {
			clantagstring += clantagstring.at(0);
			clantagstring.erase(0, 1);
			std::string clanprefix;
			if (g_cfg.misc.clantagprefix)
			{
				if (g_cfg.misc.clantagprefixmodes == 0)
					clanprefix = u8"先輩|";
				else if (g_cfg.misc.clantagprefixmodes == 1)
					clanprefix = u8"⚡|";
				else if (g_cfg.misc.clantagprefixmodes == 2)
					clanprefix = u8"♕|";
				else if (g_cfg.misc.clantagprefixmodes == 3)
					clanprefix = u8"♿|";
				else if (g_cfg.misc.clantagprefixmodes == 4) clanprefix = u8"180 名|";
				else if (g_cfg.misc.clantagprefixmodes == 5)
					clanprefix = u8"☭ ";
			}
			else clanprefix = u8"";
			clanprefix += clantagstring;
			apply(clanprefix.c_str());
			last_time = g_csgo.m_globals()->m_curtime + 0.9f;
		}
		if (fabs(last_time - g_csgo.m_globals()->m_curtime) > 1.f) last_time = g_csgo.m_globals()->m_curtime;
	}
}