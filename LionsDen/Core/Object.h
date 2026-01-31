/*!
  boldtemple Gaming ©, http://gaming.boldtemple.net
  OpenSource Project - Check Development, License & Readme Sections.

  BGE - boldtemple Cross Platform Game Engine
  /Core/Object.h
!*/

#ifndef SRC_CORE_OBJECT_H_
#define SRC_CORE_OBJECT_H_

#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>

#include <SDL.h>

#include "Vec2.h"
#include "Render/Surface.h"
#include "Render/Camera.h"
#include "Box2d/box2d.h"


enum obj_type {
    OBJ_NONE = 0,
    OBJ_ENTITY,
    OBJ_GROUND,
    OBJ_WIDGET,
    OBJ_BOX,
    OBJ_BUTTON,
    OBJ_ENTITY_ATTACK_ENEMY,
	OBJ_ENTITY_ATTACK_PLAYER,
    OBJ_ENTITY_PLAYER,
    OBJ_ENTITY_MONSTER
};

class ScrollArea;
class Object;

typedef std::vector<Object*> ObjListType;

class Object {
public:
    static ObjListType DeleteCandidates;
    bool _custom_render;

    static Object* FindByLabel(const std::string& label);

    Object();
    Object(const std::string& label);
    virtual ~Object();

    void DeleteLater();

    virtual void SetGlobalPos(const Vec2& new_pos);

    /**
     * Set new Vec2 position
     */
    virtual void SetPos(const Vec2& new_pos);
    /**
     * Move object on Vec2 delta position (dx, dy)
     */

    virtual void SetPosCentered(const int& x, const int& y);

    virtual void Move(const Vec2& delta_pos);

    Object* GetOwner() const;
    virtual void Connect(Object* obj);
    virtual void Disconnect(Object* obj);
    virtual void DisconnectAll();

    virtual const Vec2& GetPos() const;
    virtual const Vec2& GetGlobalPos() const;

    void SetLabel(const std::string& label);
    const std::string& GetLabel();

    void SetVel(const Vec2& vel);
    Vec2 GetVel() const;

    obj_type GetType() const;
    int GetId() const;

    void SetSize(const Vec2& size);
    void SetSizeCentered(const float& x, const float& y);
    void RecalcCentered();

    void SetSizeRelative(int num);
    void SetPosRelative(int i,int len);
    const Vec2& GetSize() const;


    void SetCamera(Camera* cam);
    Camera* GetCamera();

    //Check top object for mouse events
    void CheckTop();

    void IgnoreClick(bool ignored);
    void IgnoreSorting(bool ignored);
    void IgnoreWheel(bool ignored);

    bool IsClickIgnored();
    bool IsWheelIgnored();

    bool IsDeletedLater();
    void UpdateId();

    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnCollide(Object* obj);
    virtual void OnMouse();
    virtual void OnTopMouseEvent();
    virtual void OnTopMouseWheelEvent();
    Vec2 GetOffset();
    void SetOffset(Vec2 offset);
    /**
     * Update all children
     */
    virtual void UpdateChildren();
    /**
     * Render all children
     */
    virtual void RenderChildren();
    virtual void RenderChildrenYSorted();


    bool operator< (const Object* other) const {
        std::cout<<"sorted"<<std::endl;
        return _pos.y < other->_pos.y;

    }
    static bool sort(const Object* a, const Object* b)
    {
        return a->_pos.y < b->_pos.y;
    }

    static int GetObjectCount()
    {
        return _object_count;
    }



	void SetType(obj_type type);
	Vec2 _pos_relative;
	Vec2 _size_relative;
	ObjListType _ChildrenList;
protected:
    
    ObjListType GetChildrenList();
    


private:
    friend ScrollArea;

    static int _last_id; //last created object id
    static std::unordered_map<std::string, Object*> _AllObjects;
    static int _object_count;

    bool _delete_later;
    bool _ignore_click;
    bool _ignore_wheel;
    bool _ignore_sorting;
    Vec2 _pos;
    Vec2 _size;
    Vec2 _offset;
    Vec2 _global_pos;
    Vec2 _vel;

    
    
    Camera* _camera = nullptr;

    SDL_Rect _draw_rect;

    int _id;
    obj_type _type;
    Object* _owner;

    std::string _label;

	

    void SetOwner(Object* obj);

    /**
     * Move children when parent is moved
     */
    void MoveChildern(const Vec2& delta_pos);

    ObjListType::iterator FindChild(Object* obj);

    void SetDefaults();
};

#endif /* SRC_CORE_OBJECT_H_ */
