#include "MainView.h"

#include "game/Types.h"
#include "game/Tile.h"
#include "game/Level.h"
#include "game/Rules.h"

using namespace ui;
using namespace baba;

extern GameData data;
Rules rules(&data);
extern Level* level;

SDL_Surface* palette = nullptr;

constexpr uint32_t FRAMES = 3;
struct ObjectGfx
{
  SDL_Texture* texture;
  std::vector<SDL_Rect> sprites;
  /* R U L D */
};

std::unordered_map<const baba::ObjectSpec*, ObjectGfx> objectGfxs;

const ObjectGfx& GameView::objectGfx(const baba::ObjectSpec* spec)
{
  auto it = objectGfxs.find(spec);

  if (it != objectGfxs.end())
    return it->second;
  else
  {
    path base = DATA_FOLDER + R"(Sprites\)" + spec->sprite;
    std::vector<uint32_t> frames;

    switch (spec->tiling)
    {
    case baba::ObjectSpec::Tiling::None:
      frames.push_back(0);
      break;
    case baba::ObjectSpec::Tiling::Tiled:
      for (int32_t i = 0; i < 16; ++i)
        frames.push_back(i);
      break;
    case baba::ObjectSpec::Tiling::Directions:
      frames.push_back(0);
      frames.push_back(8);
      frames.push_back(16);
      frames.push_back(24);
      break;
    case baba::ObjectSpec::Tiling::Character:
      int32_t f[] = { 31, 0, 1, 2, 3, 7, 8, 9, 10, 11, 15, 16, 17, 18, 19, 23, 24, 25, 26, 27 };
      for (int32_t i : f)
        frames.push_back(i);
      break;
    }

    SDL_Surface* surface = SDL_CreateRGBSurface(0, 24 * 3 * frames.size(), 24, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

    ObjectGfx gfx;

    for (uint32_t i = 0; i < frames.size(); ++i)
    {
      for (uint32_t f = 0; f < FRAMES; ++f)
      {
        path path = base + "_" + std::to_string(frames[i]) + "_" + std::to_string(f+1) + ".png";
        SDL_Surface* tmp = IMG_Load(path.c_str());
        assert(tmp);

        SDL_Rect dest = { (i * 3 + f) * 24, 0, 24, 24 };
        SDL_BlitSurface(tmp, nullptr, surface, &dest);

        SDL_FreeSurface(tmp);

        if (f == 0)
          gfx.sprites.push_back(dest);
      }
    }

    gfx.texture = SDL_CreateTextureFromSurface(manager->renderer(), surface);
    SDL_FreeSurface(surface);

    auto rit = objectGfxs.emplace(std::make_pair(spec, gfx));
    return rit.first->second;
  }
}


GameView::GameView(ViewManager* manager) : manager(manager)
{
}

void GameView::render()
{
  SDL_Renderer* renderer = manager->renderer();

  auto tick = (SDL_GetTicks() / 150) % 3;

  if (!palette)
  {
    palette = IMG_Load((DATA_FOLDER + R"(Palettes\default.png)").c_str());
    SDL_Surface* tmp = SDL_ConvertSurfaceFormat(palette, SDL_PIXELFORMAT_BGRA8888, 0);
    SDL_FreeSurface(palette);
    palette = tmp;

    //rules.state(data.objectsByName["baba"]).properties.set(baba::ObjectProperty::YOU);
    //rules.state(data.objectsByName["wall"]).properties.set(baba::ObjectProperty::STOP);

    rules.clear();
    rules.generate(level);
    rules.apply();
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (int y = 0; y < HEIGHT/24; ++y)
    for (int x = 0; x < WIDTH/24; ++x)
    {
      auto* tile = level->get(x, y);
      
      if (tile)
      {
        for (const auto& obj : *tile)
        {
          const auto& gfx = objectGfx(obj.spec);

          SDL_Color color;
          assert(palette->format->BytesPerPixel == 4);

          const auto& ocolor = obj.active ? obj.spec->active : obj.spec->color;

          SDL_GetRGB(*(((uint32_t*)palette->pixels) + ocolor.y * palette->w + ocolor.x), palette->format, &color.r, &color.g, &color.b);
          SDL_SetTextureColorMod(gfx.texture, color.r, color.g, color.b);

          SDL_Rect src = gfx.sprites[obj.variant];
          SDL_Rect dest = { x * 24, y * 24, 24, 24 };

          src.x += tick * 24;

          SDL_RenderCopy(renderer, gfx.texture, &src, &dest);
        }
      }
    }
}

void movement(Tile* tile, decltype(Tile::objects)::iterator object, coord_t dx, coord_t dy)
{  
  std::vector<std::pair<Tile*, decltype(Tile::objects)::iterator>> objects;

  Tile* next = level->get(tile->x() + dx, tile->y() + dy);

  objects.push_back(std::make_pair(tile, object));

  while (next)
  {
    if (!next)
      return;
    else if (next->empty())
      break;
    else
    {
      for (auto it = next->begin(); it != next->end(); ++it)
      {
        if (rules.hasProperty(it->spec, ObjectProperty::STOP))
          return;
        else if (rules.hasProperty(it->spec, ObjectProperty::PUSH))
          objects.push_back(std::make_pair(next, it));
        else if (it->spec->isText)
          objects.push_back(std::make_pair(next, it));
      }
    }

    next = level->get(next->x() + dx, next->y() + dy);
  }

  for (auto rit = objects.rbegin(); rit != objects.rend(); ++rit)
  {
    Object object = *rit->second;
    rit->first->objects.erase(rit->second);
    level->get(rit->first->x() + dx, rit->first->y() + dy)->objects.push_back(object);
  }
}


void movement(coord_t dx, coord_t dy)
{
  level->forEachObject([](Object& object) { object.alreadyMoved = false; });

  std::vector<std::pair<Tile*, decltype(Tile::objects)::iterator>> movable;

  for (coord_t y = 0; y < level->height(); ++y)
    for (coord_t x = 0; x < level->width(); ++x)
    {
      Tile* tile = level->get(x, y);

      for (auto it = tile->begin(); it != tile->end(); ++it)
        if (rules.hasProperty(it->spec, ObjectProperty::YOU))
          movable.push_back(std::make_pair(tile, it));
    }

  for (auto& pair : movable)
    movement(pair.first, pair.second, dx, dy);

  for (auto& tile : *level)
    std::sort(tile.begin(), tile.end(), [](const Object& o1, const Object& o2) { return o1.spec->layer < o2.spec->layer; });

  level->forEachObject([](Object& object) { object.active = false; });
  rules.clear();
  rules.generate(level);
  rules.apply();
}


void GameView::handleKeyboardEvent(const SDL_Event& event)
{
  if (event.type == SDL_KEYDOWN)
  {
    switch (event.key.keysym.sym)
    {
    case SDLK_ESCAPE: manager->exit(); break;
    case SDLK_LEFT: movement(-1, 0);  break;
    case SDLK_RIGHT: movement(1, 0);  break;
    case SDLK_UP: movement(0, -1); break;
    case SDLK_DOWN: movement(0, 1); break;
    }
  }
}

void GameView::handleMouseEvent(const SDL_Event& event)
{

}


GameView::~GameView()
{

}
