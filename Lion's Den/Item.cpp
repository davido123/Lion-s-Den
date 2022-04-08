#include "Item.h"
#include "Settings.h"
#include <iostream>
#include <IO/Mouse.h>
#include <random>
#include <Render/Drawer.h>

std::vector<Modifier*> Item::AllModifiers;
std::vector<Modifier*> Item::AllBaseStats;
std::random_device rd;
std::mt19937 mt(rd());
Vec2 Item::_default_item_size;


Item::Item()
{
	_texture.SetFrameSize(GetSize());
    UpdateId();
    render = false;
}



void Item::SetSizeByItemType()
{
    switch (_itemType)
    {
    case HELMET:   SetGridSize(Vec2(2, 2));_name="Helmet"; break;
    case ARMOR:   SetGridSize(Vec2(2,3));_name="Armour";break;
    case MAINHAND:SetGridSize(Vec2(2,3));_name="Sword";break;
    case OFFHAND: SetGridSize(Vec2(2,3));_name="Shield";break;
    case BELT: SetGridSize(Vec2(2,1));_name="Belt";break;
    case AMULET: SetGridSize(Vec2(1,1));_name="Amulet";break;
    case RING: SetGridSize(Vec2(1,1));_name="Ring";break;
    case BOOTS: SetGridSize(Vec2(2,2));_name="Boots";break;
    case GLOVES: SetGridSize(Vec2(2,2));_name="Gloves";break;
    default:     SetGridSize(Vec2(1,1));_name="Przedmiot";break;
    }
    



}




Sprite Item::GetTexture()
{
    return _texture;
}

void Item::AddModifier(Modifier* m)
{
    std::uniform_int_distribution<int> dist(m->GetValueRange().x,m->GetValueRange().y );
    int rnd = dist(mt);
    m->SetValue(rnd);

    if (m->GetModifierType() == IMPLICIT && implicit_count >= implicit_count_max)
    {
		printf("Item cant have anymore implicits\n");
        return;
    }
	if (m->GetModifierType() == AFFIX && affix_count >= affix_count_max)
    {
		printf("Item cant have anymore affixes\n");
        return;
    }

    for (auto i : Modifiers)
    {
        if (i->tag==m->tag)
        {
			printf("There is mod with that tag already\n");
            return;
        }
    }

	

    Modifiers.push_back(m);
}

/*
std::string ItemNames[] = { "Helmet","Armour","Sword","Shield","Belt","Amulet","Ring","Boots","Gloves" };


std::string IncreasedDamageNames[] = { "Increased physical damage","Increased Elemental damage","Increased Fire damage","Increased cold damage","Increased Lightning damage"};
Vec2 IncreasedDamageValues[] = {Vec2(5,10),Vec2(10,20),Vec2(20,30)};

std::string AddedDamageNames[]={ "Added physical damage","Added Fire damage","Added cold damage","Added Lightning damage" };


Vec2 AddedPhysicalDamageValues[] = {Vec2(1,3),Vec2(4,6),Vec2(20,30)};
Vec2 AddedFireDamageValues[] = {Vec2(1,3),Vec2(4,6),Vec2(20,30)};
Vec2 AddedColdDamageValues[] = {Vec2(1,3),Vec2(4,6),Vec2(20,30)};
Vec2 AddedLightningDamageValues[] = {Vec2(1,3),Vec2(4,6),Vec2(20,30)};

//std::string IncreasedArmour;

*/

void Item::SetBaseStat()
{
	 //SetBaseStat;
	switch (_itemType) {
        case MAINHAND:
        {
			BaseStat=Modifier("Physical Damage",IMPLICIT,BASE_PHYS,Vec2(10,20),1000);
			
		    break;
        }
        case OFFHAND:
        {
			BaseStat=Modifier("Armour",IMPLICIT,BASE_ARMOUR,Vec2(10,20),1000);
            
            break;
        }
        case HELMET:
        {
			BaseStat=Modifier("Armour",IMPLICIT,BASE_ARMOUR,Vec2(10,20),1000);
            
            break;
        }
        case ARMOR:
        {
			BaseStat=Modifier("Armour",IMPLICIT,BASE_ARMOUR,Vec2(10,20),1000);
            
            break;
        }
        case BOOTS:
        {
			BaseStat=Modifier("Armour",IMPLICIT,BASE_ARMOUR,Vec2(10,20),1000);
            
            break;
        }
        case GLOVES:
        {
			BaseStat=Modifier("Armour",IMPLICIT,BASE_ARMOUR,Vec2(10,20),1000);
            
            break;
        }
	    case BELT:
        {
			BaseStat=Modifier("Added Maximum Life",IMPLICIT,BASE_HP,Vec2(10,20),1000);
            
		    break;
        }
        case AMULET:
        {
			BaseStat=Modifier("Added Maximum Life",IMPLICIT,BASE_HP,Vec2(10,20),1000);
            
		    break;
        }
        case RING:
	    {
			BaseStat=Modifier("Added Maximum Life",IMPLICIT,BASE_HP,Vec2(10,20),1000);
            
		    break;
        }

		
        //default:
            //do nothing.
    }
	std::uniform_int_distribution<int> distt(BaseStat.value_range.x, BaseStat.value_range.y);
	BaseStat.value = (int)distt(mt);
    

}

void Item::AddRandomModifier()
{
    if (rarity == NORMAL)affix_count_max = 0;
    if (rarity == MAGIC)affix_count_max = 1;
    if (rarity == RARE)affix_count_max = 2;
    if (rarity == UNIQUE)affix_count_max = 3;

	std::vector<Modifier*> tmpMods;
	switch (_itemType) {
        case MAINHAND:
        {
            for (auto i : AllModifiers)
            {
                if(i->tag == INC_PHYS || i->tag ==INC_ELE || i->tag ==INC_FIRE || i->tag ==INC_COLD ||i->tag == INC_LIGHTNING ||i->tag == ADD_PHYS ||i->tag == ADD_FIRE ||i->tag == ADD_COLD ||i->tag == ADD_LIGHTNING)
                tmpMods.push_back(i);
            }

		    break;
        }
        case OFFHAND:
        {
			for (auto i : AllModifiers)
            {
                if(i->tag == INC_PHYS ||i->tag == INC_ELE ||i->tag == INC_FIRE ||i->tag == INC_COLD || i->tag ==INC_LIGHTNING || i->tag ==ADD_PHYS || i->tag ==ADD_FIRE ||i->tag == ADD_COLD || i->tag ==ADD_LIGHTNING||i->tag ==MAX_HP||i->tag ==MAX_MP||i->tag ==ADD_ARMOUR||i->tag ==INC_ARMOUR)
                tmpMods.push_back(i);
            }
			
                
            break;
        }
        case HELMET:
        case ARMOR:
        case BOOTS:
        case GLOVES:
        {
			for (auto i : AllModifiers)
            {
                
                   
                if  (i->tag ==MAX_HP ||i->tag == MAX_MP ||i->tag == INC_HP ||i->tag == INC_MP || i->tag ==ADD_ARMOUR || i->tag ==INC_ARMOUR)
                {
					tmpMods.push_back(i);
                }
                
            }
		    break;
        }
	    case BELT:
        case AMULET:
        case RING:
	    {
			for (auto i : AllModifiers)
            {
                if(i->tag == MAX_HP || i->tag ==MAX_MP || i->tag ==INC_HP ||i->tag == INC_MP || i->tag ==HP_REG ||i->tag == MP_REG )
                tmpMods.push_back(i);
            }
		    break;
        }
        //default:
            //do nothing.
    }
	int sum_of_weight = 0;
    for(auto i:tmpMods) sum_of_weight += i->weight;
	std::uniform_int_distribution<int> dist(0, sum_of_weight);
    int rnd = dist(mt);
    for (auto i : tmpMods) {
        if (rnd < i->weight)
        {
			AddModifier(i);
            break;
        }
        rnd -= i->weight;
    }

   

}

std::vector<Modifier*> Item::GetModifiers()
{
    return Modifiers;
}


void Item::SetRarity(int _rarity)
{
    rarity = (item_rarity)_rarity;
}


void Item::FillModifiersFile()
{
	
    for (auto i : Item::AllModifiers)
    {
        delete i;
    }
    AllModifiers.clear();


	Modifier* a;
    //Inc Phys Mods
    a=new Modifier("Increased Physical Damage",AFFIX,INC_PHYS,Vec2(5,10),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Physical Damage",AFFIX,INC_PHYS,Vec2(11,20),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Physical Damage",AFFIX,INC_PHYS,Vec2(21,30),100);
	Item::AllModifiers.push_back(a);
    //Inc Ele Mods
	a=new Modifier("Increased Elemental Damage",AFFIX,INC_ELE,Vec2(5,10),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Elemental Damage",AFFIX,INC_ELE,Vec2(11,20),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Elemental Damage",AFFIX,INC_ELE,Vec2(21,30),100);
	Item::AllModifiers.push_back(a);
    //Inc Fire Mods
	a=new Modifier("Increased Fire Damage",AFFIX,INC_FIRE,Vec2(5,10),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Fire Damage",AFFIX,INC_FIRE,Vec2(11,20),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Fire Damage",AFFIX,INC_FIRE,Vec2(21,30),100);
	Item::AllModifiers.push_back(a);
	//Inc Cold Mods
	a=new Modifier("Increased Cold Damage",AFFIX,INC_COLD,Vec2(5,10),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Cold Damage",AFFIX,INC_COLD,Vec2(11,20),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Cold Damage",AFFIX,INC_COLD,Vec2(21,30),100);
	Item::AllModifiers.push_back(a);
	//Inc Lightning Mods
	a=new Modifier("Increased Lightning Damage",AFFIX,INC_LIGHTNING,Vec2(5,10),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Lightning Damage",AFFIX,INC_LIGHTNING,Vec2(11,20),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Lightning Damage",AFFIX,INC_LIGHTNING,Vec2(21,30),100);
	Item::AllModifiers.push_back(a);
	

	a=new Modifier("Added Physical Damage",AFFIX,ADD_PHYS,Vec2(1,3),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Physical Damage",AFFIX,ADD_PHYS,Vec2(4,6),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Physical Damage",AFFIX,ADD_PHYS,Vec2(7,9),100);
	Item::AllModifiers.push_back(a);

	a=new Modifier("Added Fire Damage",AFFIX,ADD_FIRE,Vec2(1,3),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Fire Damage",AFFIX,ADD_FIRE,Vec2(4,6),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Fire Damage",AFFIX,ADD_FIRE,Vec2(7,9),100);
	Item::AllModifiers.push_back(a);
	//Inc Cold Mods
	a=new Modifier("Added Cold Damage",AFFIX,ADD_COLD,Vec2(1,3),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Cold Damage",AFFIX,ADD_COLD,Vec2(4,6),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Cold Damage",AFFIX,ADD_COLD,Vec2(7,9),100);
	Item::AllModifiers.push_back(a);
	//Inc Lightning Mods
	a=new Modifier("Added Lightning Damage",AFFIX,ADD_LIGHTNING,Vec2(1,3),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Lightning Damage",AFFIX,ADD_LIGHTNING,Vec2(4,6),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Lightning Damage",AFFIX,ADD_LIGHTNING,Vec2(7,9),100);
	Item::AllModifiers.push_back(a);


	a=new Modifier("Added Maximum Life",AFFIX,MAX_HP,Vec2(1,5),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Maximum Life",AFFIX,MAX_HP,Vec2(6,10),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Maximum Life",AFFIX,MAX_HP,Vec2(11,20),100);
	Item::AllModifiers.push_back(a);

	a=new Modifier("Increased Maximum Life",AFFIX,INC_HP,Vec2(1,5),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Maximum Life",AFFIX,INC_HP,Vec2(6,10),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Maximum Life",AFFIX,INC_HP,Vec2(11,20),100);
	Item::AllModifiers.push_back(a);

	a=new Modifier("Life Regeneration %",AFFIX,INC_HP,Vec2(0.1,0.2),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Life Regeneration %",AFFIX,INC_HP,Vec2(0.3,0.4),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Life Regeneration %",AFFIX,INC_HP,Vec2(0.5,0.6),100);
	Item::AllModifiers.push_back(a);

	a=new Modifier("Added Maximum Mana",AFFIX,MAX_MP,Vec2(1,5),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Maximum Mana",AFFIX,MAX_MP,Vec2(6,10),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Maximum Mana",AFFIX,MAX_MP,Vec2(11,20),100);
	Item::AllModifiers.push_back(a);

	a=new Modifier("Increased Maximum Mana",AFFIX,INC_MP,Vec2(1,5),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Maximum Mana",AFFIX,INC_MP,Vec2(6,10),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Maximum Mana",AFFIX,INC_MP,Vec2(11,20),100);
	Item::AllModifiers.push_back(a);


	a=new Modifier("Mana Regeneration %",AFFIX,MP_REG,Vec2(0.1,0.2),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Mana Regeneration %",AFFIX,MP_REG,Vec2(0.3,0.4),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Mana Regeneration %",AFFIX,MP_REG,Vec2(0.5,0.6),100);
	Item::AllModifiers.push_back(a);

	a=new Modifier("Added Armour",AFFIX,ADD_ARMOUR,Vec2(1,10),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Armour",AFFIX,ADD_ARMOUR,Vec2(11,20),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Added Armour",AFFIX,ADD_ARMOUR,Vec2(21,30),100);
	Item::AllModifiers.push_back(a);

	a=new Modifier("Increased Armour",AFFIX,INC_ARMOUR,Vec2(10,20),1000);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Armour",AFFIX,INC_ARMOUR,Vec2(21,30),500);
	Item::AllModifiers.push_back(a);
	a=new Modifier("Increased Armour",AFFIX,INC_ARMOUR,Vec2(31,40),100);
	Item::AllModifiers.push_back(a);


    //BASE STATS
	a=new Modifier("Armour",IMPLICIT,BASE_ARMOUR,Vec2(10,20),1000);
	Item::AllBaseStats.push_back(a);
	a=new Modifier("Added Maximum Life",IMPLICIT,BASE_HP,Vec2(10,20),1000);
	Item::AllBaseStats.push_back(a);
	a=new Modifier("Physical Damage",IMPLICIT,BASE_PHYS,Vec2(10,20),1000);
	Item::AllBaseStats.push_back(a);












}

std::string Item::GetName()
{
	return _name;
}
Vec2 Item::GetGridSize()
{
    return _grid_size;
}
void Item::SetGridSize(Vec2 size)
{
    _grid_size = size;
}
Modifier Item::GetBaseStat()
{
    return BaseStat;
}
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



bool Item::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
    writer->StartObject();
    writer->Key("Name");
    writer->String(_name.c_str());
	writer->Key("Size");
    writer->StartObject();
    writer->Key("x");
    writer->Int(_grid_size.x);
	writer->Key("y");
	writer->Int(_grid_size.y);
    writer->EndObject();
	writer->Key("ItemType");
    writer->Int(_itemType);
    writer->Key("ItemTypeName");
    writer->String(ToString(_itemType));
    writer->Key("Rarity");
    writer->Int(rarity);
    writer->Key("BaseModifier");
    BaseStat.Serialize(writer);

	writer->Key("ModifiersCount");
    writer->Int(Modifiers.size());
	writer->Key("Modifiers");
    writer->StartArray();
    for (auto i : Modifiers)
    {
        i->Serialize(writer);
    }
    writer->EndArray();



    writer->EndObject();
    return true;
}

bool Item::Deserialize(const rapidjson::Value& obj)
{
 
    _name = obj["Name"].GetString();
    SetGridSize(Vec2(obj["Size"]["x"].GetInt(), obj["Size"]["y"].GetInt()));
    SetItemType((item_type)obj["ItemType"].GetInt());
    SetRarity(obj["Rarity"].GetInt());
    BaseStat.Deserialize(obj["BaseModifier"]);


    int count = obj["ModifiersCount"].GetInt();
    for (int i = 0; i < count; i++)
    {
        Modifier* mod = new Modifier();
        mod->Deserialize(obj["Modifiers"][i]);
        Modifiers.push_back(mod);
    }
    




    return true;
}
inline void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}
void Item::RenderTooltip()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
			
    ImVec2 pos = ImVec2(Mouse::GetPos().x, Mouse::GetPos().y);
    ImVec2 size = ImVec2(300, 150);
   

    if (Mouse::GetPos().x > Window::GetWidth()-size.x)
    {
		pos = ImVec2(Mouse::GetPos().x-size.x, Mouse::GetPos().y);
    }

	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

	
    if (rarity == NORMAL)
    {
		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255,255,255,255));
    }
	 if (rarity == MAGIC)
    {
		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(24,148,208,255));
    }
	 if (rarity == RARE)
    {
		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(202,196,47,255));
    }
	  if (rarity == UNIQUE)
    {
		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(236,118,16,255));
    }
	
	
    ImGui::Begin("Item info",NULL,window_flags);
    ImGui::PopStyleColor();
    TextCentered(GetName());
	ImGui::Separator();
	TextCentered("Base Stats");
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(240,240,240,255));
    std::string name = BaseStat.GetName() + BaseStat.valueToString();
       TextCentered(name);

	ImGui::PopStyleColor();
	ImGui::Separator();
	TextCentered("Modifiers");

    for (auto i :Modifiers)
    {
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,255,0,255));
        std::string name=i->GetName()+ i->valueToString();
        TextCentered(name);
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

void Item::OnRender()
{
    if (render)// #ItemRender
    {
		if (Mouse::Intersect(GetPos(), GetSize(), Window::GetCamera()))
        {
            RenderTooltip();


        }

		_texture.Draw(GetPos() + Vec2(Window::GetCamera()->X(), Window::GetCamera()->Y()), Item::_default_item_size*GetGridSize());
        

    }



    

}


void Item::SetTexture()
{
    _texture.SetFrameSize(GetSize()*32);
    if (_itemType == HELMET)
        _texture.SetTexture("Items/Helmet.png");
    if (_itemType == ARMOR)
        _texture.SetTexture("Items/Armor.png");
    if (_itemType == MAINHAND)
        _texture.SetTexture("Items/Mainhand.png");
    if (_itemType == OFFHAND)
        _texture.SetTexture("Items/Offhand.png");
    if (_itemType == BELT)
        _texture.SetTexture("Items/Belt.png");
    if (_itemType == AMULET)
        _texture.SetTexture("Items/Amulet.png");
    if (_itemType == RING)
        _texture.SetTexture("Items/Ring.png");
    if (_itemType == BOOTS)
        _texture.SetTexture("Items/Boots.png");
    if (_itemType == GLOVES)
        _texture.SetTexture("Items/Gloves.png");
    if (_itemType == OTHER)
        _texture.SetTexture("Items/Other.png");
	


}

Modifier::Modifier(std::string _name, modifier_type _type, modifier_tag _tag, Vec2 _valueRange, int _weight)
{
    name = _name;
    type = _type;
    tag = _tag;
    value_range = _valueRange;
    weight = _weight;
}

Modifier::Modifier()
{
}

bool Modifier::Deserialize(const rapidjson::Value& obj)
{
    //SetPos(Vec2(obj["Position"]["x"].GetFloat(), obj["Position"]["y"].GetFloat()));
    name=obj["Name"].GetString();
    tag=(modifier_tag)obj["Tag"].GetInt();
    value = obj["Value"].GetInt();
    value_range = Vec2(obj["ValueRangeMin"].GetInt(), obj["ValueRangeMax"].GetInt());
    weight = obj["Weight"].GetInt();
	return true;
}

bool Modifier::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
    writer->StartObject();
	writer->Key("Name");
    writer->String(name.c_str());
    writer->Key("Tag");
    writer->Int(tag);
	writer->Key("Value");
    writer->Int(value);
	writer->Key("ValueRangeMin");
    writer->Int(value_range.x);
	writer->Key("ValueRangeMax");
    writer->Int(value_range.y);



	writer->Key("Weight");
    writer->Int(weight);
	writer->EndObject();


	return true;
}




void Modifier::SetName(std::string _name)
{
    name = _name;
}

std::string Modifier::GetName()
{
    return name;
}

void Modifier::SetModifierType(modifier_type _type)
{
    type = _type;
}

modifier_type Modifier::GetModifierType()
{
    return type;
}

void Modifier::SetValue(int _value)
{
    value = _value;
}

void Modifier::SetValueRange(Vec2 _value)
{
    value_range = _value;
}

Vec2 Modifier::GetValueRange()
{
    return value_range;
}
int Modifier::GetValue()
{
    return value;
}

std::string Modifier::valueToString()
{
    std::string str= ": "+int_to_str(value) + "   (" + int_to_str(value_range.x) + "-" + int_to_str(value_range.y) + ")";
  
    return str;
}

void Modifier::SetWeight(int _weight)
{
    weight = _weight;
}

void Modifier::SetTag(modifier_tag _tag)
{
    tag = _tag;
}

modifier_tag Modifier::GetTag()
{
    return tag;
}


