#ifndef CAMERA_H_
#define CAMERA_H_

#include <SDL.h>

#include "Core/Vec2.h"
#include "Common/constants.h"

class Camera {
public:
    Camera() {}
    Camera(const Vec2& pos, const Vec2& viewport);
    void SetPos(const Vec2& pos);
    void SetViewport(const Vec2& viewport, const Vec2 &offset);
    bool InView(SDL_Rect* rect) const;
    bool InView(int x, int y, int h, int w) const;
    bool InView(const Vec2& pos, const Vec2& size) const;
    void Move(const Vec2& delta_pos);
    Vec2 GetPos() const;
    Vec2 GetViewport() const;
    int X() const;
    int Y() const;
    int W() const;
    int H() const;

private:
    Vec2 _pos;
    Vec2 _viewport; // aka size
    Vec2 _offset;
};

#endif /* CAMERA_H_ */
