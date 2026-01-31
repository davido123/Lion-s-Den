#include <Slot.h>

Vec2 Slot::DefaultSlotSize;
inline const char* ToString(item_type v)
{
	switch (v)
	{
	case HELMET:   return "HELMET";
	case ARMOR:   return "ARMOR";
	case MAINHAND: return "MAINHAND";
	case OFFHAND: return "OFFHAND";
	case BELT: return "BELT";
	case AMULET: return "AMULET";
	case RING: return "RING";
	case BOOTS: return "BOOTS";
	case GLOVES: return "GLOVES";
	default:      return "Other";
	}
}

Slot::Slot() :
	Widget(GetPos(), GetSize())
{
	slot_type = OTHER;
	_slot_background.SetFrameSize(GetSize());
	SetBackgroundTexture();
}

Slot::Slot(Vec2 _gridpos, Vec2 _pos, Vec2 _size, item_type _slot_type) :
	Widget(_pos, _size)
{
	locked = false;
	GridPos = _gridpos;
	Id = 0;
	SetPos(_pos);
	SetSize(_size);
	slot_type = _slot_type;
	_slot_background.SetFrameSize(_size);
	is_mouse_slot = 0;
	Show(false);

	SetBackgroundTexture();
}

Slot::~Slot()
{
}

void Slot::SetSizeByItemType()
{
	switch (_Item->GetItemType())
	{
	case HELMET:   SetSize(Vec2(2, 2));
	case ARMOR:   SetSize(Vec2(2, 3));
	case MAINHAND:SetSize(Vec2(2, 3));
	case OFFHAND: SetSize(Vec2(2, 3));
	case BELT: SetSize(Vec2(2, 1));
	case AMULET: SetSize(Vec2(1, 1));
	case RING: SetSize(Vec2(1, 1));
	case BOOTS: SetSize(Vec2(2, 2));
	case GLOVES: SetSize(Vec2(2, 2));
	default:     SetSize(Vec2(1, 1));;
	}
}

bool Slot::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
	writer->StartObject();
	writer->Key("SlotType");
	writer->Int(slot_type);
	writer->Key("SlotTypeName");
	writer->String(ToString(slot_type));
	writer->Key("x");
	writer->Double(GridPos.x);
	writer->Key("y");
	writer->Double(GridPos.y);
	writer->Key("Item");
	if (have_item)
	{
		_Item->Serialize(writer);
	}
	else
	{
		writer->String("Empty");
	}

	writer->EndObject();
	return true;
}

bool Slot::Deserialize(const rapidjson::Value& obj)
{
	SetGridPos(Vec2(obj["x"].GetFloat(), obj["y"].GetFloat()));
	setSlottype((item_type)obj["SlotType"].GetInt());

	if (obj["Item"].IsObject())//check if item is not empty
	{
		_Item = new Item();
		_Item->Deserialize(obj["Item"]);
		_Item->_default_item_size = DefaultSlotSize;

		_Item->SetSize(DefaultSlotSize * _Item->GetGridSize());
		_Item->SetPos(GetPos());
		_Item->SetTexture();



		have_item = true;
		//have_item;
	}

	return true;
}

Item* Slot::GetItem()
{
	return _Item;
}

void Slot::ConnectItem(Item* item)
{
	
	_Item = item;
	have_item = true;
}
void Slot::DisconnectItem()
{
	have_item = false;
	_Item = NULL;
}

void Slot::SetBackgroundTexture()
{
	if (slot_type == HELMET)
		_slot_background.SetTexture("Inventory/Helmet_background.png");
	if (slot_type == ARMOR)
		_slot_background.SetTexture("Inventory/Armor_background.png");
	if (slot_type == MAINHAND)
		_slot_background.SetTexture("Inventory/Mainhand_background.png");
	if (slot_type == OFFHAND)
		_slot_background.SetTexture("Inventory/Offhand_background.png");
	if (slot_type == BELT)
		_slot_background.SetTexture("Inventory/Belt_background.png");
	if (slot_type == AMULET)
		_slot_background.SetTexture("Inventory/Amulet_background.png");
	if (slot_type == RING)
		_slot_background.SetTexture("Inventory/Ring_background.png");
	if (slot_type == BOOTS)
		_slot_background.SetTexture("Inventory/Boots_background.png");
	if (slot_type == GLOVES)
		_slot_background.SetTexture("Inventory/Gloves_background.png");
	if (slot_type == OTHER)
		_slot_background.SetTexture("Inventory/Other.png");
}

void Slot::OnUpdate()
{
	if (have_item)
	{
		if (is_mouse_slot)
		{
			_Item->SetPos(GetPos());
		}
	}
}

void Slot::OnRender()
{
	if (_visible)
	{
		if (!is_mouse_slot)
		{
			DrawBackgroundTexture();
		}
	}

	//DrawBackgroundTexture();
}

void Slot::DrawBackgroundTexture()
{
	_slot_background.Draw(GetPos() + Vec2(Window::GetCamera()->X(), Window::GetCamera()->Y()), GetSize());
}

bool Slot::IsLocked() {
	return locked;
}

void Slot::SetLocked(bool lock)
{
	locked = lock;
}

Vec2 Slot::GetGridPos()
{
	return GridPos;
}

void Slot::SetGridPos(Vec2 pos)
{
	GridPos = pos;
}

bool Slot::getHaveitem() const
{
	return have_item;
}

void Slot::setHaveitem(bool haveitem)
{
	have_item = haveitem;
}

bool Slot::getIsmouseslot() const
{
	return is_mouse_slot;
}

void Slot::setIsmouseslot(bool ismouseslot)
{
	is_mouse_slot = ismouseslot;
}

int Slot::getId() const
{
	return Id;
}

void Slot::setId(int Id)
{
	this->Id = Id;
}

Vec2 Slot::getGridPos() const
{
	return GridPos;
}

void Slot::setGridPos(Vec2 GridPos)
{
	this->GridPos = GridPos;
}

bool Slot::getLocked() const
{
	return locked;
}

void Slot::setLocked(bool locked)
{
	this->locked = locked;
}

item_type Slot::getSlottype() const
{
	return slot_type;
}

void Slot::setSlottype(item_type slottype)
{
	slot_type = slottype;
}