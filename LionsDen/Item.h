#pragma once

#include <string>
#include <Core/Vec2.h>
#include "Render/Sprite.h"
#include <IO/JsonBase.h>
#include <variant>

enum item_type {
	HELMET = 0,
	ARMOR,
	MAINHAND,
	OFFHAND,
	BELT,
	AMULET,
	RING,
	BOOTS,
	GLOVES,
	OTHER

};
enum modifier_type {
	IMPLICIT=0,
	AFFIX
	

};
enum modifier_tag {
	INC_PHYS = 0, INC_ELE, INC_FIRE, INC_COLD, INC_LIGHTNING,
	ADD_PHYS, ADD_FIRE, ADD_COLD, ADD_LIGHTNING,
	MAX_HP, MAX_MP,
	INC_HP, INC_MP,
	HP_REG, MP_REG,
	ADD_ARMOUR, INC_ARMOUR,
	BASE_ARMOUR,BASE_HP,BASE_PHYS


};
enum item_rarity {
	NORMAL = 0,
	MAGIC,
	RARE,
	UNIQUE
};

class Modifier : public Object, public JsonBase
{
public:
	Modifier(std::string _name,modifier_type _type,modifier_tag _tag,Vec2 _valueRange,int _weight);
	Modifier();
	virtual bool Deserialize(const rapidjson::Value& obj);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;
	
	void SetName(std::string _name);
	std::string GetName();
	void SetModifierType(modifier_type _type);
	modifier_type GetModifierType();
	void SetValue(int _value);
	void SetValueRange(Vec2 _value);
	Vec2 GetValueRange();
	int GetValue();

	std::string valueToString();
	void SetWeight(int _weight);

	void SetTag(modifier_tag _tag);
	modifier_tag GetTag();
	
	std::string name;
	modifier_type type;
	modifier_tag tag;
	int value;
	Vec2 value_range;

	int weight;

};


class Item: public Object, public JsonBase
{
public:
	Item();
	

	void SetSizeByItemType();
	void RenderTooltip();
	void OnRender();
	virtual bool Deserialize(const rapidjson::Value& obj);
	
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;

	item_type GetItemType() const { return _itemType; }
	void SetItemType(item_type itemType) { _itemType = itemType; }
	int GetItemId() const { return _itemId; }
	void SetItemId(int id) { _itemId = id; }

	Sprite GetTexture();


	void AddModifier(Modifier* m);
	void SetBaseStat();
	void AddRandomModifier();
	std::vector<Modifier*> GetModifiers();

	void SetRarity(int _rarity);

	static void FillModifiersFile();


	std::string GetName();

	Vec2 GetGridSize();
	void SetGridSize(Vec2 size);

	Modifier GetBaseStat();


	void SetTexture();
	Sprite _texture;
	bool render;

	std::string _name;
	Vec2 _grid_size;
	static Vec2 _default_item_size;


	bool isGroundItem;
	item_type _itemType;
	int _itemId;

	Modifier BaseStat;

	//std::unordered_map<modifier_type*, Modifier*> 
	std::vector<Modifier*> Modifiers;
	
	static std::vector<Modifier*> AllModifiers;
	static std::vector<Modifier*> AllBaseStats;


	int implicit_count=0;
	int implicit_count_max=1;
	int affix_count = 0;
	int affix_count_max = 0;


	item_rarity rarity=NORMAL;

	
	

};

