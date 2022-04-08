

#include "GUI/Widgets/StatsBar.h"
#include <Render/Drawer.h>




StatsBar::StatsBar(const Vec2& pos, const Vec2& size) :
    Widget(pos, size)
{
    SetSize(size);
    SetPos(pos);
    life = 100;
    mana = 100;
    experience = 100;

    render = false;

}

StatsBar::~StatsBar()
{

}
void StatsBar::SetStyle(const std::string& stylefile)
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


    SDL_Rect rect_barfill = { 0 , 0, 8, 12 };
    SDL_Texture* _life_texture = SDL_CreateTexture(Window::GetRenderer(), SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, rect_barfill.w, rect_barfill.h);
    SDL_SetTextureBlendMode(_life_texture, SDL_BLENDMODE_BLEND);

    SDL_Texture* _mana_texture = SDL_CreateTexture(Window::GetRenderer(), SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, rect_barfill.w, rect_barfill.h);
    SDL_SetTextureBlendMode(_life_texture, SDL_BLENDMODE_BLEND);

    SDL_Texture* _experience_texture = SDL_CreateTexture(Window::GetRenderer(), SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, rect_barfill.w, rect_barfill.h);
    SDL_SetTextureBlendMode(_life_texture, SDL_BLENDMODE_BLEND);


    Surface::BeginViewport(Vec2::ZERO, Vec2(rect.x, rect.y));

    //start rendering _back texture
    SDL_SetRenderTarget(Window::GetRenderer(), _back_texture);
    SDL_Color background = Window::GetBackgroundColor();
    SDL_SetRenderDrawColor(Window::GetRenderer(), background.r, background.g, background.b, 0);
    SDL_RenderClear(Window::GetRenderer());

    //left side
    SDL_Rect src_rect, dst_rect;
    src_rect = { 0, 0, 84, 64 };
    dst_rect = { 0, 0, 84, 64 };
    Surface::Draw(texture, &src_rect, &dst_rect);

    //first bar
    src_rect = { 84, 0, 10, 64 };
    dst_rect = { 84, 0, 10, 64 };
    Surface::Draw(texture, &src_rect, &dst_rect);

    //9 bars
    src_rect = { 93, 0, 10, 64 };
    dst_rect = { 93, 0, 10, 64 };
    for (int step = 0; step < 9; step++)
    {
        SDL_Rect dst_tmp = { dst_rect.x + step * 9, dst_rect.y, 10, dst_rect.h };
        Surface::Draw(texture, &src_rect, &dst_tmp);
    }
    //right side
    src_rect = { 104, 0, 31, 64 };
    dst_rect = { 175, 0, 31, 64 };
    Surface::Draw(texture, &src_rect, &dst_rect);




    SDL_SetRenderTarget(Window::GetRenderer(), _life_texture);
    src_rect = { 153, 6, 8, 12 };
    dst_rect = { 0, 0, 8, 12 };
    Surface::Draw(texture, &src_rect, &dst_rect);
    _life.SetTexture(_life_texture);

    SDL_SetRenderTarget(Window::GetRenderer(), _mana_texture);
    src_rect = { 153, 26, 8, 12 };
    dst_rect = { 0, 0, 8, 12 };
    Surface::Draw(texture, &src_rect, &dst_rect);
    _mana.SetTexture(_mana_texture);

    SDL_SetRenderTarget(Window::GetRenderer(), _experience_texture);
    src_rect = { 153, 46, 8, 12 };
    dst_rect = { 0, 0, 8, 12 };
    Surface::Draw(texture, &src_rect, &dst_rect);
    _experience.SetTexture(_experience_texture);

    Surface::EndViewport();
    SDL_SetRenderTarget(Window::GetRenderer(), nullptr);
    _back.SetTexture(_back_texture);
}

void StatsBar::UpdateValues(Player* player)
{
    SetStyle("GUI/hud.png");


}

void StatsBar::ConnectPlayer(Player* player)
{
    pla = player;
    //SetCamera(pla->GetCamera());
}

void StatsBar::RenderBars()
{
    Vec2 BarSize = Vec2(8, 12);
    Vec2 LifeBarPos = Vec2(85, 6);
    Vec2 ManaBarPos = Vec2(85, 27);
    Vec2 ExperienceBarPos = Vec2(85, 46);


    int DrawMaxCounterLife = pla->GetLife() / 10;
    int DrawMaxCounterMana = pla->GetMana() / 10;
    int DrawMaxCounterExperience = pla->GetExperience() / 10;

    for (int i = 0; i < DrawMaxCounterLife; i++)
    {
        _life.Draw(LifeBarPos + Vec2(9, 0) * i, BarSize, Object::GetCamera());
    }
    for (int i = 0; i < DrawMaxCounterMana; i++)
    {
        _mana.Draw(ManaBarPos + Vec2(9, 0) * i, BarSize, Object::GetCamera());
    }
    for (int i = 0; i < DrawMaxCounterExperience; i++)
    {
        _experience.Draw(ExperienceBarPos + Vec2(9, 0) * i, BarSize, Object::GetCamera());
    }
}

void StatsBar::RecalcRelative()
{




}

void StatsBar::SetRenderOption(bool r)
{
    render = r;
}

bool StatsBar::GetRenderOption()
{
    return render;
}

void StatsBar::OnRender() {
    
    if (_visible)
    {
        // #PlayerStatsDebug
        /*
        ImGui::Begin("Player stats debug");

        ImTextureID my_tex_id1 = _back.GetTexture();
        ImTextureID my_tex_id2 = _life.GetTexture();
        ImTextureID my_tex_id3 = _mana.GetTexture();
        ImTextureID my_tex_id4 = _experience.GetTexture();

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
        ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

        ImGui::Image(my_tex_id1, ImVec2(500, 150), uv_min, uv_max, tint_col, border_col);
        ImGui::Image(my_tex_id2, ImVec2(8 * 2, 12 * 2), uv_min, uv_max, tint_col, border_col);
        ImGui::Image(my_tex_id3, ImVec2(8 * 2, 12 * 2), uv_min, uv_max, tint_col, border_col);
        ImGui::Image(my_tex_id4, ImVec2(8 * 2, 12 * 2), uv_min, uv_max, tint_col, border_col);
        // ImGui::Text("_back %d ", _back.GetTexture);
        ImGui::Text("Player life:%d", pla->GetLife());
        ImGui::Text("Player mana:%d", pla->GetMana());
        ImGui::Text("Player experience:%d", pla->GetExperience());

        ImGui::SliderInt("slider life", &life, 0, 100);
        //pla->SetLife(life);
        ImGui::SliderInt("slider mana", &mana, 0, 100);
        pla->SetMana(mana);
        ImGui::SliderInt("slider experience", &experience, 0, 100);
        pla->SetExperience(experience);

        ImGui::End();
		*/
        _back.Draw(Object::GetGlobalPos(), Object::GetSize(), Object::GetCamera());
        RenderBars();
    }
    //Drawer::Rect(GetGlobalPos(), GetSize(), COLOR_RED, false);


}