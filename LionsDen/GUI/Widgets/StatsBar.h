#ifndef STATSBAR_H_
#define STATSBAR_H_

#include <string>
#include <sstream>

#include "Render/Sprite.h"
#include "GUI/Widget.h"
#include "GUI/Text.h"
#include "Core/Resources.h"
#include <Player.h>


class StatsBar : public Widget {
public:
	StatsBar(const Vec2& pos, const Vec2& size);
	virtual ~StatsBar();

	//virtual void SetSize(const Vec2& size);
	virtual void OnRender();

	//virtual void Move(const Vec2& delta_pos);
	//virtual void SetPos(const Vec2& pos);

	void SetStyle(const std::string& stylefile);
	void UpdateValues(Player *player);
	void ConnectPlayer(Player* player);

	void RenderBars();

	void RecalcRelative();


	void SetRenderOption(bool r);
	bool GetRenderOption();


	bool render;


protected:

	Sprite _back;
	Sprite _life;
	Sprite _mana;
	Sprite _experience;

	Player* pla;

	int life;
	int mana;
	int experience;
};

#endif /* STATSBAR_H_ */