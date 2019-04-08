#include "../utils/draw_manager.h"
#include "../utils/csgo.hpp"
#include "../sdk/structs.hpp"
#include "../utils/singleton.h"
#include <cstring>
#include <vector>
/* 

struct Rect
{
	Vec2 min;
	Vec2 max;
	Rect() { min = Vec2(); max = Vec2(); };
	Rect(Vec2 _min, Vec2 _max) { min = _min; max = _max; };
};








class Message
{
private:
	bool Enable = false;

	float OfstLayWhite = 0;
	float OfstLayBlack = 0;
	int NumTicks = 100000;

	Vec2 MsgTextSize = Vec2(0, 0);

	string MessageText;
public:

	Message()
	{
		MessageText = "";
	}

	void Start(string Msg)
	{
		MessageText = Msg;


		MsgTextSize = GP_Render->CalcTextSize(MessageText.c_str(), GP_Render->SzFonts[20]);

		OfstLayWhite = 0;
		OfstLayBlack = 0;
		NumTicks = 0;

		Enable = true;
	}

#define MSG_SPEED_X 0.0999f

#define MSG_SIZE_WHITE_X 200
#define MSG_SIZE_BLACK_X 193

#define MSG_SIZE_Y 55
	void Show()
	{
		if (Enable)
		{
			int posX = CGlobal::iScreenWidth;
			int posY = 50;

			float DeltaSpeed = 15;

			if (NumTicks < 10000)
				NumTicks += (DeltaSpeed > 1000 ? 50 : DeltaSpeed);
			else
				Enable = false;

			if (NumTicks < 1900)
			{
				if (OfstLayWhite < MSG_SIZE_WHITE_X)
					OfstLayWhite += (MSG_SIZE_WHITE_X - OfstLayWhite) * MSG_SPEED_X;

				if (OfstLayWhite > MSG_SIZE_WHITE_X)
					OfstLayWhite = MSG_SIZE_WHITE_X;

				if (OfstLayWhite > MSG_SIZE_WHITE_X / 1.09f)
					if (OfstLayBlack < MSG_SIZE_BLACK_X)
						OfstLayBlack += (MSG_SIZE_BLACK_X - OfstLayBlack) * MSG_SPEED_X;

				if (OfstLayBlack > MSG_SIZE_BLACK_X)
					OfstLayBlack = MSG_SIZE_BLACK_X;
			}
			else
			{
				if (OfstLayBlack > 0)
					OfstLayBlack -= (((MSG_SIZE_BLACK_X)-OfstLayBlack) * MSG_SPEED_X) + 0.01f;

				if (OfstLayBlack < (MSG_SIZE_BLACK_X - 5.f) && OfstLayWhite >= 0)
					OfstLayWhite -= ((MSG_SIZE_WHITE_X - OfstLayWhite) * MSG_SPEED_X) + 0.01f;
			}

			GP_Render->DrawFilledBox(int(posX - OfstLayWhite), posY, int(OfstLayWhite) + 3, MSG_SIZE_Y, Color::White());
			GP_Render->DrawFilledBox(int(posX - OfstLayBlack), posY, int(OfstLayBlack) + 3, MSG_SIZE_Y, Color::Black());

			Vec2 TPos = Vec2(0, 0);

			TPos.x = (posX - OfstLayBlack) + ((MSG_SIZE_BLACK_X / 2) - (MsgTextSize.x / 2));
			TPos.y = posY + (MSG_SIZE_Y / 2) - (MsgTextSize.y / 2);

			GP_Render->DrawString(20, TPos, Color::White(), MessageText.c_str());
		}
	}

	static Message& Get()
	{
		static Message obj;
		return obj;
	}
};*/