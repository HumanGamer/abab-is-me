#include "view_manager.h"

#include "main_view.h"

using namespace ui;

ui::ViewManager::ViewManager() : SDL<ui::ViewManager, ui::ViewManager>(*this, *this), _font(nullptr),
_gameView(new GameView(this))
{
  _view = _gameView;
}

void ui::ViewManager::deinit()
{
  SDL_DestroyTexture(_font);

  SDL::deinit();
}

bool ui::ViewManager::loadData()
{
  /*SDL_Surface* font = IMG_Load("font.png");
  assert(font);

  _font = SDL_CreateTextureFromSurface(_renderer, font);

  SDL_SetTextureBlendMode(_font, SDL_BLENDMODE_BLEND);
  SDL_FreeSurface(font);*/

  return true;
}

void ui::ViewManager::handleKeyboardEvent(const SDL_Event& event, bool press)
{
  _view->handleKeyboardEvent(event);
}

void ui::ViewManager::handleMouseEvent(const SDL_Event& event)
{
  _view->handleMouseEvent(event);
}


void ui::ViewManager::render()
{
  _view->render();
}

void ui::ViewManager::text(const std::string& text, int32_t x, int32_t y)
{
  constexpr float scale = 2.0;
  constexpr int32_t GLYPHS_PER_ROW = 16;

  for (size_t i = 0; i < text.length(); ++i)
  {
    SDL_Rect src = { 8 * (text[i] % GLYPHS_PER_ROW), 8 * (text[i] / GLYPHS_PER_ROW), 4, 6 };
    SDL_Rect dest = { x + 4 * i * scale, y, 4 * scale, 6 * scale };
    SDL_RenderCopy(_renderer, _font, &src, &dest);
  }
}

void ViewManager::text(const std::string& text, int32_t x, int32_t y, SDL_Color color, TextAlign align, float scale)
{
  constexpr int32_t GLYPHS_PER_ROW = 16;

  const int32_t width = text.size() * 4 * scale;

  if (align == TextAlign::CENTER)
    x -= width / 2;
  else if (align == TextAlign::RIGHT)
    x -= width;

  SDL_SetTextureColorMod(_font, color.r, color.g, color.b);

  for (size_t i = 0; i < text.length(); ++i)
  {
    SDL_Rect src = { 8 * (text[i] % GLYPHS_PER_ROW), 8 * (text[i] / GLYPHS_PER_ROW), 4, 6 };
    SDL_Rect dest = { x + 4 * i * scale, y, 4 * scale, 6 * scale };
    SDL_RenderCopy(_renderer, _font, &src, &dest);
  }

  SDL_SetTextureColorMod(_font, 255, 255, 255);
}