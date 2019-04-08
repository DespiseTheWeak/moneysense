#include "..\..\hack\ragebot.h"
#include "..\sdk\animation_state.h"
#include "..\autowall\autowall.h"
#include <chrono>
#include <iostream>
#include <time.h>
#include "..\..\hack\microwave.h"

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
//who reads this is gay//fuck you got me, eins//xd
bool shouldnegativeflick = false;
int someautisticintthat_isusedby_pKoDa = 0;
static bool jitter = false;
bool antiaim::nade_out(CUserCmd *pUserCmd)
{
	auto m_weapon = g_ctx.m_local->m_hActiveWeapon().Get();

	if (m_weapon->is_grenade())
	{
		if (!m_weapon->m_bPinPulled() || g_ctx.get_command()->m_buttons & IN_ATTACK || g_ctx.get_command()->m_buttons & IN_ATTACK2)
		{
			float fThrowTime = m_weapon->m_fThrowTime();

			if ((fThrowTime > 0) && (fThrowTime < g_csgo.m_globals()->m_curtime))
				return true;
		}

		return false;
	}
	else
		return (g_ctx.get_command()->m_buttons & IN_ATTACK && m_weapon->can_fire());
}

static bool wasMoving = true;
static bool preBreak = false;
static bool shouldBreak = false;
static bool brokeThisTick = false;
static bool fake_walk = false;
static int chocked = 0;
static bool gaymode = false;
static bool doubleflick = false;
static bool has_broken = false;
bool is_broken;

void antiaim::create_move() {
	manual_aa::get().update();

	auto m_weapon = g_ctx.m_local->m_hActiveWeapon().Get();

	if (!m_weapon) {
		return;
	}
	if (m_weapon->is_non_aim()) {
		return;
	}
	if (g_ctx.get_command()->m_buttons & IN_USE) {
		return;
	}
	vec3_t next_angle = math::calculate_angle(g_ctx.m_local->get_eye_pos(), aimbot::get().target_data.aimspot);
	if (g_ctx.m_local->m_hActiveWeapon()->m_iItemDefinitionIndex() == (short)ItemDefinitionIndex::WEAPON_TASER)
		if (aimbot::get().can_zeus(next_angle))
			return;
	if (g_ctx.m_local->m_bGunGameImmunity() || g_ctx.m_local->m_fFlags() & FL_FROZEN || freeze_check)
		return;

	if (m_weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
	}
	else if (m_weapon->is_grenade()) {
		if (m_weapon->is_grenade() && m_weapon->m_fThrowTime() > 0.f)
			return;
	}
	else {
		if (m_weapon->get_csweapon_info()->type == WEAPONTYPE_KNIFE && ((g_ctx.get_command()->m_buttons & IN_ATTACK) || (g_ctx.get_command()->m_buttons & IN_ATTACK2))) {
			return;
		}
		else if ((g_ctx.get_command()->m_buttons & IN_ATTACK) && (m_weapon->m_iItemDefinitionIndex() != WEAPON_C4)) {
			if (m_weapon->can_fire())
				return;
		}
	}

	if (g_ctx.m_local->get_move_type() == MOVETYPE_NOCLIP || g_ctx.m_local->get_move_type() == MOVETYPE_LADDER)
		return;
	if (!g_ctx.m_globals.fakelagging)
		g_ctx.send_packet = g_ctx.get_command()->m_command_number % 2;

	g_ctx.get_command()->m_viewangles.x = get_pitch();
	if (!g_cfg.antiaim.use2ndfreestanding) {
		if (!manual_aa::get().run())
			g_ctx.get_command()->m_viewangles.y = g_ctx.send_packet ? get_fake() : get_real();
	}
	else
	{
		freestanding_spin();
	}
	//freestanding_spin
	if (!g_ctx.send_packet && g_cfg.antiaim.lby_breaker) {
		if (m_bBreakBalance && g_cfg.antiaim.break_adjustment) {
			bool negative = g_cfg.antiaim.lby_delta < 0;

			if (abs(g_cfg.antiaim.lby_delta) > 120) {
				float new_delta = g_cfg.antiaim.lby_delta / 2;

				g_ctx.get_command()->m_viewangles.y += negative ? -new_delta : new_delta;
			}
			else {
				float new_delta = g_cfg.antiaim.lby_delta - 180;

				g_ctx.get_command()->m_viewangles.y += negative ? -new_delta : new_delta;
			}
		}

		if (m_bBreakLowerBody) {
			if (!g_cfg.antiaim.ninesevenlbybreaker)
			{
				if (g_cfg.antiaim.p2cbreaker)
				{
					if (someautisticintthat_isusedby_pKoDa == 3) {
						g_ctx.get_command()->m_viewangles.y -= g_cfg.antiaim.lby_delta;
						someautisticintthat_isusedby_pKoDa = 0;
						g_ctx.m_globals.didp2cflick = true;
					}
					else
					{
						g_ctx.get_command()->m_viewangles.y += g_cfg.antiaim.lby_delta;
						someautisticintthat_isusedby_pKoDa++;
						g_ctx.m_globals.didp2cflick = false;
					}
				}
				else
				{
					g_ctx.get_command()->m_viewangles.y += g_cfg.antiaim.lby_delta;
				}
			}
			else if (g_cfg.antiaim.ninesevenlbybreaker)
			{
				if (g_cfg.antiaim.p2cbreaker)
				{
					if (someautisticintthat_isusedby_pKoDa == 3) {
						g_ctx.get_command()->m_viewangles.y -= 119.995;
						someautisticintthat_isusedby_pKoDa = 0;
						g_ctx.m_globals.didp2cflick = true;
					}
					else
					{
						g_ctx.m_globals.didp2cflick = false;
						g_ctx.get_command()->m_viewangles.y += 119.995;
						someautisticintthat_isusedby_pKoDa++;
					}
				}
				else
				{
					g_ctx.get_command()->m_viewangles.y -= 119.95f;//bc large 979 mem
				}
			}

			if (g_cfg.antiaim.pitch_flick) g_ctx.get_command()->m_viewangles.x = -89;
			fridge::ConsolePrint("ANTIAIM: Broke LBY \n");
		}
	}
}

void antiaim::update_lowerbody_breaker() {
	if (!g_ctx.m_local->m_iHealth())
		return;

	float
		server_time = util::server_time(),
		speed = animation_state::get().animstate()->m_velocity;

	if (speed > 36) {
		m_flNextBodyUpdate = server_time + 0.22;
		m_flLastMoveTime = util::server_time();
	}

	m_bBreakLowerBody = false;
	m_bBreakBalance = false;

	if (m_flNextBodyUpdate < server_time) {
		m_flNextBodyUpdate = server_time + 1.1;
		m_bBreakLowerBody = true;
	}

	if (m_flNextBodyUpdate - g_csgo.m_globals()->m_interval_per_tick < server_time)
		m_bBreakBalance = true;

	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND)) {
		m_bBreakLowerBody = false;
		m_bBreakBalance = false;
	}
}

float antiaim::at_target() {
	int cur_tar = 0;
	float last_dist = 999999999999.0f;

	for (int i = 0; i < g_csgo.m_globals()->m_maxclients; i++) {
		auto m_entity = static_cast<entity_t*>(g_csgo.m_entitylist()->GetClientEntity(i));

		if (!m_entity || m_entity == g_ctx.m_local)
			continue;

		if (!m_entity->is_player())
			continue;

		auto m_player = (player_t*)m_entity;
		if (!m_player->IsDormant() && m_player->is_alive() && m_player->m_iTeamNum() != g_ctx.m_local->m_iTeamNum()) {
			float cur_dist = (m_entity->m_vecOrigin() - g_ctx.m_local->m_vecOrigin()).Length();

			if (!cur_tar || cur_dist < last_dist) {
				cur_tar = i;
				last_dist = cur_dist;
			}
		}
	}

	if (cur_tar) {
		auto m_entity = static_cast<player_t*>(g_csgo.m_entitylist()->GetClientEntity(cur_tar));
		if (!m_entity) {
			return g_ctx.get_command()->m_viewangles.y;
		}

		Vector target_angle = math::calculate_angle(g_ctx.m_local->m_vecOrigin(), m_entity->m_vecOrigin());
		return target_angle.y;
	}

	return g_ctx.get_command()->m_viewangles.y;
}
int GetFPS()
{
	static int fps = 0;
	static int count = 0;
	using namespace std::chrono;
	auto now = high_resolution_clock::now();
	static auto last = high_resolution_clock::now();
	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000)
	{
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

float antiaim::get_pitch() {
	int index;
	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND)) {
		index = g_cfg.antiaim.pitch[2];
	}
	else {
		index = g_cfg.antiaim.pitch[0];

		if (g_ctx.m_local->m_vecVelocity().Length() > 0.1f)
			index = g_cfg.antiaim.pitch[1];
	}

	float pitch = g_ctx.get_command()->m_viewangles.x;
	if (index)
	{
		switch (index) {
		case 1:
			pitch = 89.0f;
			break;
		case 2:
			pitch = -89.0f;
			break;
		case 3:
			pitch = 89.0f;
			break;
		}
		if (g_ctx.m_local->get_animation_state()->m_bInHitGroundAnimation && g_cfg.antiaim.hittinggroundpitch) {/// change pitch while we supress
			if (g_ctx.m_local->get_animation_state()->m_flHeadHeightOrOffsetFromHittingGroundAnimation) {
				pitch = -10;
			}
		}
	}
	return pitch;
}
enum fakeangles {
	None,
	Static,
	Jitter,
	Spin,
	Shuffle,
	JitterBetweenLargestDelta,
	oneeighttreehouse
};
float antiaim::get_fake(/*what a fucking meme / nya */) {
	int
		index,
		base_angle,
		offset;

	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND)) {
		index = g_cfg.antiaim.fake_yaw[2];
		base_angle = g_cfg.antiaim.base_angle[2];
		offset = g_cfg.antiaim.fake_yaw_offset[2];
	}
	else {
		index = g_cfg.antiaim.fake_yaw[0];
		base_angle = g_cfg.antiaim.base_angle[0];
		offset = g_cfg.antiaim.fake_yaw_offset[0];

		if (g_ctx.m_local->m_vecVelocity().Length() > 36.f) {
			index = g_cfg.antiaim.fake_yaw[1];
			base_angle = g_cfg.antiaim.base_angle[1];
			offset = g_cfg.antiaim.fake_yaw_offset[1];
		}
	}
	float yaw = g_ctx.get_command()->m_viewangles.y;
	if (index)
	{
		yaw = g_ctx.m_globals.real_angles.y;

		switch (index) {
		case 0:
		{
			return g_ctx.m_globals.real_angles.y;
		}break;
		case Static:
			return g_ctx.m_globals.real_angles.y + g_ctx.m_local->get_max_desync_delta();
			break;
		case Shuffle: {
			return (g_ctx.m_globals.real_angles.y + g_ctx.m_local->get_max_desync_delta()) - RandomInt(g_cfg.antiaim.desyncjitterrange, -g_cfg.antiaim.desyncjitterrange);//make the 59 a user input
		}break;
		case Spin: {
			static int Tick = -g_ctx.m_local->get_max_desync_delta();

			Tick += 4;

			if (Tick > (g_ctx.m_local->get_max_desync_delta() - 1))
			{
				Tick = -g_ctx.m_local->get_max_desync_delta();
			}

			return (g_ctx.m_globals.real_angles.y + Tick);
		}break;
		case Jitter: {
			return (g_ctx.m_globals.real_angles.y + g_ctx.m_local->get_max_desync_delta()) - RandomInt(-g_cfg.antiaim.desyncjitterrange, g_cfg.antiaim.desyncjitterrange);//make the 59 a user input
		}break;
		case JitterBetweenLargestDelta: {
			return g_ctx.m_globals.real_angles.y + math::random_float(-g_ctx.m_local->get_max_desync_delta(), g_ctx.m_local->get_max_desync_delta());
		}break;
		case oneeighttreehouse:
		{
			return ((g_ctx.m_globals.real_angles.y + g_ctx.m_local->get_max_desync_delta()) + 140 + rand() % 123) - rand() % 20;
		}break;
		}
		yaw += offset;
		if (yaw > g_ctx.m_local->get_max_desync_delta()) {
			yaw = g_ctx.m_local->get_max_desync_delta();
		}
		else if (yaw < -g_ctx.m_local->get_max_desync_delta())
		{
			yaw = -g_ctx.m_local->get_max_desync_delta();
		}
		if (m_bBreakLowerBody)
			yaw = g_ctx.m_globals.real_angles.y;
	}
	//return yaw;
}
enum
{
	Zero,
	One,
	Two,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,
	DEVIL
};

float antiaim::get_real() {
	int
		index,
		base_angle,
		offset;

	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND)) {
		index = g_cfg.antiaim.yaw[2];
		base_angle = g_cfg.antiaim.base_angle[2];
		offset = g_cfg.antiaim.yaw_offset[2];
	}
	else {
		index = g_cfg.antiaim.yaw[0];
		base_angle = g_cfg.antiaim.base_angle[0];
		offset = g_cfg.antiaim.yaw_offset[0];

		if (g_ctx.m_local->m_vecVelocity().Length() > 36.f) {
			index = g_cfg.antiaim.yaw[1];
			base_angle = g_cfg.antiaim.base_angle[1];
			offset = g_cfg.antiaim.yaw_offset[1];
		}
	}

	float yaw = g_ctx.get_command()->m_viewangles.y;

	if (index) {
		yaw = 0.0f;

		if (base_angle == 1) {
			vec3_t angles; g_csgo.m_engine()->GetViewAngles(angles);
			yaw = angles.y;
		}

		if (base_angle == 2)
			yaw = at_target();

		switch (index) {
		case 1:
			yaw += 90.0f;
			break;
		case 2:
			yaw += 180;
			break;
		case 3:
			yaw += g_csgo.m_globals()->m_curtime * 420.0f;
			break;
		case 4:
			yaw += 180 + (math::random_float(-50, 50));
			break;
		case 5: {
			vec3_t angles; g_csgo.m_engine()->GetViewAngles(angles);
			yaw = angles.y;
			break;
		}
		case 6:
			yaw = g_ctx.m_local->m_flLowerBodyYawTarget();
			break;
		case 7:
			yaw = freestanding();
			break;
		case 9:
			yaw = g_ctx.m_local->m_flLowerBodyYawTarget() + g_cfg.antiaim.lby_delta;
			break;
		}
		float LBYDelta = g_ctx.get_command()->m_viewangles.y - g_ctx.m_local->m_flLowerBodyYawTarget();
		if (fabsf(LBYDelta) > 35.f || fabsf(LBYDelta) < -35.f)
			antiaim::breakinglby = true;
		else
			antiaim::breakinglby = false;

		yaw += offset;
	}
	return yaw;
}

void antiaim::get_unpredicted_velocity() {
	if (!g_ctx.m_local)
		return;

	unpredicted_velocity = g_ctx.m_local->m_vecVelocity();
}
void AngleVectorsANtiAim(const Vector &angles, Vector *forward)//most of this is also defined in resolver so i thought why dont make a resolver out of it
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}
void NormalizeNum(Vector &vIn, Vector &vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0) {
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}
float fov_ent(Vector ViewOffSet, Vector View, player_t* entity, int hitbox)
{
	const float MaxDegrees = 180.0f;
	Vector Angles = View, Origin = ViewOffSet;
	Vector Delta(0, 0, 0), Forward(0, 0, 0);
	Vector AimPos = entity->hitbox_position(hitbox);
	AngleVectorsANtiAim(Angles, &Forward);
	VectorSubtract(AimPos, Origin, Delta);
	NormalizeNum(Delta, Delta);
	float DotProduct = Forward.Dot(Delta);
	return (acos(DotProduct) * (MaxDegrees / PI));
}
int closest()
{
	int index = -1;
	float lowest_fov = 180.f;

	if (!g_ctx.m_local)
		return -1;

	if (!g_ctx.m_local->is_alive())
		return -1;

	Vector local_position = g_ctx.m_local->GetAbsOrigin() + g_ctx.m_local->m_vecViewOffset();
	Vector angles;
	g_csgo.m_engine()->GetViewAngles(angles);
	for (int i = 1; i <= g_csgo.m_globals()->m_maxclients; i++)
	{
		auto entity = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));

		if (!entity || entity->m_iHealth() <= 0 || entity->m_iTeamNum() == g_ctx.m_local->m_iTeamNum() || entity->IsDormant() || entity == g_ctx.m_local)
			continue;

		float fov = fov_ent(local_position, angles, entity, 0);
		if (fov < lowest_fov)
		{
			lowest_fov = fov;
			index = i;
		}
	}
	return index;
}

void antiaim::freestanding_spin()
{
	float range = g_cfg.antiaim.freestandjitterrange; static int Ticks = 0; if (g_csgo.m_inputsys()->IsButtonDown(ButtonCode_t::MOUSE_LEFT)) return;
	bool no_active = true;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	static float hold = 0.f;
	Vector besthead;
	float opposite = 0.f;
	auto leyepos = g_ctx.m_local->m_vecOrigin() + g_ctx.m_local->m_vecViewOffset();
	auto headpos = g_ctx.m_local->hitbox_position(0);
	auto origin = g_ctx.m_local->m_vecOrigin();
	auto checkWallThickness = [&](player_t* pPlayer, Vector newhead) -> float {
		Vector endpos1, endpos2;  Vector eyepos = pPlayer->m_vecVelocity() + pPlayer->m_vecViewOffset();
		Ray_t ray; ray.Init(newhead, eyepos); CTraceFilterSkipTwoEntities filter(pPlayer, g_ctx.m_local);
		trace_t trace1, trace2; g_csgo.m_trace()->TraceRay(ray, MASK_SHOT | MASK_SHOT_BRUSHONLY, &filter, &trace1);
		if (trace1.DidHit()) endpos1 = trace1.endpos; else return 0.f;
		ray.Init(eyepos, newhead); g_csgo.m_trace()->TraceRay(ray, MASK_SHOT | MASK_SHOT_BRUSHONLY, &filter, &trace1);
		if (trace2.DidHit()) endpos2 = trace2.endpos;  float add = newhead.DistTo(eyepos) - leyepos.DistTo(eyepos) + 3.f;
		return endpos1.DistTo(endpos2) + add / 3; };  int index = closest(); static player_t* entity;
		if (!g_ctx.m_local->is_alive()) { hold = 0.f; }  if (index != -1) entity = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(index));
		if (!entity || entity->IsDormant()) { g_ctx.get_command()->m_viewangles.y -= 180.f; return; }  float radius = Vector(headpos - origin).Length2D();
		float server_time = g_ctx.m_local->m_nTickBase() * g_csgo.m_globals()->m_interval_per_tick; double speed = 250;
		double exe = fmod(static_cast<double>(server_time)*speed, range * 2); if (index == -1) { no_active = true; }
		else {
			for (float besthead = 0; besthead < 7; besthead += 0.1)
			{
				Vector newhead(radius * cos(besthead) + leyepos.x, radius * sin(besthead) + leyepos.y, leyepos.z);
				float totalthickness = 0.f;
				no_active = false;
				totalthickness += checkWallThickness(entity, newhead);
				if (totalthickness > highestthickness)
				{
					highestthickness = totalthickness; opposite = besthead - 180; bestrotation = besthead;
				}
			}
		}
		if (no_active)
		{
			g_ctx.get_command()->m_viewangles.y -= (180 - range) + static_cast<float>(exe); return;
		}
		else {
			if (g_ctx.send_packet)
			{
				jitter = !jitter;
				if (jitter)
					g_ctx.get_command()->m_viewangles.y = RAD2DEG(bestrotation) - g_ctx.m_local->get_max_desync_delta();
				else g_ctx.get_command()->m_viewangles.y = RAD2DEG(bestrotation) + 50.f;
			}
			else
			{
				g_ctx.get_command()->m_viewangles.y = RAD2DEG(bestrotation - range) + static_cast<float>(exe);
			}
		}
}

float antiaim::freestanding() {
	bool change = false;
	float servertime = util::server_time();
	float nextchange = servertime + g_cfg.antiaim.freestanddelay;
	if (nextchange < servertime) {
		nextchange = servertime + g_cfg.antiaim.freestanddelay;
		change = true;
	}
	else
		change = false;

	if (nextchange - g_csgo.m_globals()->m_interval_per_tick < servertime)
	{
		change = true;
	}
	else
		change = false;
	enum {
		back,
		right,
		left,
		forward,
		jitter,
		right2,
		left2,
	};

	vec3_t view_angles;
	g_csgo.m_engine()->GetViewAngles(view_angles);

	static constexpr int damage_tolerance = 30;// make this a variable that the user can enter

	std::vector< player_t * > enemies;

	auto get_target = [&]() -> player_t * {
		player_t * target = nullptr;
		float best_fov = 360.f;

		for (int id = 1; id <= g_csgo.m_globals()->m_maxclients; id++) {
			auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(id));

			if (!e->valid(true, !g_cfg.antiaim.freestanddormant))
				continue;

			float fov = math::get_fov(view_angles, math::calculate_angle(g_ctx.m_local->get_eye_pos(), e->m_vecOrigin()));

			if (fov < best_fov) {
				target = e;
				best_fov = fov;
			}

			enemies.push_back(e);
		}

		return target;
	};

	player_t * e = get_target();
	int ServerTime = (float)g_ctx.m_local->m_nTickBase() * g_csgo.m_globals()->m_interval_per_tick;
	int value = ServerTime % 2;

	//prepare all values for pj
	int jitterrange = 0;//actualy outputangles but nvm
	int jittermode = 0;
	static bool jitterleaver = false;

	bool shouldpanic = false;
	//set values to a value
	if (g_cfg.antiaim.panicjitter)
	{
		jitterleaver = jitterleaver;
		shouldpanic = true;
		if (g_cfg.antiaim.panicjittermode == 0)//away from desync IMPORTANT: never flip angles into desync angles!!
		{
			int desyncangles = g_ctx.m_globals.fake_angles.y;
			int olddesyncangles = 0;
			if (desyncangles != olddesyncangles)
			{
				olddesyncangles = desyncangles;
			}
			if (jitterleaver) {
				jitterrange = RandomInt(-g_cfg.antiaim.panicjitterrange, g_cfg.antiaim.panicjitterrange);
			}
			else
				jitterrange = RandomInt(g_cfg.antiaim.panicjitterrange, -g_cfg.antiaim.panicjitterrange);

			if (jitterrange == olddesyncangles || jitterrange == desyncangles)
			{
				jitterrange = -10;
			}
		}
		else if (g_cfg.antiaim.panicjittermode == 1)//
		{
			int lby = g_ctx.m_local->m_flLowerBodyYawTarget();

			jitterrange = RandomInt((lby - g_cfg.antiaim.panicjitterrange), (lby + g_cfg.antiaim.panicjitterrange));
		}
		else if (g_cfg.antiaim.panicjittermode == 2)
		{
			jitterrange = math::random_float(math::random_float(-g_cfg.antiaim.panicjitterrange, -g_cfg.antiaim.panicjitterrange / 2), math::random_float(g_cfg.antiaim.panicjitterrange, g_cfg.antiaim.panicjitterrange * 2));
		}
	}
	static int Ticks = 120;
	if (!e)
	{
		vec3_t yaw;
		g_csgo.m_engine()->GetViewAngles(yaw);

		Ticks += 4; //change the 4 for the speed of the spin

		if (Ticks > 240)
			Ticks = 120;

		return (yaw.y - Ticks); // 180z using ticks owo
	}

	auto calculate_damage = [&](vec3_t point) -> int {
		int damage = 0;
		for (auto& enemy : enemies)
			damage += autowall::get().calculate_return_info(enemy->get_eye_pos(), point, enemy, g_ctx.m_local, 1).m_damage;

		return damage;
	};

	auto rotate_and_extend = [](vec3_t position, float yaw, float distance) -> vec3_t {
		vec3_t direction;
		math::angle_vectors(vec3_t(0, yaw, 0), direction);

		return position + (direction * distance);
	};

	vec3_t
		head_position = g_ctx.m_local->get_eye_pos(),
		at_target = math::calculate_angle(g_ctx.m_local->m_vecOrigin(), e->m_vecOrigin());

	float angles[8] = {
		at_target.y + 180.f,
		at_target.y - 70.f,
		at_target.y + 70.f,
		at_target.y,
		at_target.y + 180 + (math::random_float(g_cfg.antiaim.freestandjitterrange, -g_cfg.antiaim.freestandjitterrange)),
		at_target.y - 135.f,
		at_target.y + 135.f,
	};

	vec3_t head_positions[7] = {
		rotate_and_extend(head_position, at_target.y + 180.f, 17.f),
		rotate_and_extend(head_position, at_target.y - 70.f, 17.f),
		rotate_and_extend(head_position, at_target.y + 70.f, 17.f),
		rotate_and_extend(head_position, at_target.y, 17.f),
		rotate_and_extend(head_position, at_target.y + 180 + math::random_float(g_cfg.antiaim.freestandjitterrange, -g_cfg.antiaim.freestandjitterrange), 17.f),
		rotate_and_extend(head_position, at_target.y - 135.f, 17.f),
		rotate_and_extend(head_position, at_target.y + 135.f, 17.f)
	};

	int damages[7] = {
		calculate_damage(head_positions[back]),
		calculate_damage(head_positions[right]),
		calculate_damage(head_positions[left]),
		calculate_damage(head_positions[forward]),
		calculate_damage(head_positions[jitter]),
		calculate_damage(head_positions[right2]),
		calculate_damage(head_positions[left2]),
	};
	if (g_ctx.m_globals.missed_shots[e->EntIndex()] == 1 || g_ctx.m_globals.missed_shots[e->EntIndex()] == 3 || g_ctx.m_globals.missed_shots[e->EntIndex()] == 5 || g_ctx.m_globals.missed_shots[e->EntIndex()] > 6)
		change = true;
	else
		change = false;

	if (shouldpanic && g_cfg.antiaim.panicjitterhp > g_ctx.m_local->m_iHealth())
	{
		if (jitterleaver)
			return(at_target.y + 180 + jitterrange);
		else
			return(at_target.y + 180 - jitterrange);
	}

	if (g_cfg.antiaim.jitterbetweensecondbestangle)	//large skeet feature lol		it just confuses some resolvers sometimes		nyani
	{
		if (change)
		{
			if (damages[right2] > damage_tolerance && damages[left2] > damage_tolerance)
			{
				if (damages[forward] > damage_tolerance)
				{
					if (damages[back] > damage_tolerance || damages[jitter] > damage_tolerance)
					{
						if (damages[back] < damage_tolerance) {
							g_ctx.m_globals.headsavenotshure = true; g_ctx.m_globals.headsave = false;
							return angles[jitter];
						}
						else
						{
							if (shouldpanic)
							{
								if (jitterleaver)
									return(at_target.y + 180 + jitterrange);
								else
									return(at_target.y + 180 - jitterrange);
							}
							else
							{
								if (damages[forward] < damage_tolerance) {
									g_ctx.m_globals.headsavenotshure = true; g_ctx.m_globals.headsave = false;
									return angles[forward];
								}
								else {
									return angles[back];
									g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = false;
								}
							}
						}
					}
					else
					{
						g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = true;
						return angles[jitter];
					}
				}
				else {
					g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = false;
					if (shouldpanic)
					{
						if (jitterleaver)
							return(at_target.y + jitterrange);
						else
							return(at_target.y - jitterrange);
					}
					else
					{
						return angles[forward];
					}
				}
			}
		}
		else
		{
			if (damages[right] > damage_tolerance && damages[right] > damage_tolerance)
			{
				if (damages[forward] > damage_tolerance)
				{
					if (damages[back] > damage_tolerance && damages[jitter] > damage_tolerance)
					{
						return angles[jitter];
					}
					else
					{
						if (damages[back] > damage_tolerance)
						{
							if (shouldpanic)
							{
								if (jitterleaver)
									return(angles[jitter] + jitterrange);
								else
									return(angles[jitter] - jitterrange);
							}
							else
							{
								return angles[jitter];
							}
						}
						else {
							return angles[back];
						}
					}
				}
				else {
					if (shouldpanic)
					{
						if (jitterleaver)
							return(at_target.y + jitterrange);
						else
							return(at_target.y - jitterrange);
					}
					else
					{
						return angles[forward];
					}
				}
				g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = false;
			}
		}
	}
	else {
		if (damages[right] > damage_tolerance && damages[left] > damage_tolerance)
		{
			if (shouldpanic)
			{
				if (jitterleaver)
					return(at_target.y + 180 + jitterrange);
				else
					return(at_target.y + 180 - jitterrange);
			}
			else
			{
				return angles[back];
			}
			g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = false;
		}
	}
	//return angles[back];
	if (at_target.x > 15.f)	//just replace the 15 with an float given by user
		return angles[jitter];		//autobackwards here c; large mutiny feature that is useless oof

	if (damages[right] == damages[left]) {
		auto trace_to_end = [](vec3_t start, vec3_t end) -> vec3_t {
			trace_t trace;
			CTraceFilterWorldOnly filter;
			Ray_t ray;

			ray.Init(start, end);
			g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
			g_ctx.m_globals.headsavenotshure = true; g_ctx.m_globals.headsave = false;
			return trace.endpos;
		};

		vec3_t
			trace_right_endpos = trace_to_end(head_position, head_positions[right]),
			trace_left_endpos = trace_to_end(head_position, head_positions[left]),
			trace_right_endpos2 = trace_to_end(head_position, head_positions[right2]),
			trace_left_endpos2 = trace_to_end(head_position, head_positions[left2]);

		Ray_t ray;
		trace_t trace;
		CTraceFilterWorldOnly filter;

		ray.Init(trace_right_endpos, e->get_eye_pos());
		g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
		float distance_1 = (trace.startpos - trace.endpos).Length();

		ray.Init(trace_left_endpos, e->get_eye_pos());
		g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
		float distance_2 = (trace.startpos - trace.endpos).Length();

		ray.Init(trace_right_endpos2, e->get_eye_pos());
		g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
		float distance_3 = (trace.startpos - trace.endpos).Length();

		ray.Init(trace_left_endpos2, e->get_eye_pos());
		g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
		float distance_4 = (trace.startpos - trace.endpos).Length();
		if (g_cfg.antiaim.jitterbetweensecondbestangle)
		{
			if (change)
			{
				if (fabs(distance_3 - distance_4) > 20.f) {
					g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = true;
					return (distance_3 < distance_4) ? angles[right2] : angles[left2];
				}
				else {
					g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = false;
					return angles[jitter];
				}
			}
			else
			{
				if (fabs(distance_1 - distance_2) > 15.f) {
					g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = true;
					return (distance_1 < distance_2) ? angles[right] : angles[left];
				}
				else {
					g_ctx.m_globals.headsavenotshure = true; g_ctx.m_globals.headsave = false;
					if (shouldpanic)
					{
						if (jitterleaver)
							return(at_target.y + 180 + jitterrange);
						else
							return(at_target.y + 180 - jitterrange);
					}
					else
					{
						return angles[back];
					}
				}
			}
		}
		else {
			if (fabs(distance_1 - distance_2) > 15.f) {
				return (distance_1 < distance_2) ? angles[right] : angles[left];
				g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = true;
			}
			else {
				return angles[back];
				g_ctx.m_globals.headsavenotshure = true; g_ctx.m_globals.headsave = false;
			}
		}
	}
	else {
		g_ctx.m_globals.headsavenotshure = false; g_ctx.m_globals.headsave = true;
		return (damages[right] < damages[left]) ? angles[right] : angles[left];
	}
}