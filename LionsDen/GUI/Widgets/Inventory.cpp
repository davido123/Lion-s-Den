#include "GUI/Widgets/Inventory.h"
#include "GUI/Widgets/box.h"
#include <Render/Drawer.h>
#include "item.h"
#include <Settings.h>
#include <DebugUI.h>
#include <IO/JsonBase.h>
#include <ImGui/imgui_stdlib.h>
#include <Monster.h>

std::vector <Slot*> Inventory::grid;
std::vector <Slot*> Inventory::equipment;
bool Inventory::inventoryChanged;

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

Inventory::Inventory(const Vec2& pos, const Vec2& size, const std::string& font, int font_pt_size) :
	Box(pos.x, pos.y,size.x,size.y, font, font_pt_size)
	//Box(pos, size, font, font_pt_size)
	//const Box(x,y,w,h,font, font_pt_size)
{
	//SetSize(size);
	//SetPos(pos);



	SetBackGround("GUI/borders.png", Vec2::ZERO, 32);
	Show(false);

	_pt_size = font_pt_size;
	_text.Init(0, 0, "", font, font_pt_size);

	_text_size = Vec2::ZERO;
	_text_texture = nullptr;
	_has_icon = false;

	Vec2 UpperPos = Vec2::ZERO;
	Vec2 UpperSize = Vec2(GetSize().x, GetSize().y / 2);
	Vec2 LowerPos = Vec2(0, GetSize().y / 2);
	Vec2 LowerSize = Vec2(GetSize().x,GetSize().y / 2);

	Upper = new Box(UpperPos, UpperSize, font, font_pt_size);
	Lower = new Box(LowerPos, LowerSize, font, font_pt_size);

	IgnoreSorting(1);
	Connect(Upper);
	Connect(Lower);
	overlay = new Object();
	Connect(overlay);
	FillGrid();

	ConnectSlots();
	_EditItem = new Item;
	_sound_equip = new Audio("Data/Sounds/inventory.wav",false);
}
void Inventory::RecalcRelative()
{
    SetSizeCentered(_size_relative.x, _size_relative.y);
    SetPosCentered(_pos_relative.x, _pos_relative.y);
    SetBackGround("GUI/borders.png", Vec2::ZERO, 32);

	Vec2 UpperPos = Vec2::ZERO;
	Vec2 UpperSize = Vec2(GetSize().x, GetSize().y / 2);
	Vec2 LowerPos = Vec2(0, GetSize().y / 2);
	Vec2 LowerSize = Vec2(GetSize().x,GetSize().y / 2);

	Upper->SetSize(UpperSize);
    Upper->SetPos(UpperPos);
    Upper->SetBackGround("GUI/borders.png", Vec2::ZERO, 32);

	Lower->SetSize(LowerSize);
    Lower->SetPos(LowerPos);
    Lower->SetBackGround("GUI/borders.png", Vec2::ZERO, 32);




	grid.clear();
	equipment.clear();

	FillGrid();
	ConnectSlots();

}
Inventory::~Inventory()
{
}

bool Inventory::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
	writer->StartObject();

	writer->Key("Upper");
	writer->StartArray();
	for (std::vector<Slot*>::const_iterator it = equipment.begin(); it != equipment.end(); it++)
	{
		(*it)->Serialize(writer);
	}
	writer->EndArray();
	///////////////////////////
	writer->Key("Lower");
	writer->StartArray();
	for (std::vector<Slot*>::const_iterator it = grid.begin(); it != grid.end(); it++)
	{
		(*it)->Serialize(writer);
	}
	writer->EndArray();
	writer->EndObject();

	return true;
}
bool Inventory::Deserialize(const rapidjson::Value& obj)
{
	const rapidjson::Value& upper = obj["Upper"];

	for (rapidjson::SizeType i = 0; i < upper.Size(); i++)
	{
		const rapidjson::Value& c = upper[i];

		equipment[i]->Deserialize(c);
		equipment[i]->SetLocked(true);
		if (equipment[i]->have_item)
		{
			overlay->Connect(equipment[i]->GetItem());
		}
	}
	const rapidjson::Value& lower = obj["Lower"];

	for (rapidjson::SizeType i = 0; i < lower.Size(); i++)
	{
		const rapidjson::Value& c = lower[i];

		grid[i]->Deserialize(c);


		if (grid[i]->have_item)
		{
			overlay->Connect(grid[i]->GetItem());
		}
		//grid[i]->SetLocked(true);
	}

	for (auto i : grid)
	{
		if (i->getHaveitem())
		{
			SetLockedOnLoad(i->GetGridPos(),i->GetItem());
		}
	}

	

	return true;
}

void Inventory::SetStyle(const std::string& stylefile)
{
	if (_back.GetTexture() != nullptr) {
		SDL_DestroyTexture(_back.GetTexture());
		_back.SetTexture(nullptr);
	}

	if (stylefile.empty()) {
		return;
	}

	SDL_Texture* texture = Resources::GetTexture(stylefile);

	if (!texture) {
		return;
	}

	SDL_Rect rect = { 0, 0, static_cast<int>(GetSize().x), static_cast<int>(GetSize().y) };
	SDL_Texture* _back_texture = SDL_CreateTexture(Window::GetRenderer(), SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, rect.w, rect.h);
	SDL_SetTextureBlendMode(_back_texture, SDL_BLENDMODE_BLEND);

	Surface::BeginViewport(Vec2::ZERO, Vec2(rect.x, rect.y));

	//start rendering _back texture
	SDL_SetRenderTarget(Window::GetRenderer(), _back_texture);
	SDL_Color background = Window::GetBackgroundColor();
	SDL_SetRenderDrawColor(Window::GetRenderer(), background.r, background.g, background.b, 0);
	SDL_RenderClear(Window::GetRenderer());

	Surface::EndViewport();
	SDL_SetRenderTarget(Window::GetRenderer(), nullptr);
	_back.SetTexture(_back_texture);
}

void Inventory::ConnectPlayer(Player* player)
{
	pla = player;
}

void Inventory::FillGrid()
{
	SlotSize = Vec2(Lower->GetSize().x / 10 - 12 / 10, Lower->GetSize().y / 8 - 12 / 8);


	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			Vec2 size = Vec2(Lower->GetSize().x / 10 - 12 / 10, Lower->GetSize().y / 8 - 12 / 8);
			Vec2 pos = Upper->GetGlobalPos() + Vec2(6, 6);

			//helmet
			if (i == 4 && j == 0)
			{
				Slot* sl = new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, size * 2, HELMET);
				equipment.push_back(sl);
			}

			//armor
			if (i == 4 && j == 2) {
				Slot* sl = new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, Vec2(size.x * 2, size.y * 3), ARMOR);
				equipment.push_back(sl);
			}
			//boots
			if (i == 6 && j == 5)
			{
				Slot* sl = new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, size * 2, BOOTS);
				equipment.push_back(sl);
			}
			//1hand
			if (i == 2 && j == 2) {
				Slot* sl = new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, Vec2(size.x * 2, size.y * 3), MAINHAND);
				equipment.push_back(sl);
			}

			//2hand
			if (i == 6 && j == 2)
			{
				Slot* sl = new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, Vec2(size.x * 2, size.y * 3), OFFHAND);
				equipment.push_back(sl);
			}
			//gloves
			if (i == 2 && j == 5)
			{
				Slot* sl = new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, size * 2, GLOVES);
				equipment.push_back(sl);
			}
			//belt
			if (i == 4 && j == 5)
			{
				Slot* sl = new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, Vec2(size.x * 2, size.y * 1), BELT);
				equipment.push_back(sl);
			}
			//ring 1
			if (i == 4 && j == 6)equipment.push_back(new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, size, RING));
			//ring 2
			if (i == 5 && j == 6)equipment.push_back(new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, size, RING));
			//amulet
			if (i == 6 && j == 1)equipment.push_back(new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, size, AMULET));

			if (i == 9 && j == 0)equipment.push_back(new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, size, OTHER));
		}
	}

	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			Vec2 size = Vec2(Lower->GetSize().x / 10 - 12 / 10, Lower->GetSize().y / 8 - 12 / 8);
			Vec2 pos = Lower->GetGlobalPos() + Vec2(6, 6);

			Slot* sl = new Slot(Vec2(i, j), pos + size * Vec2::RIGHT * i + size * Vec2::DOWN * j, size, OTHER);
			
			grid.push_back(sl);
		}
	}
	MouseSlot = new Slot();
	//MouseSlot->DefaultSlotSize = Vec2((Upper->GetSize().x / 20), Upper->GetSize().y / 16);;

	for (int i = 0; i < equipment.size(); i++)
	{
		Vec2 size = Vec2((Upper->GetSize().x / 20), Upper->GetSize().y / 16);
		//equipment[i]->DefaultSlotSize = size;
	}
	Slot::DefaultSlotSize = SlotSize ;
}

void Inventory::setitem(Vec2 Pos, Vec2 Size) {
	for (int i = 0; i < grid.size(); i++)
	{
		if (grid[i]->GetGridPos() == Pos)
		{
			if (!grid[i]->IsLocked())
			{
				Item* it = new Item;
				it->SetItemType(ARMOR);
				it->SetSizeByItemType();
				//it->SetSize(grid[i]->GetSize());
				it->SetTexture();

				grid[i]->ConnectItem(it);
				grid[i]->SetLocked(true);
			}
		}
	}
}

bool Inventory::CheckOversizedIsPlaceable(Vec2 pos, Vec2 size)
{
	if (size.x + pos.x > 10)
	{
		return false;
	}
	else if (size.y + pos.y > 8)
	{
		return false;
	}
	else
	{
		for (int i = pos.x; i < pos.x + size.x; i++)
		{
			for (int j = pos.y; j < pos.y + size.y; j++)
			{
				int concat = ConcatInt(j, i);
				if (concat <= grid.size())
				{
					if (grid[concat]->IsLocked())
					{
						return false;
					}
				}
			}
		}
		return true;
	}
}
void Inventory::ConnectSlots()
{
	Lower->DisconnectAll();
	Upper->DisconnectAll();
	for (int i = 0; i < grid.size(); i++)
	{
		//grid[i]->overlay = overlay;
		Lower->Connect(grid[i]);
	}
	for (int i = 0; i < equipment.size(); i++)
	{
		//equipment[i]->overlay = overlay;
		Upper->Connect(equipment[i]);
	}
	MouseSlot->is_mouse_slot = true;
	overlay->Connect(MouseSlot);

	//Lower->Connect(MouseSlot);
}

void Inventory::Clear()
{
	SetRenderItems(false);
	for (auto i : equipment)
	{
		if (i->getHaveitem())
		{
			i->have_item = false;
			i->SetLocked(false);
			
			overlay->Disconnect(i->GetItem());
			i->DisconnectItem();
			delete i->GetItem();
		}
	}
	for (auto i : grid)
	{
		if (i->getHaveitem())
		{
			i->have_item = false;
			i->SetLocked(false);
			overlay->Disconnect(i->GetItem());
			i->DisconnectItem();
			delete i->GetItem();
		}
	}
}

int Inventory::ConcatInt(int a, int b)
{
	std::string s1 = std::to_string(a);
	std::string s2 = std::to_string(b);
	if (s1 == "0" && s2 != "0")
	{
		s1 = "";
	}
	return std::stoi(s1 + s2);
}
void Inventory::PullItem(Vec2 Pos)
{
	for (int i = 0; i < grid.size(); i++) {
		if (grid[i]->GetGridPos() == Pos) {
			if (grid[i]->GetItem() != NULL) {
				MouseSlot->ConnectItem(grid[i]->GetItem());

				MouseSlot->SetSize(grid[i]->GetItem()->GetGridSize() * SlotSize);

				Item* it = grid[i]->GetItem();
				grid[i]->DisconnectItem();

				for (int i = Pos.x; i < Pos.x + it->GetGridSize().x; i++) {
					for (int j = Pos.y; j < Pos.y + it->GetGridSize().y; j++) {
						int concat = ConcatInt(j, i);
						grid[concat]->SetLocked(false); 
						_sound_equip->Play();
						
					}
				}
				MouseSlot->have_item = true;
			}
		}
	}
}
void Inventory::PushItem(Vec2 Pos)
{
	for (int i = 0; i < grid.size(); i++) {
		if (grid[i]->GetGridPos() == Pos) {
			if (grid[i]->GetItem() == NULL) {
				if (CheckOversizedIsPlaceable(Pos, MouseSlot->GetItem()->GetGridSize())) {
					Item* it = MouseSlot->GetItem();
					it->SetPos(grid[i]->GetPos());
					grid[i]->ConnectItem(it);

					MouseSlot->have_item = false;
					MouseSlot->DisconnectItem();

					for (int i = Pos.x; i < Pos.x + it->GetGridSize().x; i++) {
						for (int j = Pos.y; j < Pos.y + it->GetGridSize().y; j++) {
							int concat = ConcatInt(j, i);
							grid[concat]->SetLocked(true); 
							_sound_equip->Play();
						}
					}
					grid[i]->have_item = true;
				}
			}
		}
	}
}


void Inventory::SetLockedOnLoad(Vec2 Pos,Item* it)
{
	for (int i = Pos.x; i < Pos.x + it->GetGridSize().x; i++) {
						for (int j = Pos.y; j < Pos.y + it->GetGridSize().y; j++) {
							int concat = ConcatInt(j, i);
							grid[concat]->SetLocked(true); 
							
						}
					}




}






void Inventory::EquipItem(Vec2 Pos)
{
	for (int i = 0; i < equipment.size(); i++) {
		if (equipment[i]->GetGridPos() == Pos) {
			if (equipment[i]->GetItem() == NULL)
			{
				Item* it = MouseSlot->GetItem();
				if (it->GetItemType() == equipment[i]->getSlottype() || equipment[i]->getSlottype() == OTHER)
				{
					it->SetPos(equipment[i]->GetPos());
					equipment[i]->ConnectItem(it);
					MouseSlot->DisconnectItem();
					_sound_equip->Play();
				}

				if (equipment[i]->getGridPos() == Vec2(9, 0))
				{
					equipment[i]->have_item = false;
					equipment[i]->SetLocked(false);
					overlay->Disconnect(equipment[i]->GetItem());

					equipment[i]->DisconnectItem();
					delete equipment[i]->GetItem();
				}

				inventoryChanged = true;
			}
		}
	}
}
void Inventory::UnEquipItem(Vec2 Pos)
{
	for (int i = 0; i < equipment.size(); i++) {
		if (equipment[i]->GetGridPos() == Pos) {
			if (equipment[i]->GetItem() != NULL)
			{
				MouseSlot->ConnectItem(equipment[i]->GetItem());
				MouseSlot->SetSize(equipment[i]->GetItem()->GetGridSize() * SlotSize);

				equipment[i]->DisconnectItem();
				inventoryChanged = true;
				_sound_equip->Play();
			}
		}
	}
}
void Inventory::TakeItem(Item* item)
{
	item->render = true;
	MouseSlot->ConnectItem(item);

	MouseSlot->SetSize(item->GetGridSize() * SlotSize);
	_sound_equip->Play();



}



void Inventory::DisconnectAllOverlay()
{
	overlay->DisconnectAll();
}
bool Inventory::IsItemOnHand()
{
	if (MouseSlot->GetItem() == NULL)
		return false;
	else
		return true;
}

void Inventory::SetRenderItems(bool render)
{
	for (int i = 0; i < grid.size(); i++)
	{
		if (grid[i]->have_item)
		{
			grid[i]->GetItem()->render = render;
		}
	}
	for (int i = 0; i < equipment.size(); i++)
	{
		if (equipment[i]->have_item)
		{
			equipment[i]->GetItem()->render = render;
		}
	}
}

void Inventory::OnUpdate()
{
	for (int i = 0; i < grid.size(); i++)
	{
		if (IsVisible())
		{
			if (Mouse::Intersect(grid[i]->GetPos(), grid[i]->GetSize(), Object::GetCamera()))
			{
				if (Mouse::Pressed(MOUSE_LEFT))
				{
					if (_mbtpressed)
					{
						if (IsItemOnHand())
						{
							PushItem(grid[i]->GetGridPos());
							_mbtpressed = false;
						}
						else
						{
							PullItem(grid[i]->GetGridPos());
							_mbtpressed = false;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < equipment.size(); i++)
	{
		if (IsVisible())
		{
			if (Mouse::Intersect(equipment[i]->GetPos(), equipment[i]->GetSize(), Object::GetCamera()))
			{
				if (Mouse::Pressed(MOUSE_LEFT))
				{
					if (_mbtpressed)
					{
						if (IsItemOnHand())
						{
							EquipItem(equipment[i]->GetGridPos());
							_mbtpressed = false;
						}
						else
						{
							UnEquipItem(equipment[i]->GetGridPos());
							_mbtpressed = false;
						}
					}
				}
			}
		}
	}

	MouseSlot->SetPos(Mouse::GetPos());
}
inline void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

void Inventory::OnRender() {
	if (_visible)// #Inventory
	{
		bool isdead = false;
		bool isnearby = false;
		for (auto i : Monster::MonsterList)
		{
			if (i->IsDead())
			{
				isdead = true;
				Vec2 ppos = Vec2(round(pla->GetPos().x), round(pla->GetPos().y));
				Vec2 mtar = Vec2(round(i->GetPos().x), round(i->GetPos().y));
				Vec2 a = ppos - mtar;
				if (a.GetLength() <= 100)
				{
					isnearby = true;
					i->isNearbyToLoot = true;
				}
				else
				{
					i->isNearbyToLoot = false;
				}
			}
		}
		if (isdead && isnearby)
		{
			Vec2 WindowSize = Vec2(Lower->GetSize().x, Lower->GetSize().y / 2);
			Vec2 WindowPos = Lower->GetGlobalPos() + Vec2(0, Lower->GetSize().y + 10);

			ImVec2 ImWindowPos = ImVec2(WindowPos.x, WindowPos.y);
			ImVec2 ImWindowSize = ImVec2(WindowSize.x, WindowSize.y);;

			ImGui::SetNextWindowPos(ImWindowPos);
			ImGui::SetNextWindowSize(ImWindowSize);

			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			

			// #RenderMonsterLoot

			ImGui::Begin("Nearby Monster Loot", NULL, window_flags);

			static ImGuiTableFlags flags = 0;
			flags |= ImGuiTableFlags_SizingFixedFit;

			ImGui::BeginTable("Items", 8, flags);
			
				for (auto i : Monster::MonsterList)
				{
					
					if (i->isNearbyToLoot)
					{
						if (i->GroundItems!=NULL) 
						if(i->GroundItems->getHaveitem())
						{

						
							
							ImGui::TableNextColumn();
							ImTextureID my_tex_id1 = i->GroundItems->GetItem()->GetTexture().GetTexture();
							ImU32 bgcolor = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
							

							//GroundItems
							if (ImGui::ImageButton(my_tex_id1, ImVec2(i->GroundItems->GetItem()->GetGridSize().x*i->GroundItems->DefaultSlotSize.x, i->GroundItems->GetItem()->GetGridSize().y*i->GroundItems->DefaultSlotSize.y),ImVec2(0,0),ImVec2(1,1),0,ImVec4(0.06,0.06,0.06,0.940)))
							{
								if (!IsItemOnHand())
								{
									TakeItem(i->GroundItems->GetItem());
									overlay->Connect(MouseSlot->GetItem());
									i->GroundItems->DisconnectItem();
									//i->GroundItems.clear();
								}
								
							}
							
							}
						
						
						
					}
					//ImGui::Text("Item %d", i->GetItem()->GetName());
				}
				ImGui::EndTable();
			

			ImGui::End();
		}
		ImGui::Begin("Inventory debug");
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Informacje"))
			{
				ImGui::Text("Inventory vector grid size:%i ", grid.size());
				ImGui::Text("Is item on hand %d ", IsItemOnHand());
				ImGui::Text("mbt pressed %d ", _mbtpressed);
				ImGui::Text("mbt released %d ", _mbtreleased);


				ImGui::Separator();
				 size_t len = overlay->_ChildrenList.size();

				for (size_t i = 0; i < len; ++i) {
					ImGui::Text("overlay: id %d",overlay->_ChildrenList[i]->GetId() );
					;
				}

				//ImGui::NewLine();
				ImGui::Separator();
				ImGui::Text("ITEM HELD IN HAND");

				if (IsItemOnHand())
				{
					//std::string text2 = grid[0]->GetItem()->GetName();
					ImGui::Text("id: %d", MouseSlot->GetItem()->GetId());
					ImGui::Text("render: %d", MouseSlot->GetItem()->render);
					ImGui::Text("Name: %s", MouseSlot->GetItem()->GetName().c_str());
					ImGui::Text("Item Type: %s", ToString(MouseSlot->GetItem()->GetItemType()));
					ImGui::Text("Size: %f x %f", MouseSlot->GetItem()->GetSize().x, MouseSlot->GetItem()->GetSize().y);
					ImGui::Text("Grid Size: %f x %f", MouseSlot->GetItem()->GetGridSize().x, MouseSlot->GetItem()->GetGridSize().y);
					ImGui::Text("Position: %f x %f", MouseSlot->GetItem()->GetPos().x, MouseSlot->GetItem()->GetPos().y);
					ImTextureID my_tex_id1 = MouseSlot->GetItem()->_texture.GetTexture();
					ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
					ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
					ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white 
					ImGui::Image(my_tex_id1, ImVec2(MouseSlot->GetItem()->GetGridSize().x*MouseSlot->DefaultSlotSize.x, MouseSlot->GetItem()->GetGridSize().y*MouseSlot->DefaultSlotSize.y), uv_min, uv_max, tint_col, border_col);
				}

				












				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Przedmioty"))
			{
				for (int i = 0; i < equipment.size(); i++)
				{
					ImGui::Text("have item: %i ", equipment[i]->getHaveitem());
					if (equipment[i]->GetItem() != NULL)
					{
						ImGui::Text("item name: %s ", equipment[i]->GetItem()->GetName().c_str()); ImGui::SameLine();
						ImGui::Text("Size: %f x %f", equipment[i]->GetItem()->GetGridSize().x, equipment[i]->GetItem()->GetGridSize().y);
					}
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Przedmioty2"))
			{
				for (int i = 0; i < grid.size(); i++)
				{
					ImGui::Text("have item: %i ", grid[i]->getHaveitem());
					if (grid[i]->GetItem() != NULL)
					{
						ImGui::Text("item name: %s ", grid[i]->GetItem()->GetName().c_str()); ImGui::SameLine();
						ImGui::Text("Size: %f x %f", grid[i]->GetItem()->GetGridSize().x, grid[i]->GetItem()->GetGridSize().y);
					}
				}
				ImGui::EndTabItem();
			}

			// #ADDITEM
			if (ImGui::BeginTabItem("DodajPrzedmiot"))
			{
				ImGui::InputText("Item Name", &_EditItem->_name);

				const char* items[] = { "HELMET", "ARMOR", "MAINHAND", "OFFHAND","BELT","AMULET","RING","BOOTS","GLOVES","OTHER" };
				static int item = 0;
				ImGui::Combo("Item Type", &item, items, IM_ARRAYSIZE(items));

				ImGui::Text("Item type int selected %i", item);

				if (ImGui::Button("Create"))
				{
					_EditItem->_itemType = (item_type)item;
					_EditItem->SetSizeByItemType();
					_EditItem->SetSize(Slot::DefaultSlotSize * _EditItem->GetGridSize());
					_EditItem->_default_item_size = Slot::DefaultSlotSize;
					_EditItem->SetTexture();
					_EditItem->render = true;
					_EditItem->rarity = UNIQUE;

					while (_EditItem->Modifiers.size() < _EditItem->rarity)
					{
						_EditItem->AddRandomModifier();
					}

					overlay->Connect(_EditItem);
					MouseSlot->ConnectItem(_EditItem);

					_EditItem = new Item();
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear"))
				{
					delete _EditItem;
					_EditItem = new Item();

					for (auto i : grid)
					{
						if (i->getHaveitem())
						{
							overlay->Disconnect(i->GetItem());

							i->DisconnectItem();
							delete i->GetItem();
						}
						i->SetLocked(false);
					}

					ImGui::Text("clicked clear");
				}

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		ImGui::End();

		_back.Draw(Object::GetGlobalPos(), Object::GetSize(), Object::GetCamera());

		//equipment grid
		for (int i = 0; i < equipment.size(); i++)
		{
			Drawer::StaticRect(equipment[i]->GetPos(), equipment[i]->GetSize(), COLOR_GRAY, false);
		}
		for (int i = 0; i < equipment.size(); i++)
		{
			if (equipment[i]->GetItem() != NULL)
			{
				Drawer::StaticRect(equipment[i]->GetPos(), equipment[i]->GetSize(), COLOR_BLUE, false);
				if (Mouse::Intersect(equipment[i]->GetPos(), equipment[i]->GetSize(), Object::GetCamera()))
				{
					Drawer::StaticRect(equipment[i]->GetPos(), equipment[i]->GetSize(), COLOR_RED, false);
				}
			}
			else
			{
				if (Mouse::Intersect(equipment[i]->GetPos(), equipment[i]->GetSize(), Object::GetCamera()))
				{
					Drawer::StaticRect(equipment[i]->GetPos(), equipment[i]->GetSize(), COLOR_RED, false);
				}
			}
		}
		for (int i = 0; i < grid.size(); i++)
		{
			if (grid[i]->GetItem() != NULL)
			{
				if (Mouse::Intersect(grid[i]->GetPos(), grid[i]->GetSize() * grid[i]->GetItem()->GetGridSize(), Object::GetCamera()))
				{
					Drawer::StaticRect(grid[i]->GetPos(), grid[i]->GetSize() * grid[i]->GetItem()->GetGridSize(), COLOR_RED, false);
				}
			}
			else
			{
				if (Mouse::Intersect(grid[i]->GetPos(), grid[i]->GetSize(), Object::GetCamera()))
				{
					Drawer::StaticRect(grid[i]->GetPos(), grid[i]->GetSize(), COLOR_RED, false);
				}
			}
			
			if (grid[i]->IsLocked())
			{
				//Drawer::StaticRect(grid[i]->GetPos(), grid[i]->GetSize(), COLOR_RED, false);
			}
			


		}
		if (IsItemOnHand())
		{
			Drawer::StaticRect(Mouse::GetPos(), MouseSlot->GetItem()->GetGridSize() * SlotSize, COLOR_BLACK, false);
		}
	}
}