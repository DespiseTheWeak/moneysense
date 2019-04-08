#include "world_esp.h"
player_t* ara;
void worldesp::paint_traverse() {
	if (!g_ctx.available())
		return;

	for (int i = 1; i < g_csgo.m_entitylist()->GetHighestEntityIndex(); i++) {
		auto e = static_cast<entity_t *>(g_csgo.m_entitylist()->GetClientEntity(i));

		if (!e)
			continue;

		if (e->IsDormant())
			continue;

		Box box; if (!util::get_bbox_world(e, box))
			continue;

		if (g_cfg.esp.projectiles)
			grenades((weapon_t *)e, box);

		if (g_cfg.esp.dropped_weapons)
			weapons((weapon_t *)e, box);

		if (g_cfg.esp.espbomb)
			PlantedBomb((player_t*)ara);
	}
}
void worldesp::PlantedBomb(player_t* pt_hook)
{
	if (!g_cfg.esp.espbomb)
		return;
}

void worldesp::DrawZeusRange() {
	if (!g_csgo.m_engine()->IsConnected() || !g_csgo.m_engine()->IsInGame())
		return;

	if (!g_ctx.m_local) return;

	if (!*(bool*)((uintptr_t)g_csgo.m_input() + 0xAD))
		return;

	if (!g_cfg.esp.zeusrange)
		return;

	auto weapon = g_ctx.m_local->m_hActiveWeapon().Get();
	if (weapon->m_iItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_TASER)
		return;
#define M_PI 3.14159265358979323846
	float step = M_PI * 2.0 / 1023;
	float rad = weapon->get_csweapon_info()->range;
	Vector origin = g_ctx.m_local->get_eye_pos();

	static double rainbow;

	Vector screenPos;
	static Vector prevScreenPos;

	for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
	{
		Vector pos(rad * cos(rotation) + origin.x, rad * sin(rotation) + origin.y, origin.z);

		trace_t trace;
		CTraceFilter filter;

		filter.pSkip = g_ctx.m_local;


		g_csgo.m_trace()->TraceRay(Ray_t(origin, pos), MASK_SHOT_BRUSHONLY, &filter, &trace);


		if (g_csgo.m_debugoverlay()->ScreenPosition(trace.endpos, screenPos))
			continue;

		if (!prevScreenPos.IsZero() && !screenPos.IsZero() && screenPos.DistTo(Vector(-107374176, -107374176, -107374176)) > 3.f&& prevScreenPos.DistTo(Vector(-107374176, -107374176, -107374176)) > 3.f) {
			rainbow += 0.00001;
			if (rainbow > 1.f)
				rainbow = 0;

			Color color = Color::FromHSB(rainbow, 1.f, 1.f);
			render::get().line(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color);
			//DrawLine(prevScreenPos.x, prevScreenPos.y + 1, screenPos.x, screenPos.y + 1, color);
			//DrawLine(Vector2D(prevScreenPos.x, prevScreenPos.y - 1), Vector2D(screenPos.x, screenPos.y - 1), color);
		}
		prevScreenPos = screenPos;
	}
}

void worldesp::grenades(weapon_t * m_nade, Box box) {
	const model_t* model = m_nade->GetModel();
	if (!model)
		return;

	studiohdr_t* hdr = g_csgo.m_modelinfo()->GetStudiomodel(model);
	if (!hdr)
		return;

	std::string entity_name = hdr->szName;

	switch (m_nade->GetClientClass()->m_ClassID) {
	case 9:
		entity_name = entity_name[16] == 's' ? "i" : "j";
		break;
	case 134:
		entity_name = "K";
		break;
	case 98:
		entity_name = "I";
		break;
	case 41:
		entity_name = "m";
		break;
	default:
		return;
	}

	Color color = g_cfg.esp.projectiles_color;

	//render::get().rect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(255, 255, 255));
	//render::get().rect(box.x, box.y, box.w, box.h, Color(255, 255, 255));
	//render::get().rect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(255, 255, 255));

	render::get().text(fonts[GRENADEICONS], box.x + (box.w / 5), box.y, color, HFONT_CENTERED_X, entity_name.c_str());
}
void worldesp::weapons(weapon_t * e, Box box) {

	Box n_box = {
	box.x + 1,
	box.y + box.h + 3,
	box.w - 1,
	2
	};

	if (e->GetClientClass()->m_ClassID != (int)ClassId::CBaseWeaponWorldModel && ((strstr(e->GetClientClass()->m_pNetworkName, "Weapon") || e->GetClientClass()->m_ClassID == (int)ClassId::CDEagle || e->GetClientClass()->m_ClassID == (int)ClassId::CAK47)))
	{
		char * icon = e->get_name();

		Color color = g_cfg.esp.dropped_weapons_color;
		auto w = render::get().text_width(fonts[SUBTABWEAPONS], icon);

		render::get().rect_filled(n_box.x - 1, n_box.y - 1, box.w + 2, 4, Color(0, 150, 255));
		render::get().rect(n_box.x - 1, n_box.y - 1, box.w + 2, 4, Color(0, 0, 0, 255));
		render::get().rect_filled(n_box.x, n_box.y, box.w, 2, Color(130, 130, 130, 130));
		render::get().text(fonts[SUBTABWEAPONS], box.x + (box.w / 2), box.y, color, HFONT_CENTERED_X, icon);
	}
}