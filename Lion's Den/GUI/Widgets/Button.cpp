#include "Button.h"

Button::Button(const Vec2& pos, const Vec2& size, const std::string& font, int font_pt_size, std::string text="") :
Box(pos, size, font, font_pt_size) {
    ShowBack(true);

    Object::SetType(OBJ_BUTTON);
    auto set_offset = [this](int shift){
        SDL_Rect offset = GetOffset();

        offset.x += shift;
        offset.y += shift;
        offset.h -= shift;
        offset.w -= shift;

        SetOffset(offset);
        CalcTextPos();
    };
	SetText(text);
	SetStyle("GUI/button_style.png", 32);

    AddAction("mousedown", [this, set_offset](){
        SetBackGround(_back_style, _pos_pressed, _tile_resolution);
        set_offset(1);

     

    }, this);

    AddAction("mouseup", [this, set_offset](){
        SetBackGround(_back_style, _pos_normal, _tile_resolution);
        set_offset(-1);
    }, this);

    AddAction("mousehover", [this](){
        SetBackGround(_back_style, _pos_hovered, _tile_resolution);
    }, this);

    AddAction("mouseleave", [this, set_offset](){
        SetBackGround(_back_style, _pos_normal, _tile_resolution);

        if(_state == WIDGET_PRESSED){
           set_offset(-1);
        }
    }, this);
    if (GetLabel() == "")
    {
        SetLabel(text);
    }
    

}

Button::~Button() {
}


void Button::SetStyle(const std::string& style,
                      const Vec2& pos_normal,
                      const Vec2& pos_hovered,
					  const Vec2& pos_pressed,
                      int tile_resolution) {
    _back_style = style;
    _pos_normal = pos_normal;
    _pos_hovered = pos_hovered;
	_pos_pressed = pos_pressed;
    _tile_resolution = tile_resolution;

    SetBackGround(_back_style, _pos_normal, _tile_resolution);
}
void Button::SetStyle(const std::string& style,
					  int tile_resolution) {
	_back_style = style;
	_pos_normal = Vec2::ZERO;
	_pos_hovered = _pos_normal+(Vec2::RIGHT * tile_resolution * 3);
	_pos_pressed = _pos_hovered+(Vec2::RIGHT * tile_resolution * 3);
	_tile_resolution = tile_resolution;

	SetBackGround(_back_style, _pos_normal, _tile_resolution);
}

void Button::ReloadStyle()
{
    SetBackGround(_back_style, _pos_normal, _tile_resolution);
}


/*
void Button::SetText(const std::string& str){
    Box::SetText(str);

    _text_offset.x = (GetSize().x - _text_size.x)/2;
    _text_offset.y = (GetSize().y - _text_size.y)/2;

    Box::SetText(str);
    CalcTextPos();
}
*/