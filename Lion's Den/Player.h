#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>

#include <Core/Object.h>
#include <Core/Vec2.h>
#include <Core/Timer.h>
#include <Core/Resources.h>
#include <Core/Audio.h>

#include <Render/Sprite.h>
#include <Render/Animation.h>
#include <IO/Keyboard.h>
#include <IO/Mouse.h>
#include <IO/JsonBase.h>
#include <functional>





enum turn_side {
	SIDE_NONE = 0, // staying
	SIDE_UP,
	SIDE_DOWN,
	SIDE_LEFT,
	SIDE_RIGHT
};
enum Action {
	MOVE = 0, // staying
	STAY,
	ATTACK,
};

class Attack :public Object {
public:
	Attack();

	
	void OnUpdate();
	void OnCollide(Object* attacker) override;

	Timer timer;

};



class Player : public Object, public JsonBase {
public:
	Player();
	virtual ~Player();

	void AnimateAttack();

	virtual bool Deserialize(const rapidjson::Value& obj);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;


	void SetOldPos(Vec2 Pos);
	Vec2 GetOldPos();

	void OnUpdate();
	void OnRender();
	void OnCollide (Object* obj) override ;
	void DrawPlayer();
	void DrawStatBox();
	unsigned int _speed; //keyboard delay, lower is faster

	void MoveSound();
	void ManageAnimation();
	void UpdateStats();
	void StatsReset();

	void SetColliderBox(Vec2 colliderBox);
	Vec2 GetColliderBox();
	void SetColliderPos(Vec2 colliderPos);
	Vec2 GetColliderPos();
	Vec2 GetMoveTarget();
	void SetMoveTarget(Vec2 target);
	int GetCurrentAction();
	int GetCurrentSide();
	void SetCurrentAction();
	int CalculateDamage();

	int GetLife();
	int GetMana();
	int GetExperience();

	void SetLife(int life);
	void SetMana(int mana);
	void SetExperience(int experience);

	void AddAction(const std::string& action_name, std::function<void(void)> action, Object* listener);
	void DeleteAction(const std::string& action_name, Object* listener);
	void DeleteAllActions(Object* listener);
	void EmitAction(const std::string& action_name);
	bool drawStatBox=0;
	bool isDead;

	double timeplayed;
private:

	static const int input_timer_time = 500;
	Timer _input_timer;
	
	Sprite* _current_sprite;
	Sprite _attack_sprite;
	Sprite _sprite;
	Sprite _sabre;
	Sprite _death;
	Sprite _hurt_mask;

	Animation _anim_stay;
	Animation _anim_run_up;
	Animation _anim_run_down;
	Animation _anim_run_left;
	Animation _anim_run_right;

	Animation _anim_attack_up;
	Animation _anim_attack_down;
	Animation _anim_attack_left;
	Animation _anim_attack_right;

	Audio* _sound_slash;
	Audio* _sound_walk;
	Audio* _sound_pain;
	Audio* _sound_death;

	Animation _anim_death;

	typedef struct { Object* listener; std::unordered_map<std::string, std::function<void(void)> > Actions; } Actionss;
	std::vector<Actionss> _Listeners;
	Vec2 _OldPos; // used for managing animations direction


	Vec2 _colliderBox;
	Vec2 _colliderPos;
	Vec2 _moveTarget;
	Timer hurtTimer;
	Timer footstep;


	Action _current_action;
	turn_side _side;
	turn_side _attack_side;

	

	float velIncrease;
	float velMax;

	int Life;
	int MaxLife;
	int Mana;
	int Experience;
	int Armour;


	int resFire;
	int resCold;
	int resLightning;

	//stats
	int IncPhys;
	int IncEle;
	int IncFire;
	int IncCold;
	int IncLightning;
	int AddPhys;
	int AddFire;
	int AddCold;
	int AddLightning;
	int MaxHp;
	int IncHp;
	int MaxMP;
	int IncMP;
	int HpReg;
	int MpReg;
	int AddArmour;
	int IncArmour;

	




};

#endif // PLAYER_H
