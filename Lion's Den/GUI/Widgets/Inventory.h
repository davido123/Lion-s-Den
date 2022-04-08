
#include <string>
#include <sstream>

#include "Render/Sprite.h"
#include "GUI/Widget.h"
#include "GUI/Widgets/box.h"
#include "GUI/Text.h"
#include "Core/Resources.h"
#include <Player.h>
#include "Item.h"
#include <IO/JsonBase.h>
#include <Slot.h>




class Inventory : public Box, public JsonBase {
public:
	Inventory(const Vec2& pos, const Vec2& size, const std::string& font, int font_pt_size);

	void RecalcRelative();

	virtual ~Inventory();

	virtual bool Deserialize(const rapidjson::Value& obj);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;

	virtual void OnRender();

	void SetStyle(const std::string& stylefile);
	void UpdateValues(Player* player);
	void ConnectPlayer(Player* player);
	void EquipItem(Vec2 Pos);
	void UnEquipItem(Vec2 Pos);

	void TakeItem(Item* item);

	void DisconnectAllOverlay();

	static std::vector <Slot*> grid;
	static std::vector <Slot*> equipment;
	Slot* MouseSlot;
	void FillGrid();

	void OnUpdate();

	void setitem(Vec2 Pos, Vec2 Size);

	bool CheckOversizedIsPlaceable(Vec2 pos, Vec2 size);

	void ConnectSlots();

	void Clear();


	int ConcatInt(int a, int b);

	void PullItem(Vec2 Pos);
	void PushItem(Vec2 Pos);
	void SetLockedOnLoad(Vec2 Pos, Item* it);
	bool IsItemOnHand();

	void SetRenderItems(bool render);

	bool _mbtpressed;
	bool _mbtreleased;

	Item* _EditItem;

	Vec2 SlotSize;


	static bool inventoryChanged;
	
	Object* overlay;

	Audio* _sound_equip;



protected:
	Box* Upper;
	Box* Lower;
	Player* pla;
	

};