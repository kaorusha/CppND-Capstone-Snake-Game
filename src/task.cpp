#include "task.h"

Task::Task(int x, int y):position{x, y}, texture(NULL) {
}

Task::~Task() {
}

void Task::GetPosition(int &x, int &y) {
  x = position.x;
  y = position.y;
}

void Task::SetPosition(int x, int y){
  position.x = x;
  position.y = y;
}

void Task::LoadTexture(const std::string filename, SDL_Renderer* sdl_renderer){
  // get rid of preexisting texture
  Free();
  SDL_Surface *loaded_surface = IMG_Load(filename.c_str());
  if (loaded_surface == NULL) {
    std::cerr << "Unable to load image " << filename << "\n";
    std::cerr << "SDL_image Error: " << SDL_GetError() << "\n";
  } else {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Loading %s", filename.c_str());
    // create texture from surface pixels
    texture = SDL_CreateTextureFromSurface (sdl_renderer, loaded_surface);
    if (texture == NULL) {
      std::cerr << "Unable to create texture from " << filename << "\n";
      std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    }
    // Get rid of old loaded surface
    SDL_FreeSurface(loaded_surface);
  }
}

void Task::Free() {
  //Free texture if it exits
  if (GetTexture() != NULL) {
    SDL_DestroyTexture(texture);
    texture = NULL;
    print("free texture");
  }
}