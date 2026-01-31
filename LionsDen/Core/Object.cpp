/*!
  boldtemple Gaming ©, http://gaming.boldtemple.net
  OpenSource Project - Check Development, License & Readme Sections.

  BGE - boldtemple Cross Platform Game Engine
  /Core/Object.cpp
!*/

#include "Object.h"
#include "IO/Mouse.h" //FIXME WTF? THIS SHIT DOESNT COMPILE IN Object.h
#include <algorithm>
#include "GUI/Widgets/Box.h"
#include "GUI/Widgets/Button.h"
#include "Box2d/box2d.h"


ObjListType Object::DeleteCandidates;
std::unordered_map<std::string, Object*> Object::_AllObjects;
int Object::_last_id = 0;
int Object::_object_count = 0;

const float ppm = 32.0f;


Object* Object::FindByLabel(const std::string &label){
   auto found = _AllObjects.find(label);

   if(found != _AllObjects.end()){
       return found->second;
   }else{
       return nullptr;
   }
}

Object::Object() {
    SetDefaults();
}

Object::Object(const std::string& label){
   SetDefaults();
   SetLabel(label);
}

Object::~Object() {
    if (_owner) {
        _owner->Disconnect(this);
    }

    while (!_ChildrenList.empty()) {
        auto it = _ChildrenList.begin();
        delete (*it);
    }

    //if (_id == _last_id - 1) {
    //    _last_id = 0; // reset when all objects are deleted
    //}

    _AllObjects.erase(_label);
}

void Object::DeleteLater(){
    if(!_delete_later){
        DeleteCandidates.push_back(this);
    }
}

void Object::SetGlobalPos(const Vec2& new_pos){
    _pos = new_pos;
    _global_pos = _pos;

}

void Object::SetPos(const Vec2& new_pos) {
    _pos = new_pos;

    if (_owner) {
        auto tmp_global = _owner->GetGlobalPos();
        //_global_pos = _owner->GetGlobalPos() + GetPos();
        _global_pos = _owner->GetGlobalPos() + GetPos();
    } else {
        _global_pos = _pos;
    }

    if (!_ChildrenList.empty())
        MoveChildern(Vec2::ZERO);
}

void Object::SetPosCentered(const int& x, const int& y)
{
    Vec2 window=Window::GetCamera()->GetViewport();
    _pos = Vec2(window.x/100*x, window.y / 100 * y) - GetSize()/2;

    if (_owner) {
        auto tmp_global = _owner->GetGlobalPos();
        //_global_pos = _owner->GetGlobalPos() + GetPos();
        _global_pos = _owner->GetGlobalPos() + GetPos();
    }
    else {
        _global_pos = _pos;
    }
    if (!_ChildrenList.empty())
    {
        MoveChildern(Vec2::ZERO);
    }
}



void Object::Move(const Vec2& delta_pos) {
    _pos += delta_pos;
    float friction = 0.05f;
    Vec2 vel = GetVel();
    vel = vel - Vec2(vel * friction);
   
		  if (vel.x < 0.1f && vel.x > -0.1f && vel.x!=0)
            {
                vel.x = trunc(vel.x);
            }
	        if (vel.y < 0.1f && vel.y > -0.1f && vel.y!=0)
            {
                vel.y = trunc(vel.y);
            }

    
  
    SetVel(vel);

    if (_owner) {
        _global_pos = _owner->GetGlobalPos() + GetPos();
    } else {
        _global_pos = _pos;
    }

    if (!_ChildrenList.empty())
        MoveChildern(Vec2::ZERO);
}

Object* Object::GetOwner() const {
    return _owner;
}

void Object::SetOwner(Object* obj) {
    _owner = obj;
}

void Object::Connect(Object* obj) {
    if (obj->_owner != nullptr) {
        std::cout << "[Warning!] Object {label: '"
                  << obj->_label <<"', id: "
                  << obj->_id << "} already connected to {label: '"
                  << this->_label <<"', id: "
                  << this->_id << "} " << std::endl;
    } 
    else if (FindChild(obj) == _ChildrenList.end()) {
        _ChildrenList.push_back(obj);
        obj->SetOwner(this);
        obj->Move(Vec2::ZERO); // update global pos
    }
}

void Object::Disconnect(Object* obj) {
    auto iter = FindChild(obj);
    if (iter != _ChildrenList.end()) {
        _ChildrenList.erase(iter);
        obj->SetOwner(nullptr);
    }
}

void Object::DisconnectAll()
{
    for (int i=0 ;i<_ChildrenList.size();i++ )
    {
        Disconnect(_ChildrenList[i]);
    }
    _ChildrenList.clear();
}



void Object::SetType(obj_type type) {
    _type = type;
}

ObjListType Object::GetChildrenList() {
    return _ChildrenList;
}

const Vec2& Object::GetPos() const {
    return _pos;
}

const Vec2& Object::GetGlobalPos() const {
    return _global_pos;
}

void Object::SetLabel(const std::string& label){
    auto found = _AllObjects.find(label);

    if(found != _AllObjects.end()){
        std::cerr << "Label '" << label << " has been already taken!" << std::endl;
    }else{
        _label = label;
        _AllObjects[label] = this;
    }
}

const std::string& Object::GetLabel(){
    return _label;
}

void Object::SetVel(const Vec2 &vel) {
    _vel = vel;
}





Vec2 Object::GetVel() const {
    /*
    
    if (_owner != nullptr) {
        return _owner->GetVel() + _vel;
    } else {
	
        return _vel;
    }
	*/
        return _vel;
    
}

obj_type Object::GetType() const {
    return _type;
}

int Object::GetId() const {
    return _id;
}

void Object::SetSize(const Vec2& size) {
    _size = size;
}
void Object::SetSizeCentered(const float& x, const float& y) {
    Vec2 window = Window::GetCamera()->GetViewport();
    _size = Vec2(window.x/100*x,window.y/100*y);
    
    if (_ChildrenList.empty()) {
        return;
    }

    size_t len = _ChildrenList.size();

    for (size_t i = 0; i < len; ++i) {
        _ChildrenList[i]->SetSizeRelative(len);
        _ChildrenList[i]->SetPosRelative(i,len);
        if (_ChildrenList[i]->GetType() == OBJ_BUTTON)
        {
            Box* pc = static_cast<Box*>(_ChildrenList[i]);
            
            pc->SetText(pc->GetText());
            pc->CalcTextPos();
        }

    }
}
void Object::RecalcCentered()
{
    if (_ChildrenList.empty()) {
        return;
    }

    size_t len = _ChildrenList.size();

    for (size_t i = 0; i < len; ++i) {
        _ChildrenList[i]->SetSizeRelative(len);
        _ChildrenList[i]->SetPosRelative(i, len);
        if (_ChildrenList[i]->GetType() == OBJ_BUTTON)
        {
            Button* pc = static_cast<Button*>(_ChildrenList[i]);

            pc->SetText(pc->GetText());
            pc->CalcTextPos();
            pc->ReloadStyle();
        }

    }
}


void Object::SetSizeRelative(int num)
{
    Vec2 s= _owner->GetSize();
    SetSize(Vec2(s.x * 0.8,(s.y/(num*2+1) )));


}
void Object::SetPosRelative(int i, int num)
{
    Vec2 s = _owner->GetSize();
    SetPos(Vec2((s.x / 2) - (GetSize().x / 2), ((s.y / (num * 2 + 1))) +((s.y / (num * 2 + 1)) * 2 * i)));

}

const Vec2& Object::GetSize() const {
    return _size;
}


void Object::SetCamera(Camera* cam)
{
    _camera = cam;
}

Camera* Object::GetCamera()
{
    return _camera;
}

void Object::MoveChildern(const Vec2& delta_pos) {
    for (auto it = _ChildrenList.begin(); it != _ChildrenList.end(); it++) {
        (*it)->Move(delta_pos);
    }
}

ObjListType::iterator Object::FindChild(Object* obj) {
    for (auto it = _ChildrenList.begin(); it < _ChildrenList.end(); it++) {
        if ((*it)->_id == obj->_id) {
            return it;
        }
    }
    return _ChildrenList.end(); //not found
}

void Object::SetDefaults(){
    _delete_later = false;
    _owner = nullptr;
    _id = _last_id++;
    _type = OBJ_NONE;
    _ignore_click = false;
    _ignore_sorting = false;
    Object::_object_count++;
    SetVel(Vec2(0,0));
}

//if you want your object to be clicked call this function on update tick
void Object::CheckTop() {
	
    if(Mouse::Intersect(this)){
        const Surface::viewport* view = &(Surface::GetLastViewport());
		
        if(!_camera){
            SetCamera(Window::GetCamera());
        }

        if(Mouse::Intersect(view->_offset, view->_size, _camera)){
            OnMouse();

            if (!_ignore_click) {
                GUI::SetTopObject(this);
            }

            if(!_ignore_wheel){
                GUI::SetTopWheeled(this);
            }
        }
    }

}

void Object::IgnoreClick(bool ignored) {
    _ignore_click = ignored;
}

void Object::IgnoreSorting(bool ignored) {
    _ignore_sorting = ignored;
}

void Object::IgnoreWheel(bool ignored){
    _ignore_wheel = ignored;
}

bool Object::IsClickIgnored(){
    return _ignore_click;
}

bool Object::IsWheelIgnored(){
    return _ignore_wheel;
}

bool Object::IsDeletedLater(){
    return _delete_later;
}

void Object::UpdateId()
{
    _id = _last_id++;
}

void Object::OnUpdate() {
    //UpdateChildren();
}

void Object::OnRender() {
    //RenderChildren();

}

void Object::OnCollide(Object* obj) {

    /*
    printf("collided with %d", obj->GetId());
	Collider::UnregisterAttack(this);
    */

}

void Object::OnMouse(){

}

void Object::OnTopMouseEvent() {

}

void Object::OnTopMouseWheelEvent(){

}


Vec2 Object::GetOffset()
{
    return _offset;
}


void Object::SetOffset(Vec2 offset)
{
    _offset = offset;
}

void Object::UpdateChildren() {
    if (_ChildrenList.empty()) {
        return;
    }

    size_t len = _ChildrenList.size();

    for (size_t i = 0; i < len; ++i) {
        _ChildrenList[i]->OnUpdate();
        _ChildrenList[i]->UpdateChildren();
    }
}


void Object::RenderChildren() {
    if (_ChildrenList.empty()) {
        return;
    }

    size_t len = _ChildrenList.size();
    if (!_ignore_sorting)
    {
		std::sort(_ChildrenList.begin(), _ChildrenList.end(),sort);
    }
    for (size_t i = 0; i < len; ++i) {
        _ChildrenList[i]->OnRender();
        _ChildrenList[i]->RenderChildren();
    }
}

void Object::RenderChildrenYSorted() {
    if (_ChildrenList.empty()) {
        return;
    }
    size_t len = _ChildrenList.size();

    for (size_t i = 0; i < len; ++i) {
        
        std::cout << _ChildrenList[i]->GetId()<<" - "<< _ChildrenList[i]->GetGlobalPos().y << std::endl;
    }

    std::sort(_ChildrenList.begin(), _ChildrenList.end());



    for (size_t i = 0; i < len; ++i) {
        std::cout << _ChildrenList[i]->GetId() << " s " << _ChildrenList[i]->GetGlobalPos().y << std::endl;

    }
   

    for (size_t i = 0; i < len; ++i) {
        _ChildrenList[i]->OnRender();
        _ChildrenList[i]->RenderChildren();
    }
}



