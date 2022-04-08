#pragma once


#include <iostream>

#include <Core/Object.h>
#include <Core/Vec2.h>
#include <Core/Timer.h>
#include <Core/Resources.h>
#include <Core/Audio.h>

#include <Render/Sprite.h>
#include <Render/Animation.h>

#include "Player.h"
#include "PathAlgorithm.h"
#include "ImGUI/imgui.h"
#include <Slot.h>



class Monster : public Object, public JsonBase
{
public:
	
	
	Monster();
	virtual ~Monster();

	void OnUpdate();
	void OnRender();

	virtual bool Deserialize(const rapidjson::Value& obj);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;

	void Move();
	void SetParams(Monster* m);
	static bool LoadMonsterTypes();
	static bool SaveMonsterTypes();

	void SetColliderBox(Vec2 colliderBox);
	Vec2 GetColliderBox();
	void SetColliderPos(Vec2 colliderPos);
	Vec2 GetColliderPos();
	Vec2 GetMoveTarget();
	void SetMoveTarget(Vec2 target);
	void ManageAnimation();

	int GetLife();
	void SetLife(int life);

	void DropItem();

	void OnCollide (Object* obj) override ;

	void AnimateAttack();
	bool IsDead();

	int monsterType;
	static std::vector <Monster*>MonsterTypes;
	static std::vector <Monster*>MonsterList;
	//std::vector<Slot*> GroundItems;
	Slot* GroundItems;
	bool isNearbyToLoot;



private:
	Sprite* _render;
	Sprite _sprite;
	Sprite _hurt_mask;
	Sprite _attack_sprite;
	Sprite _death;

	Action _current_action;
	Timer hurtTimer;
	turn_side _side;
	turn_side _attack_side;

	Animation _anim_stay;
	Animation _anim_run_up;
	Animation _anim_run_down;
	Animation _anim_run_left;
	Animation _anim_run_right;
		

	Animation _anim_attack_up;
	Animation _anim_attack_down;
	Animation _anim_attack_left;
	Animation _anim_attack_right;

	Animation _anim_death;

	Audio* _sound_hit;
	

	Vec2 _colliderBox;
	Vec2 _colliderPos;
	Vec2 _moveTarget;
	bool _target;
	
	bool droppedItem;

	std::string name;

	int MaxLife;
	int Life;
	int Armour;
	int FireResistance;
	int ColdResistance;
	int LightningResistance;

	
	Vec2 PhysicalDamage;
	Vec2 FireDamage;
	Vec2 ColdDamage;
	Vec2 LightningDamage;

	bool isDead;
};

