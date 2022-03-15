#include "renderer.h"
#include <iostream>
#include <string>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width,
                   const std::size_t grid_height,
                   const int food_types,
                   const int sport_types)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height),
      food_types(food_types),
      sport_types(sport_types) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  } else {
    // Initialize PNG or JPG loading
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
      std::cerr << "SDL_image could not initialize!\n";
      std::cerr << "SDL_image Error: " << IMG_GetError() << "\n";
    }
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(sdl_window);
  IMG_Quit();
  SDL_Quit();
}

void Renderer::Render(Snake const snake, std::list<Task> &tasks) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  //SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  SDL_Point food;
  for (auto it = tasks.begin(); it!=tasks.end(); ++it) {
    it->GetPosition(food.x, food.y);
    block.x = food.x * block.w;
    block.y = food.y * block.h;
    //SDL_RenderFillRect(sdl_renderer, &block);
    if (it->GetTexture() == NULL) {
      std::string dir = "../picture/";
      char file;
      switch (it->type) {
        case TaskType::Food:
          file = food.x%food_types + 1;
          dir += std::to_string(file);
          break;
        
        default:
          file = food.x%sport_types + 'a';
          dir += file;
          break;
      }
      dir += ".png";
      it->LoadTexture(dir, sdl_renderer);
    }
    it->SetAlpha();
    if (SDL_RenderCopy(sdl_renderer, it->GetTexture(), NULL, &block) != 0) {
      std::cerr << "Unable to copy texture to renderer" << "\n";
      std::cerr << "SDL_image Error: " << SDL_GetError() << "\n";
    }
  }

  // Render snake's body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
