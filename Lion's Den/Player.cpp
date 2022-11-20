#include "Player.h"
#include <Render/Camera.h>
#include <IO/JsonBase.h>
#include <Render/Drawer.h>
#include <Monster.h>
#include <GUI/Widgets/Inventory.h>

Player::Player()
{
	Vec2 sprite_size(64, 64);
	SetSize(sprite_size);
	Life = 100;
	Mana = 100;
	Experience = 100;
	_side = SIDE_NONE;
	_speed = 2;
	SetType(OBJ_ENTITY_PLAYER);
	SetColliderBox(Vec2(32.0f, 32.0f));
	SetColliderPos(Vec2(GetColliderBox().x / 2, GetColliderBox().y));

	_sound_slash=new Audio("Data/Sounds/Sword.wav",false);

	_sound_walk=new Audio("Data/Sounds/0.ogg",false);
	_sound_pain=new Audio("Data/Sounds/pain1.wav",false);
	_sound_death=new Audio("Data/Sounds/death.wav",false);

	//stay
	_anim_stay.SetBeginFrame(20);
	_anim_stay.SetMaxFrame(20);
	_anim_stay.SetCurrentFrame(20);

	//Running up side
	_anim_run_up.SetBeginFrame(0);
	_anim_run_up.SetMaxFrame(7);
	_anim_run_up.SetCurrentFrame(0);

	//Running left side
	_anim_run_left.SetBeginFrame(8);
	_anim_run_left.SetMaxFrame(15);
	_anim_run_left.SetCurrentFrame(8);

	//Running down side
	_anim_run_down.SetBeginFrame(15);
	_anim_run_down.SetMaxFrame(23);
	_anim_run_down.SetCurrentFrame(15);

	//Running right side
	_anim_run_right.SetBeginFrame(24);
	_anim_run_right.SetMaxFrame(30);
	_anim_run_right.SetCurrentFrame(24);

	//
	_anim_attack_up.SetBeginFrame(0);
	_anim_attack_up.SetMaxFrame(5);
	_anim_attack_up.SetCurrentFrame(0);
	_anim_attack_up.SetSound(_sound_slash);
	_anim_attack_up._animate_once = true;

	_anim_attack_left.SetBeginFrame(6);
	_anim_attack_left.SetMaxFrame(11);
	_anim_attack_left.SetCurrentFrame(6);
	_anim_attack_left.SetSound(_sound_slash);
	_anim_attack_left._animate_once = true;

	_anim_attack_down.SetBeginFrame(12);
	_anim_attack_down.SetMaxFrame(17);
	_anim_attack_down.SetCurrentFrame(12);
	_anim_attack_left.SetSound(_sound_slash);
	_anim_attack_down._animate_once = true;

	_anim_attack_right.SetBeginFrame(18);
	_anim_attack_right.SetMaxFrame(23);
	_anim_attack_right.SetCurrentFrame(18);
	_anim_attack_right.SetSound(_sound_slash);
	_anim_attack_right._animate_once = true;

	_sprite.SetTexture(Resources::GetTexture("Characters/Player_Walk.png"));
	_sprite.SetFrameSize(sprite_size);
	_sprite.SetAnimation(&_anim_stay);

	_sprite.SetAnimationRate(300);

	_attack_sprite.SetTexture(Resources::GetTexture("Characters/Player_Attack.png"));
	_attack_sprite.SetFrameSize(sprite_size);
	//_attack_sprite.SetAnimation(&_anim_attack_up);
	_attack_sprite.SetAnimationRate(300);


	_sabre.SetTexture(Resources::GetTexture("Characters/Sabre.png"));
	_sabre.SetFrameSize(sprite_size*2);
	_sabre.SetAnimationRate(300);

	_death.SetTexture(Resources::GetTexture("Characters/Player_Death.png"));
	_death.SetFrameSize(sprite_size);
	_death.SetAnimationRate(300);
	_anim_death.SetBeginFrame(0);
	_anim_death.SetMaxFrame(5);
	_anim_death.SetCurrentFrame(0);
	_anim_death._animate_once = true;


	_hurt_mask.SetTexture(Resources::GetTexture("Characters/Player_Hurt_Mask.png"));
	_hurt_mask.SetAnimation(&_anim_stay);
	_hurt_mask.SetFrameSize(sprite_size);
	_hurt_mask.SetAnimationRate(300);


	_current_sprite = &_sprite;
	_current_action = STAY;

	SetColliderBox(Vec2(32.0f, 32.0f));
	SetColliderPos(Vec2(GetColliderBox().x / 2, GetColliderBox().y));
	SetMoveTarget(GetPos());


	velIncrease = 0.2f;
	velMax = 3.0f;

	isDead = false;

}

Player::~Player()
{
	//dtor
}

void Player::AnimateAttack()
{
	if (!_sound_slash->IsPlaying())
	{
		//_sound_slash->Play();
	}

	_current_action = ATTACK;

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
		attack->SetType(OBJ_ENTITY_ATTACK_ENEMY);
		//attack->Connect(this);
		this->Connect(attack);

		Collider::RegisterAttack(attack);
		
		_current_action = MOVE;
	}
}

bool Player::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
	writer->StartObject();
	writer->Key("Position");
	writer->StartObject();
	writer->Key("x");
	writer->Double(GetPos().x);
	writer->Key("y");
	writer->Double(GetPos().y);
	writer->EndObject();
	writer->Key("PlayTime");
	writer->Double(timeplayed+SDL_GetTicks());


	writer->EndObject();

	return true;
	//rializeToFile();
}

bool Player::Deserialize(const rapidjson::Value& obj)
{
	SetPos(Vec2(obj["Position"]["x"].GetFloat(), obj["Position"]["y"].GetFloat()));
	timeplayed = obj["PlayTime"].GetDouble();
	return true;
}
void Player::OnRender() {
	DrawPlayer();
	if (drawStatBox)
	{
		DrawStatBox();
	}
	

}

inline void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}
void Player::DrawStatBox() {

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
			
    ImVec2 pos = ImVec2(30,100 );
    ImVec2 size = ImVec2(250, 800);
   


	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);


    ImGui::Begin("Character Stats",NULL,window_flags);
    
  
	ImGui::Separator();
	TextCentered("Defence Stats");
	ImGui::Text("");

	ImGui::Separator();

	ImGui::Text("Total Armour :%d",AddArmour+((AddArmour*IncArmour)/100));
	ImGui::Text("Added Armour :%d",AddArmour);
	ImGui::Text("Increased Armour :%d%s",IncArmour,"%");
	ImGui::Text("");

	ImGui::Separator();
	ImGui::Text("Total Hp :%d",MaxLife+MaxHp+(MaxHp*IncHp)/100);
	ImGui::Text("Added Max HP :%d",MaxHp);
	ImGui::Text("Increased Max HP :%d%s",IncHp,"%");
	ImGui::Text("HP Regeneration :%d",HpReg);
	ImGui::Text("");

	ImGui::Separator();
	ImGui::Text("Total MP :%d",MaxMP);
	ImGui::Text("Added Max MP :%d",MaxMP);
	ImGui::Text("Increased Max MP :%d%s",IncMP,"%");
	ImGui::Text("MP Regeneration :%d",MpReg);
	ImGui::Text("");
	ImGui::Separator();
	ImGui::Text("Fire Resistance :%d",resFire);
	ImGui::Text("Cold Resistance:%d",resCold);
	ImGui::Text("Lightning Resistance:%d",resLightning);
	ImGui::Text("");




	ImGui::Separator();
	TextCentered("Offence Stats");
	ImGui::Text("");
	int p = AddPhys + (AddPhys * IncPhys) / 100;
	int f = AddFire + (AddFire * (IncFire + IncEle)) / 100;
	int c = AddCold + (AddCold * (IncCold + IncEle)) / 100;
	int l = AddLightning + (AddLightning * (IncLightning + IncEle)) / 100;
	ImGui::Text("Total Damage :%d (",p+f+c+l);

	
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,255,255));
	ImGui::SameLine(); ImGui::Text("%d",p);
	ImGui::PopStyleColor();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
	ImGui::SameLine(); ImGui::Text("+%d", f);
	ImGui::PopStyleColor();
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100,150,255,255));
	ImGui::SameLine(); ImGui::Text("+%d", c);
	ImGui::PopStyleColor();
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,50,255));
	ImGui::SameLine(); ImGui::Text("+%d)", l);
	ImGui::PopStyleColor();



	ImGui::Text("Added Physical Damage :%d",AddPhys);
	ImGui::Text("Increased Physical Damage :%d%s",IncPhys,"%");
	ImGui::Text("");

	ImGui::Text("Added Fire Damage :%d",AddFire);
	ImGui::Text("Added Cold Damage :%d",AddCold);
	ImGui::Text("Added Lightning Damage :%d",AddLightning);
	ImGui::Text("");

	ImGui::Text("Increased Elemental Damage :%d%s",IncEle,"%");
	ImGui::Text("Increased Fire Damage :%d%s",IncFire,"%");
	ImGui::Text("Increased Cold Damage :%d%s",IncCold,"%");
	ImGui::Text("Increased Lightning Damage :%d%s",IncLightning,"%");
	
	



	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(240,240,240,255));


	ImGui::PopStyleColor();
	ImGui::Separator();

    ImGui::End();


}
void Player::SetOldPos(Vec2 Pos)
{
	_OldPos = Pos;
}

Vec2 Player::GetOldPos()
{
	return _OldPos;
}

void Player::OnUpdate() {
	//if (!ImGui::GetIO().WantCaptureKeyboard && !ImGui::GetIO().WantCaptureMouse)
 	ManageAnimation();
	
	if(!isDead)
	{
		if (Life <= 0)
		{
			Collider::UnregisterTarget(this);
			Collider::UnregisterObject(this);
			isDead = true;

			_sound_death->Play();

		}

		if (hurtTimer.GetTime() >0 )
		{
			_current_sprite = &_hurt_mask;
			if(hurtTimer.GetTime() > SECOND * 1)
				hurtTimer.Stop();
		}

		if (Mouse::Pressed(MOUSE_LEFT)) // movement by mouse
		{
			//EmitAction("Move");

			//Vec2 pos = Window::GetCamera()->GetPos() + Mouse::GetPos();
			//SetMoveTarget(pos);
			
		}
		if (Keyboard::isKeyDown(KEY_A))
		{
			_side = SIDE_LEFT;
			_current_action = MOVE;

			Vec2 vel = GetVel();
			vel.x = Max(GetVel().x - velIncrease, -velMax);
			SetVel(vel);
			
		}
		if (Keyboard::isKeyDown(KEY_W))
		{
			_side = SIDE_UP;
			_current_action = MOVE;
			Vec2 vel = GetVel();
			vel.y = Max(GetVel().y - velIncrease, -velMax);
			SetVel(vel);
		}
		if (Keyboard::isKeyDown(KEY_S))
		{
			_side = SIDE_DOWN;
			_current_action = MOVE;
			Vec2 vel = GetVel();
			vel.y = Min(GetVel().y + velIncrease, +velMax);
			SetVel(vel);
		}
		if (Keyboard::isKeyDown(KEY_D))
		{
			_side = SIDE_RIGHT;
			_current_action = MOVE;
			Vec2 vel = GetVel();
			vel.x = Min(GetVel().x + velIncrease, velMax);
			SetVel(vel);
		}
		if (Keyboard::isKeyDown(KEY_LSHIFT))
		{
			velIncrease = 0.3f;
		}
		else
		{
			velIncrease = 0.2f;
		}

		if (_attack_sprite.GetAnimation()->IsAnimationFinished())
		{
			if (GetVel().x == 0 && GetVel().y == 0)
			{
				_side = SIDE_NONE;
				_current_action = STAY;
			}
		}

		if (Mouse::Pressed(MOUSE_RIGHT)) // movement by mouse
		{
			AnimateAttack();

			//Vec2 pos = Window::GetCamera()->GetPos() + Mouse::GetPos();
			//SetMoveTarget(pos);
		}
	}

	//Move();
	
	//MoveRandom();
	if (Inventory::inventoryChanged)
	{
		UpdateStats();
	}

}

void Player::MoveSound() {

	footstep.Start();
	if (!_sound_walk->IsPlaying()&& footstep.GetTime() >0 )
	{
		
		if (footstep.GetTime() > SECOND * (velMax-GetVel().GetLength()) *0.5f)
		{
			//_sound_walk->Play();
				footstep.Stop();
		}
			
	}


}

void Player::ManageAnimation()
{
	if (!isDead)
	{
		
		if (_current_action == MOVE)
		{
			MoveSound();
			//_current_sprite->GetAnimation()->ResetFrame();
			_current_sprite = &_sprite;
			if (_side == SIDE_LEFT)
			{
				_current_sprite->SetAnimation(&_anim_run_left);
				_sabre.SetAnimation(&_anim_attack_left);
			}
			if (_side == SIDE_UP)
			{
				_current_sprite->SetAnimation(&_anim_run_up);
				_sabre.SetAnimation(&_anim_attack_up);
			}
			if (_side == SIDE_DOWN)
			{
				_current_sprite->SetAnimation(&_anim_run_down);
				_sabre.SetAnimation(&_anim_attack_down);
			}
			if (_side == SIDE_RIGHT)
			{
				_current_sprite->SetAnimation(&_anim_run_right);
				_sabre.SetAnimation(&_anim_attack_right);
			}
			if (_side == SIDE_NONE)
			{
				_current_sprite->SetAnimation(&_anim_stay);
				_sabre.SetAnimation(&_anim_attack_down);
			}
		}
		if (_current_action == STAY)
		{
			_current_sprite = &_sprite;
			_sprite.SetAnimation(&_anim_stay);

		}

		if (_current_action == ATTACK)
		{
			Vec2 ppos = Vec2(round(GetPos().x + 32), round(GetPos().y + 32));
			Vec2 mtar = Mouse::GetPos() + Window::GetCamera()->GetPos();

			Vec2 a = ppos - mtar;
			//Drawer::Line(ppos, mtar, COLOR_RED);

			Vec2 diff = a.GetNormalized() * -1;
			float angle = a.GetAngle();
			_current_sprite = &_attack_sprite;

			if (angle > -45 && angle < 45 && angle != 0)
			{
				_side = SIDE_LEFT;
				_attack_side = SIDE_LEFT;
				_attack_sprite.SetAnimation(&_anim_attack_left);
				_sabre.SetAnimation(&_anim_attack_left);
			}
			else if (angle > 45 && angle < 135)
			{
				_side = SIDE_UP;
				_attack_side = SIDE_UP;
				_attack_sprite.SetAnimation(&_anim_attack_up);
				_sabre.SetAnimation(&_anim_attack_up);
			}
			else if (angle > 135 && angle < 180 || angle > -180 && angle < -135)
			{
				_side = SIDE_RIGHT;
				_attack_side = SIDE_RIGHT;
				_attack_sprite.SetAnimation(&_anim_attack_right);
				_sabre.SetAnimation(&_anim_attack_right);
			}
			else if (angle > -135 && angle < -45)
			{
				_side = SIDE_DOWN;
				_attack_side = SIDE_DOWN;
				_attack_sprite.SetAnimation(&_anim_attack_down);
				_sabre.SetAnimation(&_anim_attack_down);
			}
		}
	}

	else
	{
		_current_sprite = &_death;
		_current_sprite->SetAnimation(&_anim_death);
		if (_anim_death.IsAnimationFinished())
		{
			_anim_death.SetCurrentFrame(5);
		}
	}


}

void Player::UpdateStats()
{
	StatsReset();
	for (auto i : Inventory::equipment)
	{
		if (i->getHaveitem())
		{
			for(auto j : i->GetItem()->GetModifiers())
			{
				if (j->GetTag() == INC_PHYS)
				{
					IncPhys += j->GetValue();
				}
				if (j->GetTag() == INC_ELE)
				{
					IncEle += j->GetValue();
				}
				if (j->GetTag() == INC_FIRE)
				{
					IncFire+=j->GetValue();
				}
				if (j->GetTag() == INC_COLD)
				{
					IncCold+=j->GetValue();
				}
				if (j->GetTag() == INC_LIGHTNING)
				{
					IncLightning+=j ->GetValue();
				}
				if (j->GetTag() == ADD_PHYS)
				{
					AddPhys+= j->GetValue();
				}
				if (j->GetTag() == ADD_FIRE)
				{
					AddFire +=j->GetValue();
				}
				if (j->GetTag() == ADD_COLD)
				{
					AddCold+= j->GetValue();
				}
				if (j->GetTag() == ADD_LIGHTNING)
				{
					AddLightning+= j->GetValue();
				}
				if (j->GetTag() == MAX_HP)
				{
					MaxHp+=j->GetValue();
				}
				if (j->GetTag() == MAX_MP)
				{
					MaxMP+= j->GetValue();
				}
				if (j->GetTag() == INC_HP)
				{
					IncHp+= j->GetValue();
				}
				if (j->GetTag() == INC_MP)
				{
					IncMP += j->GetValue();
				}
				if (j->GetTag() == HP_REG)
				{
					HpReg+= j->GetValue();
				}
				if (j->GetTag() == MP_REG)
				{
					MpReg+= j->GetValue();
				}
				if (j->GetTag() == ADD_ARMOUR)
				{
					AddArmour+= j->GetValue();
				}
				if (j->GetTag() == INC_ARMOUR)
				{
					IncArmour += j->GetValue();
				}


			}
			
			if (i->GetItem()->GetBaseStat().tag == BASE_PHYS)
			{
				AddPhys += i->GetItem()->GetBaseStat().value;
			}
			if (i->GetItem()->GetBaseStat().tag == BASE_HP)
			{
				MaxHp += i->GetItem()->GetBaseStat().value;
			}
			if (i->GetItem()->GetBaseStat().tag == BASE_ARMOUR)
			{
				AddArmour += i->GetItem()->GetBaseStat().value;
			}


		}
	}
	







	Inventory::inventoryChanged = false;
}

void Player::StatsReset()
{
	Life = 100;
	MaxLife = 100;
	IncPhys=0;
	IncFire=0;
	IncCold=0;
	IncLightning=0;
	AddPhys=0;
	AddFire=0;
	AddCold=0;
	AddLightning=0;
	MaxHp=0;
	IncHp=0;
	MaxMP=0;
	IncMP=0;
	HpReg=0;
	MpReg=0;
	AddArmour=0;
	IncArmour=0;
}

void Player::DrawPlayer() {
	//_current_sprite->Draw(GetGlobalPos(), Vec2(64, 64), Window::GetCamera());
	//_current_sprite->Draw(GetPos(), Vec2(64, 64), Window::GetCamera());

	_current_sprite->Draw(GetGlobalPos(), Vec2(64, 64), Window::GetCamera());
	_sabre.Draw(GetGlobalPos()-Vec2(32, 32), Vec2(128, 128), Window::GetCamera());
	//Drawer::Rect(Vec2(position.x*32,position.y*32),Vec2(64, 64), COLOR_RED, false);
	//.//_attack_sprite.Draw(GetGlobalPos(), Vec2(64, 64), Window::GetCamera());
	//_sprite.Draw(GetGlobalPos()+Vec2(64,0), Vec2(64, 64), Window::GetCamera());
}

void Player::SetColliderBox(Vec2 colliderBox)
{
	_colliderBox = colliderBox;
}

Vec2 Player::GetColliderBox()
{
	return _colliderBox;
}

void Player::SetColliderPos(Vec2 colliderPos)
{
	_colliderPos = colliderPos;
}

Vec2 Player::GetColliderPos()
{
	return _colliderPos;
}

Vec2 Player::GetMoveTarget()
{
	return _moveTarget;
}

void Player::SetMoveTarget(Vec2 target)
{
	_moveTarget = target;
}

int Player::GetCurrentAction()
{
	return _current_action;
}

int Player::GetCurrentSide()
{
	return _side;
}

int Player::CalculateDamage()
{
	int phys=AddPhys + AddPhys * IncPhys;
	int fire = AddFire + AddFire * IncFire+ AddFire* IncEle;
	int cold = AddCold + AddCold * IncCold + AddCold * IncEle;
	int light = AddLightning + AddLightning * IncLightning + AddLightning * IncEle;
	int finaldmg = phys + fire + cold + light;
	if (finaldmg < 10)
	{
		return 10;
	}
	else
	{
		return finaldmg;
	}
	
}

int Player::GetLife()
{
	return Life;
}
int Player::GetMana()
{
	return Mana;
}
int Player::GetExperience()
{
	return Experience;
}

void Player::SetLife(int life)
{
	Life = life;
}
void Player::SetMana(int mana)
{
	Mana = mana;
}
void Player::SetExperience(int experience)
{
	Experience = experience;
}

void Player::AddAction(const std::string& action_name, std::function<void()> action, Object* listener) {
	size_t s = _Listeners.size();
	bool found = false;

	for (size_t i = 0; i < s; ++i) {
		if (_Listeners[i].listener == listener) {
			_Listeners[i].Actions[action_name] = action;
			found = true;
			break;
		}
	}

	if (!found) {
		Actionss actions;
		actions.listener = listener;
		actions.Actions[action_name] = action;

		_Listeners.push_back(actions);
	}
}

void Player::DeleteAction(const std::string& action_name, Object* listener) {
	size_t s = _Listeners.size();

	for (size_t i = 0; i < s; ++i) {
		if (_Listeners[i].listener == listener) {
			if (_Listeners[i].Actions.count(action_name)) {
				_Listeners[i].Actions.erase(action_name);
				break;
			}
		}
	}
}

void Player::DeleteAllActions(Object* listener) {
	for (auto it = _Listeners.begin(); it != _Listeners.end(); ++it) {
		if (it->listener == listener) {
			_Listeners.erase(it);
			break;
		}
	}
}

void Player::EmitAction(const std::string& action_name) {
	size_t s = _Listeners.size();

	for (size_t i = 0; i < s; ++i) {
		if (_Listeners[i].Actions.count(action_name)) {
			auto fun = _Listeners[i].Actions[action_name];

			if (fun) {
				fun();
			}
		}
	}
}

void Player::OnCollide(Object* obj)
{
	//if(obj->GetType()==OBJ_ENTITY_ATTACK)
	//printf("monster attacked player");
	//hurtTimer.
	//hurtTimer.Start();
	if (obj->GetType() == OBJ_ENTITY_ATTACK_PLAYER)
	{
		hurtTimer.Start();
		_sound_pain->Play();
	}
	

}



Attack::Attack()
{
	
	timer.Start();
}

void Attack::OnUpdate()
{

	
	if (timer.GetTime() > SECOND*0.3f)
	{
		Collider::UnregisterAttack(this);

		
		//printf("Unregistered at timer:%d", timer.GetTime());
	}

	







}

void Attack::OnCollide(Object* tar)
{
	// printf("%d collided with %d \n",this->GetId(), obj->GetId());
	
	if (tar->GetType()==OBJ_ENTITY_PLAYER)
	{
		Player* target = static_cast<Player*>(tar);
		target->SetLife(target->GetLife() - 10);
		 printf("Target:%d attacked life left: %d \n",target->GetId(), target->GetLife() );

	}
	if (tar->GetType()==OBJ_ENTITY_MONSTER)
	{

		//Player* a = static_cast<Player*>(att);
		//int attackVal = a->CalculateDamage();
		//printf("attackVal: %d",attackVal);



		Monster* target = static_cast<Monster*>(tar);
		target->SetLife(target->GetLife() - 50);

		



		 printf("Target:%d attacked life left: %d \n",target->GetId(), target->GetLife() );

	}

	 

}
