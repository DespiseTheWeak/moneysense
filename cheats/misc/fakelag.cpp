#include "fakelag.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\aimbot.h"
#include "../../NaNWalk.h"

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

float curtime(CUserCmd* ucmd) {
	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->m_predicted) {
		g_tick = g_ctx.m_local->m_nTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * g_csgo.m_globals()->m_interval_per_tick;
	return curtime;
}
void AstroWalk() {
	Vector velocity = g_ctx.m_local->m_vecVelocity();
	Vector direction = velocity.Direction();
	float speed = velocity.Length();

	direction.y = g_ctx.get_command()->m_viewangles.y - direction.y;

	auto local_weapon = g_ctx.m_local->m_hActiveWeapon()->get_csweapon_info();
	Vector negated_direction = direction * -speed;
	if (velocity.Length() >= (local_weapon->max_speed * .25f))
	{
		g_ctx.get_command()->m_forwardmove = negated_direction.y;
		g_ctx.get_command()->m_sidemove = negated_direction.x;
	}
	float delta = g_ctx.m_local->m_flSimulationTime() - TICKS_TO_TIME(g_ctx.get_command()->m_tickcount);
	{
		static bool switcher = true;
		if (switcher)
		{
			g_ctx.get_command()->m_tickcount += fabsf(delta);
			switcher = false;
		}
		else
		{
			g_ctx.get_command()->m_tickcount = TIME_TO_TICKS(g_csgo.m_globals()->m_frametime);
			switcher = true;
		}
	}
}

void fakelag::create_move() {
	auto weapon = g_ctx.m_local->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
		return;

	bool should_fakelag = false;
	bool should_exploit = false;
	static bool new_factor = false;
	bool
		moving,
		inair,
		standing,
		shooting;

	int
		tick_times = ((int)(1.0f / g_csgo.m_globals()->m_interval_per_tick)) / 64,
		fakelag_amountstand,
		fakelag_amountair,
		fakelag_amountshoot,
		fakelag_amountmove,
		maxchockeamount = 14,
		choke = 0;

	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND))
	{
		//fakelag_amountair = g_cfg.antiaim.breaklagcomp ? BreakLagCompensation() : g_cfg.antiaim.fakelag_amountinair;
		fakelag_amountair = g_cfg.antiaim.fakelag_amountinair;
		inair = true,
			moving = false,
			standing = false,
			shooting = false;
	}
	else if (g_ctx.m_local->m_fFlags() & FL_ONGROUND && g_ctx.m_local->m_vecVelocity().Length() > 0)
	{
		//fakelag_amountmove = g_cfg.antiaim.breaklagcomp ? BreakLagCompensation() : g_cfg.antiaim.fakelag_amountmoving;
		fakelag_amountmove = g_cfg.antiaim.fakelag_amountmoving;
		inair = false,
			moving = true,
			standing = false,
			shooting = false;
	}
	else if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
	{
		fakelag_amountshoot = g_cfg.antiaim.fakelag_amountother;
		inair = false,
			moving = false,
			standing = false,
			shooting = true;
	}
	else if (g_ctx.m_local->m_fFlags() & FL_ONGROUND && g_ctx.m_local->m_vecVelocity().Length() <= 0)
	{
		fakelag_amountstand = g_cfg.antiaim.fakelag_amountstanding;
		inair = false,
			moving = false,
			standing = true,
			shooting = false;
	}

	switch (g_cfg.antiaim.fakelag_mode)
	{
	case 1:
	{
		if (standing)
			choke = min(fakelag_amountstand * tick_times, 14);
		else if (shooting)
			choke = min(fakelag_amountshoot * tick_times, 14);
		else if (moving)
			choke = min(fakelag_amountmove * tick_times, 14);
		else if (inair)
			choke = min(fakelag_amountair * tick_times, 14);
	}break;
	case 2:
	{
		if (standing)
			choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
		else if (shooting)
			choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
		else if (moving)
			choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
		else if (inair)
			choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
	}break;
	case 3:
	{
		if (standing)
			choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
		else if (shooting)
			choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
		else if (moving)
			choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
		else if (inair)
			choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
	}break;
	case 4:
	{
		if (new_factor)
		{
			static int FakelagFactor = 10;
			static int m_iOldHealth = 0;
			static int iChoked;
			if (m_iOldHealth != g_ctx.m_local->m_iHealth())
			{
				m_iOldHealth = g_ctx.m_local->m_iHealth();
				if (standing)
					choke = rand() % 7 + 6 + 1;
				else if (shooting)
					choke = rand() % 7 + 6 + 1;
				else if (moving)
					choke = rand() % 7 + 6 + 1;
				else if (inair)
					choke = rand() % 7 + 6 + 1;
				new_factor = false;
			}
		}
	}break;
	case 5:
	{
		if (new_factor)
		{
			int min_packets = 0;
			Vector vel_p_t = Vector(g_ctx.m_local->m_vecVelocity() *  g_csgo.m_globals()->m_interval_per_tick);
			for (int i = 1; i <= 14; i++)
			{
				if (Vector(vel_p_t * i).Length() > 64)
				{
					min_packets = i;
					break;
				}
			}
			if (standing)
				choke = max(min_packets, fakelag_amountstand);
			else if (shooting)
				choke = max(min_packets, fakelag_amountshoot);
			else if (moving)
				choke = max(min_packets, fakelag_amountmove);
			else if (inair)
				choke = max(min_packets, fakelag_amountair);
		}
	}break;

	case 6:
	{
		Vector velocity = g_ctx.m_local->m_vecVelocity();
		velocity.z = 0;
		float speed = velocity.Length();
		if (speed > 0.f) {
			auto distance_per_tick = speed * g_csgo.m_globals()->m_interval_per_tick;
			int choked_ticks = std::ceilf(65.f / distance_per_tick);
			if (g_ctx.m_local->m_fFlags() & FL_ONGROUND)
				choke = std::min<int>(choked_ticks, g_cfg.antiaim.fakelag_amountmoving);
			else
				choke = std::min<int>(choked_ticks, g_cfg.antiaim.fakelag_amountinair);
		}
		else
			choke = 1;
	}break;
	}

	if (g_cfg.antiaim.fakelag_enablers[FAKELAG_AIR].enabled) {
		if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND))
			should_fakelag = true;
		should_exploit = true;
	}

	if (g_ctx.m_local->m_vecVelocity().Length() > 0 && g_ctx.m_local->m_fFlags() & FL_ONGROUND) {
		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_MOVING].enabled) {
			if (g_ctx.m_local->m_vecVelocity().Length2D())
			{
				should_fakelag = true;
				should_exploit = true;
			}
		}

		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_SHOOTING].enabled) {
			if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
			{
				should_fakelag = true;
				should_exploit = true;
			}
		}

		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_STANDING].enabled) {
			if (g_ctx.m_local->m_fFlags() & FL_ONGROUND && !g_ctx.m_local->m_vecVelocity().Length())
			{
				should_fakelag = true;
				should_exploit = true;
			}
		}

		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_SHOOTING].enabled) {
			if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
				should_fakelag = true;
		}

		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_ONPEEK].enabled) {			//autism
		}
		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_UNDUCK].enabled) {
			if (g_ctx.m_local->m_fFlags() & IN_BULLRUSH)
			{
				should_fakelag = true;
			}
		}
		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_WEAPONSWITCH].enabled) {
		}
		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_RELOAD].enabled) {
		}
		if (g_cfg.antiaim.fakelag_enablers[FAKELAG_ONKEY].enabled) {
			if (GetAsyncKeyState(g_cfg.antiaim.fakelagonkey))
			{
				should_fakelag = true;
			}
		}
		if (g_csgo.m_engine()->IsVoiceRecording())	//big large fakelag voice fix
		{
			should_fakelag = false;
			should_exploit = false;
		}
		vec3_t next_angle = math::calculate_angle(g_ctx.m_local->get_eye_pos(), aimbot::get().target_data.aimspot);
		if (g_ctx.m_local->m_hActiveWeapon()->m_iItemDefinitionIndex() == (short)ItemDefinitionIndex::WEAPON_TASER)
			if (aimbot::get().can_zeus(next_angle))
				should_fakelag = false;
		if (g_ctx.m_globals.fakewalking)
			return;
		if (should_exploit)
		{
		}

		/*if (g_cfg.nanwalk.enabled)
		{
			NaNWalk::get().Old();
		}*/
	}
	//g_cfg.fastwalk.key
// 		if (choke >= maxchockeamount)
// 			choke = maxchockeamount;
	if (should_fakelag) {
		static int choked = 0;/*
		if (g_ctx.get_command()->m_command_number % 30 < choke) //use me correctly u fucking prick
			choked = min(choke, 15);
		else
			choked = 0;*/

		if (choked > choke) {
			choked = 0;
			g_ctx.send_packet = true;
			new_factor = true;
		}
		else {
			g_ctx.send_packet = false;
			choked++;
		}
		g_ctx.m_globals.fakelagamt = choke;
		g_ctx.m_globals.fakelagging = true;
	}
}

/*		Mutiny break lag comp (no work)
	float speed = g_ctx.m_local->m_vecVelocity().Length2D() * g_csgo.m_globals()->m_interval_per_tick;
		int chokeAmmount = 0;
		if (speed < 1)//should be 66/maxChoke instead of 1 but meh
		{
			choke = maxchockeamount;
		}
		else
		{
			while (speed * chokeAmmount <= 70)
			{
				//While they aren't breaking lag comp
				chokeAmmount++;
			}
			if (chokeAmmount > maxchockeamount)
			{
				choke = maxchockeamount;
			}
			else
			{//Breaking lag comp, lets make some different
				//values in this area not just minumin
				int minChoke = chokeAmmount;
				int stepVal = minChoke;
				if (minChoke < g_cfg.antiaim.fakelag_amountstanding && standing)
				{
					stepVal = g_cfg.antiaim.fakelag_amountstanding;
				}
				if (minChoke < g_cfg.antiaim.fakelag_amountmoving && moving)
				{
					stepVal = g_cfg.antiaim.fakelag_amountmoving;
				}
				if (minChoke < g_cfg.antiaim.fakelag_amountinair && inair)
				{
					stepVal = g_cfg.antiaim.fakelag_amountinair;
				}
				if (minChoke < g_cfg.antiaim.fakelag_amountshooting && shooting)
				{
					stepVal = g_cfg.antiaim.fakelag_amountshooting;
				}

				static bool switcher = false;
				static int counter = minChoke;
				if (counter < minChoke)
					counter = minChoke;
				if (!g_csgo.m_clientstate()->m_nChokedCommands)
				{
					switcher = !switcher;
					counter++;
					if (counter > maxchockeamount)
						counter = maxchockeamount;
				}
				if (switcher)
					choke = stepVal;
				else
					choke = counter;
			}
		}

*/

/*		if (g_csgo.m_engine()->IsVoiceRecording())	//big large fakelag voice fix

		{
			should_fakelag = false;
			should_exploit = false;
		}*/

		//if (g_cfg.antiaim.fakelag_onpeek) {			//fakelag on peek -> vergleichen mit freestanding
		// 			auto GetHitboxPosition = [g_ctx.m_local](int hitbox_index) -> Vector
		// 			{
		// 				if (g_ctx.m_local->IsDormant() || g_ctx.m_local->m_iHealth() <= 0)
		// 					return Vector(0, 0, 0);
		//
		// 				const auto pModel = g_ctx.m_local->GetModel();
		// 				if (!pModel)
		// 					return Vector(0, 0, 0);
		//
		// 				auto pStudioHdr = INTERFACES::ModelInfo->GetStudioModel(pModel);
		// 				if (!pStudioHdr)
		// 					return Vector(0, 0, 0);
		//
		// 				auto pSet = pStudioHdr->GetHitboxSet(0);
		// 				if (!pSet)
		// 					return Vector(0, 0, 0);
		//
		// 				if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
		// 					return Vector(0, 0, 0);
		//
		// 				auto hitbox = pSet->GetHitbox(hitbox_index);
		// 				if (!hitbox)
		// 					return Vector(0, 0, 0);
		//
		// 				const auto bone_matrix = g_ctx.m_local->hitbox_position(hitbox->bone);
		// 				const auto bone_pos = Vector(bone_matrix[0][3], bone_matrix[1][3], bone_matrix[2][3]);
		//
		// 				return ((hitbox->bbmax + hitbox->bbmin) * 0.5f) + bone_pos;
		// 			};
		//
		// 			local_player->SetupBones(nullptr, 128, BONE_USED_BY_ANYTHING, UTILS::GetCurtime());
		// 			const auto end_position_1 = GetHitboxPosition(18),
		// 				end_position_2 = GetHitboxPosition(16); //18, 16
		//
		// 			for (int i = 0; i < 64; i++)
		// 			{
		// 				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
		// 				if (!entity || entity->GetIsDormant() || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || !entity->IsAlive())
		// 					continue;
		//
		// 				const auto damage_1 = FEATURES::RAGEBOT::autowall.CalculateDamage(entity->GetVecOrigin() + Vector(0, 0, 64.f), local_player->GetVecOrigin(),
		// 					entity, local_player, 1).damage, damage_2 = FEATURES::RAGEBOT::autowall.CalculateDamage(entity->GetVecOrigin() + Vector(0, 0, 64.f),
		// 						local_player->GetVecOrigin(), entity, local_player, 1).damage;
		//
		// 				if (UTILS::Max<int>(damage_1, damage_2) > 0)
		// 				{
		// 					GLOBAL::choke_amount = 14;
		// 					break;
		// 				}
		// 			}
		//}
					// Break lag comp cOdE; da fehlt was du kek
		// 		else if (Type == 7) {
		// 			auto net_channel = g_Engine->GetNetChannelInfo();
		// 			float speed = local->GetVelocity().Length2D() * g_Globals->interval_per_tick;
		// 			int chokeAmmount = 0;
		// 			if (speed < 1)//should be 66/ticksMax instead of 1 but meh
		// 			{
		// 				packetsToChoke = ticksMax;
		// 			}
		// 			else
		// 			{
		// 				while (speed * chokeAmmount <= 70)
		// 				{
		// 					//While they aren't breaking lag comp
		// 					chokeAmmount++;
		// 				}
		// 				if (chokeAmmount > ticksMax)
		// 				{
		// 					packetsToChoke = ticksMax;
		// 				}
		// 				else
		// 				{//Breaking lag comp, lets make some different
		// 				 //values in this area not just minumin
		// 					int minChoke = chokeAmmount;
		// 					int fromMenuStand = g_Options.Ragebot.fakelag.standingamt;
		// 					int fromMenuMove = g_Options.Ragebot.fakelag.movingamt;
		// 					int fromMenuAir = g_Options.Ragebot.fakelag.inairamt;
		// 					int stepVal = minChoke;
		// 					if (minChoke < fromMenuStand && standing)
		// 					{
		// 						stepVal = fromMenuStand;
		// 					}
		// 					if (minChoke < fromMenuMove && walking)
		// 					{
		// 						stepVal = fromMenuMove;
		// 					}
		// 					if (minChoke < fromMenuAir && inair)
		// 					{
		// 						stepVal = fromMenuAir;
		// 					}
		// 					static bool switcher = false;
		// 					static int counter = minChoke;
		// 					if (counter < minChoke)
		// 						counter = minChoke;
		// 					if (!net_channel->m_nChokedPackets)
		// 					{
		// 						switcher = !switcher;
		// 						counter++;
		// 						if (counter > ticksMax)
		// 							counter = minChoke;
		// 					}
		// 					if (switcher)
		// 						packetsToChoke = stepVal;
		// 					else
		// 						packetsToChoke = counter;
		//
		// 				}
		// 			}
		/*

			auto weapon = g_ctx.m_local->m_hActiveWeapon().Get();
			if (!weapon)
				return;

			bool should_fakelag = false;

			int
				tick_times = ((int)(1.0f / g_csgo.m_globals()->m_interval_per_tick)) / 64,
				choke;

			switch (g_cfg.antiaim.fakelag_mode) {
			case 0:
				choke = min(g_cfg.antiaim.fakelag_amount * tick_times, 14);
				break;
			case 1:
				choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
				break;
			}

			if (g_cfg.antiaim.fakelag_enablers[FAKELAG_AIR].enabled) {
				if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND))
					should_fakelag = true;
			}

			if (g_ctx.m_local->m_vecVelocity().Length() > 0 && g_ctx.m_local->m_fFlags() & FL_ONGROUND) {
				if (g_cfg.antiaim.fakelag_enablers[FAKELAG_MOVING].enabled)
					should_fakelag = true;

				if (g_cfg.antiaim.fakelag_onpeek) {
				}
			}

			if (g_cfg.antiaim.fakelag_enablers[FAKELAG_SHOOTING].enabled) {
				if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
					should_fakelag = true;
			}

			if (g_cfg.antiaim.fakelag_enablers[FAKELAG_STANDING].enabled) {
				if (g_ctx.m_local->m_fFlags() & FL_ONGROUND && !g_ctx.m_local->m_vecVelocity().Length())
					should_fakelag = true;
			}

			if (!g_cfg.antiaim.fakelag_enablers[FAKELAG_SHOOTING].enabled) {
				if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
					should_fakelag = false;
			}

			if (g_ctx.m_globals.fakewalking)
				return;

			if (should_fakelag) {
				static int choked = 0;

				if (choked > choke) {
					g_ctx.send_packet = true;
					choked = 0;
				}
				else {
					g_ctx.send_packet = false;
					choked++;
				}

				g_ctx.m_globals.fakelagging = true;
			}*/

			/*	if (fltype == 1)
				{
					choke = min(fakelag_amount * tick_times, 14);
				}
				else if (fltype == 2)
				{
					choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
				}
				else if (fltype == 3)
				{
					choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
				}
				else if (fltype == 4 && new_factor)
				{
					static int FakelagFactor = 10;
					static int m_iOldHealth = 0;
					static int iChoked;
					if (m_iOldHealth != g_ctx.m_local->m_iHealth())
					{
						m_iOldHealth = g_ctx.m_local->m_iHealth();
						choke = rand() % 7 + 6 + 1;
						new_factor = false;
					}
				}
				else if (fltype == 5 && new_factor) {
					int min_packets = 0;
					Vector vel_p_t = Vector(g_ctx.m_local->m_vecVelocity() *  g_csgo.m_globals()->m_interval_per_tick);
					for (int i = 1; i <= 14; i++)
					{
						if (Vector(vel_p_t * i).Length() > 64)
						{
							min_packets = i;
							break;
						}
					}

					choke = max(min_packets, fakelag_amount);
				}
				else if (fltype == 6)
				{
					float speed = g_ctx.m_local->m_vecVelocity().Length2D() * g_csgo.m_globals()->m_interval_per_tick;
					int chokeAmmount = 0;
					if (speed < 1)//should be 66/maxChoke instead of 1 but meh
					{
						choke = maxchockeamount;
					}
					else
					{
						while (speed * chokeAmmount <= 70)
						{
							//While they aren't breaking lag comp
							chokeAmmount++;
						}
						if (chokeAmmount > maxchockeamount)
						{
							choke = maxchockeamount;
						}
						else
						{//Breaking lag comp, lets make some different
							//values in this area not just minumin
							int minChoke = chokeAmmount;
							int stepVal = minChoke;
							if (minChoke < g_cfg.antiaim.fakelag_amountstanding && standing)
							{
								stepVal = g_cfg.antiaim.fakelag_amountstanding;
							}
							if (minChoke < g_cfg.antiaim.fakelag_amountmoving && moving)
							{
								stepVal = g_cfg.antiaim.fakelag_amountmoving;
							}
							if (minChoke < g_cfg.antiaim.fakelag_amountinair && inair)
							{
								stepVal = g_cfg.antiaim.fakelag_amountinair;
							}
							if (minChoke < g_cfg.antiaim.fakelag_amountshooting && shooting)
							{
								stepVal = g_cfg.antiaim.fakelag_amountshooting;
							}

							static bool switcher = false;
							static int counter = minChoke;
							if (counter < minChoke)
								counter = minChoke;
							if (!g_csgo.m_clientstate()->m_nChokedCommands)
							{
								switcher = !switcher;
								counter++;
								if (counter > maxchockeamount)
									counter = maxchockeamount;
							}
							if (switcher)
								choke = stepVal;
							else
								choke = counter;
						}
					}
				}*/

				/*	auto weapon = g_ctx.m_local->m_hActiveWeapon().Get();
					if (!weapon)
						return;

					if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
						return;

					bool should_fakelag = false;
					bool should_exploit = false;
					static bool new_factor = false;
					bool
						moving,
						inair,
						standing,
						shooting;

					int
						tick_times = ((int)(1.0f / g_csgo.m_globals()->m_interval_per_tick)) / 64,
						fltype = g_cfg.antiaim.fakelag_mode,
						fakelag_amountstand,
						fakelag_amountair,
						fakelag_amountshoot,
						fakelag_amountmove,
						maxchockeamount = 14,
						choke;

					if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND))
					{
						fakelag_amountair = g_cfg.antiaim.breaklagcomp ? BreakLagCompensation() : g_cfg.antiaim.fakelag_amountinair;
						inair = true,
						moving = false,
						standing = false,
						shooting = false;
					}
					else if (g_ctx.m_local->m_fFlags() & FL_ONGROUND && g_ctx.m_local->m_vecVelocity().Length() > 0)
					{
						fakelag_amountmove = g_cfg.antiaim.breaklagcomp ? BreakLagCompensation() : g_cfg.antiaim.fakelag_amountmoving;
						inair = false,
						moving = true,
						standing = false,
						shooting = false;
					}
					else if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
					{
						fakelag_amountshoot = g_cfg.antiaim.fakelag_amountshooting;
						inair = false,
							moving = false,
							standing = false,
							shooting = true;
					}
					else if (g_ctx.m_local->m_fFlags() & FL_ONGROUND && g_ctx.m_local->m_vecVelocity().Length() <= 0)
					{
						fakelag_amountstand = g_cfg.antiaim.fakelag_amountstanding;
						inair = false,
							moving = false,
							standing = true,
							shooting = false;
					}

					switch (fltype)
					{
					case 1:
					{
						if(standing)
							choke = min(fakelag_amountstand * tick_times, 14);
						else if(shooting)
							choke = min(fakelag_amountshoot * tick_times, 14);
						else if(moving)
							choke = min(fakelag_amountmove * tick_times, 14);
						else if(inair)
							choke = min(fakelag_amountair * tick_times, 14);
					}break;
					case 2:
					{
						if (standing)
							choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
						else if (shooting)
							choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
						else if (moving)
							choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
						else if (inair)
							choke = math::clamp(static_cast<int>(std::ceilf(69 / (g_ctx.m_local->m_vecVelocity().Length() * g_csgo.m_globals()->m_interval_per_tick))), 1, 14);
					}break;
					case 3:
					{
						if (standing)
							choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
						else if (shooting)
							choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
						else if (moving)
							choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
						else if (inair)
							choke = max(1, min((int)(fabs(g_ctx.m_local->m_vecVelocity().Length() / 80.f)), 5));
					}break;
					case 4:
					{
						if (!new_factor)
							break;

						static int FakelagFactor = 10;
						static int m_iOldHealth = 0;
						static int iChoked;
						if (m_iOldHealth != g_ctx.m_local->m_iHealth())
						{
							m_iOldHealth = g_ctx.m_local->m_iHealth();
							if (standing)
								choke = rand() % 7 + 6 + 1;
							else if (shooting)
								choke = rand() % 7 + 6 + 1;
							else if (moving)
								choke = rand() % 7 + 6 + 1;
							else if (inair)
								choke = rand() % 7 + 6 + 1;
							new_factor = false;
						}
					}break;
					case 5:
					{
						if (!new_factor)
							break;

						int min_packets = 0;
						Vector vel_p_t = Vector(g_ctx.m_local->m_vecVelocity() *  g_csgo.m_globals()->m_interval_per_tick);
						for (int i = 1; i <= 14; i++)
						{
							if (Vector(vel_p_t * i).Length() > 64)
							{
								min_packets = i;
								break;
							}
						}
						if (standing)
							choke = max(min_packets, fakelag_amountstand);
						else if (shooting)
							choke = max(min_packets, fakelag_amountshoot);
						else if (moving)
							choke = max(min_packets, fakelag_amountmove);
						else if (inair)
							choke = max(min_packets, fakelag_amountair);
					}break;

					case 6:
					{
						float speed = g_ctx.m_local->m_vecVelocity().Length2D() * g_csgo.m_globals()->m_interval_per_tick;
						int chokeAmmount = 0;
						if (speed < 1)//should be 66/maxChoke instead of 1 but meh
						{
							choke = maxchockeamount;
						}
						else
						{
							while (speed * chokeAmmount <= 70)
							{
								//While they aren't breaking lag comp
								chokeAmmount++;
							}
							if (chokeAmmount > maxchockeamount)
							{
								choke = maxchockeamount;
							}
							else
							{//Breaking lag comp, lets make some different
								//values in this area not just minumin
								int minChoke = chokeAmmount;
								int stepVal = minChoke;
								if (minChoke < g_cfg.antiaim.fakelag_amountstanding && standing)
								{
									stepVal = g_cfg.antiaim.fakelag_amountstanding;
								}
								if (minChoke < g_cfg.antiaim.fakelag_amountmoving && moving)
								{
									stepVal = g_cfg.antiaim.fakelag_amountmoving;
								}
								if (minChoke < g_cfg.antiaim.fakelag_amountinair && inair)
								{
									stepVal = g_cfg.antiaim.fakelag_amountinair;
								}
								if (minChoke < g_cfg.antiaim.fakelag_amountshooting && shooting)
								{
									stepVal = g_cfg.antiaim.fakelag_amountshooting;
								}

								static bool switcher = false;
								static int counter = minChoke;
								if (counter < minChoke)
									counter = minChoke;
								if (!g_csgo.m_clientstate()->m_nChokedCommands)
								{
									switcher = !switcher;
									counter++;
									if (counter > maxchockeamount)
										counter = maxchockeamount;
								}
								if (switcher)
									choke = stepVal;
								else
									choke = counter;
							}
						}
					}break;
					}

					if (g_cfg.antiaim.fakelag_enablers[FAKELAG_AIR].enabled) {
						if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND))
							should_fakelag = true;
						should_exploit = true;
					}

					if (g_ctx.m_local->m_vecVelocity().Length() > 0 && g_ctx.m_local->m_fFlags() & FL_ONGROUND) {
						if (g_cfg.antiaim.fakelag_enablers[FAKELAG_MOVING].enabled) {
							should_fakelag = true;
							should_exploit = true;
						}

						if (g_cfg.antiaim.fakelag_enablers[FAKELAG_SHOOTING].enabled) {
							if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
								should_fakelag = true;
							should_exploit = true;
						}

						if (g_cfg.antiaim.fakelag_enablers[FAKELAG_STANDING].enabled) {
							if (g_ctx.m_local->m_fFlags() & FL_ONGROUND && !g_ctx.m_local->m_vecVelocity().Length())
								should_fakelag = true;
							should_exploit = true;
						}

						if (!g_cfg.antiaim.fakelag_enablers[FAKELAG_SHOOTING].enabled) {
							if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
								should_fakelag = false;
						}
						if (g_csgo.m_engine()->IsVoiceRecording())	//big large fakelag voice fix

						{
							should_fakelag = false;
							should_exploit = false;
						}

						if (g_ctx.m_globals.fakewalking)
							return;
						// 		if (should_exploit && (g_cfg.astrowalk.enabled || g_cfg.fastwalk.enabled))
						// 		{
						// 			if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.astrowalk.key) && g_cfg.astrowalk.enabled) {
						// 				AstroWalk();
						// 			}
						// 			static bool gay;
						// 			gay = !gay;
						//
						// 			if (gay)
						// 				g_ctx.get_command()->m_tickcount += 128;
						// 			else
						// 				g_ctx.get_command()->m_tickcount -= 128;
						// 			if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.fastwalk.key) && g_cfg.fastwalk.enabled) {
						// 				pWalk();
						// 			}
						// 		}
								//g_cfg.fastwalk.key
						// 		if (choke >= maxchockeamount)
						// 			choke = maxchockeamount;

						if (should_fakelag) {
							static int choked = 0;

							if (choked > choke) {
								g_ctx.send_packet = true;
								new_factor = true;
								choked = 0;
							}
							else {
								g_ctx.send_packet = false;
								choked++;
							}
							g_ctx.m_globals.fakelagging = true;
						}
					}*/