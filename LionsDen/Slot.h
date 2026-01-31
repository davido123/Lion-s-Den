#pragma once


#include "Render/Sprite.h"
#include "GUI/Widget.h"
#include "GUI/Widgets/box.h"
#include "GUI/Text.h"
#include "Core/Resources.h"
#include <IO/JsonBase.h>
#include <Item.h>

class Slot : public Widget, public JsonBase {
public:
	Slot(Vec2 gridpos, Vec2 _pos, Vec2 _size, item_type _slot_type);
	Slot();
	~Slot();

	virtual bool Deserialize(const rapidjson::Value& obj);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;


	void SetSizeByItemType();

	Vec2 GetGridPos();
	void SetGridPos(Vec2 pos);

	bool IsLocked();
	void SetLocked(bool lock);


	Item* GetItem();
	
	void ConnectItem(Item* _item);
	void DisconnectItem();
	void SetBackgroundTexture();
	void OnUpdate();
	void OnRender();
	void DrawBackgroundTexture();
	

	bool have_item;
	bool is_mouse_slot;
	static Vec2 DefaultSlotSize;
	static Object* overlay;
	Item* _Item;
	bool locked;
private:
	int Id;
	Vec2 GridPos;
	

	

	
	Sprite _slot_background;
	item_type slot_type;
	
public:
    bool getHaveitem() const;
    void setHaveitem(bool haveitem);

    bool getIsmouseslot() const;
    void setIsmouseslot(bool ismouseslot);

    int getId() const;
    void setId(int Id);

    Vec2 getGridPos() const;
    void setGridPos(Vec2 GridPos);

    bool getLocked() const;
    void setLocked(bool locked);

    item_type getSlottype() const;
    void setSlottype(item_type slottype);

};