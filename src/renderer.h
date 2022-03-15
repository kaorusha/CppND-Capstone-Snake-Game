#ifndef RENDERER_H
#define RENDERER_H

#include <list>
#include "SDL.h"
#include "SDL_image.h"
#include "snake.h"
#include "task.h"

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height,
           const int food_types, const int sport_types);
  ~Renderer();

  void Render(Snake const snake, std::list<Task> &tasks);
  void UpdateWindowTitle(int score, int fps);

 private:
  SDL_Window *sdl_window;
  SDL_Surface *sdl_surface;
  SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
  const int food_types;
  const int sport_types;
};

#endif