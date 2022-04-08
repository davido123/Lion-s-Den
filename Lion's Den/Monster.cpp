#include "Monster.h"
#include "Player.h"
#include "PathAlgorithm.h"
#include <Render/Drawer.h>
#include <Item.h>
#include <random>
#include <GUI/Widgets/Inventory.h>

std::vector <Monster*> Monster::MonsterTypes;
std::vector <Monster*> Monster::MonsterList;
std::random_device rd2;
std::mt19937 mt2(rd2());

Monster::Monster()
{
	Vec2 sprite_size(64, 64);
	SetSize(sprite_size);
	_side = SIDE_NONE;
	SetType(OBJ_ENTITY_MONSTER);
	SetColliderBox(Vec2(32.0f, 32.0f));
	SetColliderPos(Vec2(GetColliderBox().x/2, GetColliderBox().y));



	//stay
	_anim_stay.SetBeginFrame(16);
	_anim_stay.SetMaxFrame(16);
	_anim_stay.SetCurrentFrame(16);

	//Running up side
	_anim_run_up.SetBeginFrame(0);
	_anim_run_up.SetMaxFrame(7);
	_anim_run_up.SetCurrentFrame(0);

	//Running left side
	_anim_run_left.SetBeginFrame(8);
	_anim_run_left.SetMaxFrame(15);
	_anim_run_left.SetCurrentFrame(8);

	//Running down side
	_anim_run_down.SetBeginFrame(16);
	_anim_run_down.SetMaxFrame(23);
	_anim_run_down.SetCurrentFrame(16);

	//Running right side
	_anim_run_right.SetBeginFrame(24);
	_anim_run_right.SetMaxFrame(31);
	_anim_run_right.SetCurrentFrame(24);

	_sprite.SetTexture(Resources::GetTexture("Characters/Skeleton_Walk.png"));
	_sprite.SetAnimation(&_anim_stay);
	_sprite.SetFrameSize(sprite_size);
	_sprite.SetAnimationRate(300);

	_hurt_mask.SetTexture(Resources::GetTexture("Characters/Skeleton_Hurt_Mask.png"));
	_hurt_mask.SetAnimation(&_anim_stay);
	_hurt_mask.SetFrameSize(sprite_size);
	_hurt_mask.SetAnimationRate(300);

	//
	_anim_attack_up.SetBeginFrame(0);
	_anim_attack_up.SetMaxFrame(5);
	_anim_attack_up.SetCurrentFrame(0);
	_anim_attack_up._animate_once = true;

	_anim_attack_left.SetBeginFrame(6);
	_anim_attack_left.SetMaxFrame(11);
	_anim_attack_left.SetCurrentFrame(6);
	_anim_attack_left._animate_once = true;

	_anim_attack_down.SetBeginFrame(12);
	_anim_attack_down.SetMaxFrame(17);
	_anim_attack_down.SetCurrentFrame(12);
	_anim_attack_down._animate_once = true;

	_anim_attack_right.SetBeginFrame(18);
	_anim_attack_right.SetMaxFrame(23);
	_anim_attack_right.SetCurrentFrame(18);
	_anim_attack_right._animate_once = true;

	_attack_sprite.SetTexture(Resources::GetTexture("Characters/Skeleton_Attack.png"));
	_attack_sprite.SetFrameSize(sprite_size);
	//_attack_sprite.SetAnimation(&_anim_attack_up);
	_attack_sprite.SetAnimationRate(300);

	_death.SetTexture(Resources::GetTexture("Characters/Skeleton_Death.png"));
	_death.SetFrameSize(sprite_size);
	_death.SetAnimationRate(300);
	_anim_death.SetBeginFrame(0);
	_anim_death.SetMaxFrame(5);
	_anim_death.SetCurrentFrame(0);
	_anim_death._animate_once = true;

	_sound_hit=new Audio("Data/Sounds/hit.wav",false);
	isDead = false;
	droppedItem = false;

	_render = &_sprite;
	_current_action = STAY;

	MaxLife = Life = 100;
}
void Monster::SetParams(Monster* m)
{
	std::string name;
	MaxLife = m->Life;
	Life=m->Life;
	Armour = m->Armour;
	FireResistance = m->FireResistance;
	ColdResistance = m->ColdResistance;
	LightningResistance = m->LightningResistance;
	PhysicalDamage = m->PhysicalDamage;
	FireDamage = m->FireDamage;
	ColdDamage = m->ColdDamage;
	LightningDamage = m->LightningDamage;
	monsterType = m->monsterType;

	
}
inline void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}


Monster::~Monster() {
}

void Monster::OnUpdate()
{
	ManageAnimation();
	if (!isDead)
	{
		Move();
		if (Life <= 0)
		{
			Collider::UnregisterTarget(this);
			Collider::UnregisterObject(this);
			isDead = true;
		}
		if (isDead && !droppedItem)
		{
			droppedItem = true;
			DropItem();
		}
 		if (hurtTimer.GetTime() >0 )
		{
			_render = &_hurt_mask;
			if(hurtTimer.GetTime() > SECOND * 1)
				hurtTimer.Stop();
		}
		else
		{
			//_render = &_sprite;
		}
	}

	
}

void Monster::Move()
{
	Vec2 ppos = Vec2(round(GetPos().x + 32), round(GetPos().y + 32));
	Vec2 mtar = Vec2(round(GetMoveTarget().x + 32), round(GetMoveTarget().y + 32));
	Vec2 a = ppos - mtar;
	
	if (a.GetLength() < 250)
	{
		Drawer::Line(ppos, mtar, COLOR_RED);
		_target = true;
		_current_action = MOVE;
		Vec2 vel = GetVel();
		if (mtar.x > ppos.x)vel.x = Min(GetVel().x + 0.15f, 3.0f);
		if (mtar.x < ppos.x)vel.x = Max(GetVel().x - 0.15f, -3.0f);
		if (mtar.y > ppos.y)vel.y = Min(GetVel().y + 0.15f, 3.0f);
		if (mtar.y < ppos.y)vel.y = Max(GetVel().y - 0.15f, -3.0f);
		SetVel(vel);

		if (a.GetLength() < 70)
		{
			Drawer::Line(ppos, mtar, COLOR_OLIVE);
			_current_action = ATTACK;
			AnimateAttack();
		}
	}
	else {
		_current_action = STAY;
		_target = false;
	}
		
}

void Monster::AnimateAttack()
{

	_render = &_attack_sprite;
	if (_attack_sprite.GetAnimation()->IsAnimationFinished())
	{
		_attack_sprite.GetAnimation()->SetAnimationFinished(false);
		_attack_sprite.GetAnimation()->ResetFrame();
		Object* attack = new Attack;
		Vec2 center = GetPos()+GetSize()/2-Vec2(16,0);
 		if (_attack_side==SIDE_LEFT) attack->SetPos(center - Vec2(32, 0));
		else if (_attack_side==SIDE_RIGHT) attack->SetPos(center - Vec2(-32, 0));
		else if (_attack_side==SIDE_UP) attack->SetPos(center - Vec2(0, 32));
		else if (_attack_side==SIDE_DOWN) attack->SetPos(center - Vec2(0, -32));


		
		attack->SetSize(Vec2(32, 32));
		attack->SetType(OBJ_ENTITY_ATTACK_PLAYER);
		//attack->Connect(this);
		this->Connect(attack);

		Collider::RegisterAttack(attack);
		
		_current_action = MOVE;
	}
}

bool Monster::IsDead()
{
	return isDead;
}



bool Monster::LoadMonsterTypes()
{
	return false;
}

bool Monster::SaveMonsterTypes()
{
	Monster* a;
  
    a=new Monster();
	a->name = "Physical";
	a->Life = 100;
	a->Armour = 150;
	a->FireResistance = 10;
	a->ColdResistance=10;
	a->LightningResistance=10;
	a->PhysicalDamage=Vec2(5,10);
	a->FireDamage=Vec2(5,10);
	a->ColdDamage=Vec2(5,10);
	a->LightningDamage=Vec2(5,10);
	a->monsterType = 0;
	Monster::MonsterTypes.push_back(a);

	a=new Monster();
	a->name = "Fire";
	a->Life = 10;
	a->Armour = 10;
	a->FireResistance = 1;
	a->ColdResistance=10;
	a->LightningResistance=10;
	a->PhysicalDamage=Vec2(5,10);
	a->FireDamage=Vec2(5,10);
	a->ColdDamage=Vec2(5,10);
	a->LightningDamage=Vec2(5,10);
	a->monsterType = 1;
	Monster::MonsterTypes.push_back(a);

	a=new Monster();
	a->name = "Cold";
	a->Life = 10;
	a->Armour = 10;
	a->FireResistance = 1;
	a->ColdResistance=10;
	a->LightningResistance=10;
	a->PhysicalDamage=Vec2(5,10);
	a->FireDamage=Vec2(5,10);
	a->ColdDamage=Vec2(5,10);
	a->LightningDamage=Vec2(5,10);
	a->monsterType = 2;
	Monster::MonsterTypes.push_back(a);

	a=new Monster();
	a->name = "Lightning";
	a->Life = 10;
	a->Armour = 10;
	a->FireResistance = 1;
	a->ColdResistance=10;
	a->LightningResistance=10;
	a->PhysicalDamage=Vec2(5,10);
	a->FireDamage=Vec2(5,10);
	a->ColdDamage=Vec2(5,10);
	a->LightningDamage=Vec2(5,10);
	a->monsterType = 3;
	Monster::MonsterTypes.push_back(a);

	a=new Monster();
	a->name = "Boss";
	a->Life = 10;
	a->Armour = 10;
	a->FireResistance = 1;
	a->ColdResistance=10;
	a->LightningResistance=10;
	a->PhysicalDamage=Vec2(5,10);
	a->FireDamage=Vec2(5,10);
	a->ColdDamage=Vec2(5,10);
	a->LightningDamage=Vec2(5,10);
	a->monsterType = 4;
	Monster::MonsterTypes.push_back(a);

	for (int i = 0; i < 5; i++)
	{
		a=new Monster();
		a = Monster::MonsterTypes.at(i);
		Monster::MonsterTypes.push_back(a);
	}


	return true;
}

void Monster::ManageAnimation()
{
	if (!isDead)
	{
		Vec2 ppos = Vec2(round(GetPos().x), round(GetPos().y));
		Vec2 mtar = Vec2(round(GetMoveTarget().x), round(GetMoveTarget().y));

		Vec2 a = ppos - mtar;

		Vec2 diff = a.GetNormalized() * -1;
		float angle = a.GetAngle();


		if (angle > -45 && angle < 45 && angle != 0)
		{
			_side = SIDE_LEFT;
			_attack_side = SIDE_LEFT;

			if (_current_action == MOVE)
			{
				_render = &_sprite;
				_render->SetAnimation(&_anim_run_left);
			}
			if (_current_action == ATTACK)
			{
				_render = &_attack_sprite;
				_render->SetAnimation(&_anim_attack_left);
			}


			//_hurt_mask.SetAnimation(&_anim_run_left);
		}
		else if (angle > 45 && angle < 135)
		{
			_side = SIDE_UP;
			_attack_side = SIDE_UP;
			if (_current_action == MOVE)
			{
				_render = &_sprite;
				_render->SetAnimation(&_anim_run_up);
			}
			if (_current_action == ATTACK)
			{
				_render = &_attack_sprite;
				_render->SetAnimation(&_anim_attack_up);
			}
			//_hurt_mask.SetAnimation(&_anim_run_up);
		}
		else if (angle > 135 && angle < 180 || angle > -180 && angle < -135)
		{
			_side = SIDE_RIGHT;
			_attack_side = SIDE_RIGHT;
			if (_current_action == MOVE)
			{
				_render = &_sprite;
				_render->SetAnimation(&_anim_run_right);
			}
			if (_current_action == ATTACK)
			{
				_render = &_attack_sprite;
				_render->SetAnimation(&_anim_attack_right);
			}
			//_hurt_mask.SetAnimation(&_anim_run_right);
		}
		else if (angle > -135 && angle < -45)
		{
			_side = SIDE_DOWN;
			_attack_side = SIDE_DOWN;
			if (_current_action == MOVE)
			{
				_render = &_sprite;
				_render->SetAnimation(&_anim_run_down);
			}
			if (_current_action == ATTACK)
			{
				_render = &_attack_sprite;
				_render->SetAnimation(&_anim_attack_down);
			}
			//_hurt_mask.SetAnimation(&_anim_run_down);
		}
		if (!_target)
		{
			_side = SIDE_NONE;
			_attack_side = SIDE_NONE;
			_current_action = STAY;
			_render = &_sprite;
			_render->SetAnimation(&_anim_stay);
			//_hurt_mask.SetAnimation(&_anim_stay);

		}
	}
	else
	{
		_render = &_death;
		_render->SetAnimation(&_anim_death);
		if (_anim_death.IsAnimationFinished())
		{
			_anim_death.SetCurrentFrame(5);
		}
	}
}

void Monster::OnRender()
{
    // #Monster
	if (!isDead && Life<MaxLife)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		Vec2 tmp = Window::GetCamera()->GetPos();
		ImGui::SetNextWindowPos(ImVec2(GetGlobalPos().x-tmp.x, GetGlobalPos().y-tmp.y-20), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(80, 30), ImGuiCond_Always);
	
		std::string s = std::to_string(GetId());
		char const *pchar = s.c_str(); 

		ImGui::Begin(pchar,NULL,window_flags);
		static float progress = 0.1f;

		progress = Life / MaxLife;
	
		char buf[32];
		sprintf_s(buf, "%d/%d",Life , MaxLife);
		ImGui::ProgressBar(progress, ImVec2(0.0f, -1.0f), buf);
		ImGui::End();
	}
	

	/*
	ImGui::Begin("mosterbox");
	ImGui::Text("Action:%d",_current_action);
	ImGui::Text("Attack side:%d",_attack_side);
	ImGui::Text("Target:%d",_target);

	ImGui::End();
	*/
	_render->Draw(GetGlobalPos(), Vec2(64, 64), Window::GetCamera());
}

bool Monster::Deserialize(const rapidjson::Value& obj)
{
	SetPos(Vec2(obj["Position"]["x"].GetFloat(), obj["Position"]["y"].GetFloat()));
	Life = obj["life"].GetInt();
	MaxLife = obj["Maxlife"].GetInt();
		
	Collider::RegisterObject(this, GetColliderPos(),GetColliderBox(), false);
	Collider::RegisterTarget(this, GetColliderPos(),GetColliderBox());

	return true;
}
bool Monster::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
	writer->StartObject();
		writer->Key("Position");
			writer->StartObject();
				writer->Key("x");
			writer->Double(GetPos().x);
				writer->Key("y");
			writer->Double(GetPos().y);
		writer->EndObject();
	writer->Key("life");
		writer->Int(Life);
	writer->Key("Maxlife");
		writer->Int(MaxLife);

	writer->EndObject();


	

	return true;
}



void Monster::DropItem()
{
	std::uniform_int_distribution<int> drop(0, 2);

	if (drop(mt2) == 0)
	{
		Slot* slot = new Slot;
		Item* item=new Item;
		item->SetPos(GetPos());
		std::uniform_int_distribution<int> dist(0, 8);
		std::uniform_int_distribution<int> distr(0, 3);
		item->SetItemType((item_type)dist(mt2));
		item->SetRarity(distr(mt2));
		while (item->Modifiers.size() < item->rarity)
		{
			item->AddRandomModifier();
		}
		item->SetBaseStat();


		item->SetSizeByItemType();
		//item->_default_item_size = Vec2(39.0f, 36.5f);
		item->SetSize(item->Item::_default_item_size * item->GetGridSize());
		item->SetTexture();
		item->render = false;
		slot->ConnectItem(item);
		slot->SetPos(GetPos());

		GroundItems = slot;
		//GroundItems.push_back(slot);
	}


	
}

void Monster::OnCollide(Object* obj)
{
	if (obj->GetType() == OBJ_ENTITY_ATTACK_ENEMY)
	{
		hurtTimer.Start();
		_sound_hit->Play(); 
	}
}

void Monster::SetColliderBox(Vec2 colliderBox)
{
	_colliderBox = colliderBox;
}

Vec2 Monster::GetColliderBox()
{
	return _colliderBox;
}

void Monster::SetColliderPos(Vec2 colliderPos)
{
	_colliderPos = colliderPos;
}

Vec2 Monster::GetColliderPos()
{
	return _colliderPos;
}
Vec2 Monster::GetMoveTarget()
{
	return _moveTarget;
}

void Monster::SetMoveTarget(Vec2 target)
{
	_moveTarget = target;
}

int Monster::GetLife()
{
	return Life;
}
void Monster::SetLife(int life)
{
	Life = life;
}
