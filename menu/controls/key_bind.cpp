#include "..\cmenu.hpp"
C_KeyBind::C_KeyBind(std::string n_label, ButtonCode_t * n_key) {
	parent = g_cfg.menu.last_group;
	label = n_label;
	key = n_key;
	area.h = 20;
	if (n_label != "") {
		height_offset = 14;
		area.h += 14;
	}

	parent->add_control(this);
}
/*
	static float rainbow;
	rainbow += g_cfg.misc.rainbowspeed * 0.0001;
	if (rainbow > 1.f)
		rainbow = 0.f;
	 Color::FromHSB(rainbow, 1.f, 1.f)
			if (g_cfg.misc.RainbowBar)
			{
			}
	else
	{
	}
	*/
void C_KeyBind::draw() {
	rect_t n_area = rect_t(
		area.x + 60,
		area.y + height_offset,
		area.w,
		0
	);

	if (label != "-")
		render::get().text(fonts[TAHOMA12], area.x, area.y, Color::White, HFONT_CENTERED_NONE, label.c_str());

	float
		left_brack_size = render::get().text_width(fonts[TAHOMA12], "["),
		right_brack_size = render::get().text_width(fonts[TAHOMA12], "]");

	std::string to_render = g_csgo.m_inputsys()->ButtonCodeToString("[", *key);
	std::transform(to_render.begin(), to_render.end(), to_render.begin(), ::tolower);

	if (*key <= BUTTON_CODE_NONE) {
		to_render = "[...]";
	}

	if (taking_input) {
		to_render = "[.....]";
	}

	Color text_color = taking_input ? g_cfg.menu.menu_theme[0] : Color(153, 153, 153);
	render::get().text(fonts[TAHOMA12], area.x + 60 + 10, area.y +	19, text_color, HFONT_CENTERED_Y, to_render.c_str());
}
void C_KeyBind::update() {
	if (!taking_input) {
		return;
	}
	parent->focus_control(this);
	for (int i = 0; i < 255; i++) {
		if (menu::get().key_press(i)) {
			*key = g_csgo.m_inputsys()->VirtualKeyToButtonCode(i);
			switch (i) {
			case 1: *key = MOUSE_LEFT; break;
			case 2: *key = MOUSE_RIGHT; break;
			case 4: *key = MOUSE_MIDDLE; break;
			case 5: *key = MOUSE_4; break;
			case 6: *key = MOUSE_5; break;
			}
			if (*key == KEY_ESCAPE) {
				*key = BUTTON_CODE_NONE;
				taking_input = false;
				parent->reset_block(); return;
			}
			taking_input = false;
			parent->reset_block(); return;
		}
	}
}
void C_KeyBind::click() {
	if (!taking_input) {
		taking_input = true;
	}
}