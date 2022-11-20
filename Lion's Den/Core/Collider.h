//
// Created by snickers on 14.05.15.
// 
// Edit by Dawid Dzi?gielewski 
// 
//

#ifndef ENGINE_COLLIDER_H
#define ENGINE_COLLIDER_H

#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cmath>

#include "Vec2.h"
#include "Object.h"

class Collider {
public:
    enum TYPE_COLLIDER {
        COLLIDER_RECT = 0,
        COLLIDER_ATTACK,
        COLLIDER_CIRCLE
    };


    static void ProcessMovementCollisions();
    static void ProcessAttackCollisions();
    static bool CheckForCollisions();
    static Collider* RegisterObject(Object* obj, const Vec2& pos = Vec2::ZERO, const Vec2& size = Vec2::ZERO, bool is_static = true);
	static Collider* RegisterObject(Object* obj, TYPE_COLLIDER  _type= COLLIDER_RECT, const Vec2& pos = Vec2::ZERO, const Vec2& size = Vec2::ZERO, bool is_static = true);
    static Collider* RegisterTarget(Object* obj, const Vec2& pos, const Vec2& size);
    static Collider* RegisterAttack(Object* obj);

    static void UnregisterObject(Object* obj);
    static void UnregisterTarget(Object* obj);
    static void UnregisterAttack(Object* obj);
    static Collider* GetCollider(Object* obj);

    void SetStatic(bool is_static);

    void SetCircleRadius(float radius);
    float GetCircleRadius();

    TYPE_COLLIDER GetType();
    void SetType(TYPE_COLLIDER type);

    bool CheckCollision(Collider* col, Vec2& dif);
    //Object* RayCast(const Vec2& begin, const Vec2& end);

    bool CheckAttack(Collider* col);

    static void DrawCollisions();

    static int GetSize();
    


    bool _is_static = true;
    Vec2 _offset;
    Vec2 _size;
    float _radius = 0;

private:
    static std::unordered_map<Object*, Collider*> _Colliders;
	static std::unordered_map<Object*, Collider*> _Targets;
	static std::unordered_map<Object*, Collider*> _Attacks;

    static std::unordered_map<Object*, Collider*>::iterator Find(Object* obj);
    static std::unordered_map<Object*, Collider*>::iterator FindT(Object* obj);
	static std::unordered_map<Object*, Collider*>::iterator FindA(Object* obj);
    Object* _owner = nullptr;
    TYPE_COLLIDER  _type = COLLIDER_RECT;

    Collider(Object *obj);
    ~Collider();

    void SetOwner(Object* owner);
    Object* GetOwner();
};

#endif //ENGINE_COLLIDER_H
