#include "task.h"
/**
 * @brief Construct a new Task:: Task object
 * 
 * @param x the horizontal position in the window, user is responsible to check the range
 * @param y the horizontal position in the window, user is responsible to check the range
 * @param duration the time in millisecond that the texture changed its transparency
 */
Task::Task(int x, int y, double duration):position{x, y}, texture(NULL), a(255) {
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Task constructor\n");
  _duration = (duration > 0) ? duration: 1000;
}
/**
 * @brief Destroy the Task:: Task object
 * Will stop the down counter thread and deallocate the texture
 */
Task::~Task() {
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Task destructor\n");
  ZeroAlpha();
  if (_future.valid())
    _future.wait();
  Free();
}

// copy constructor
Task::Task(const Task& source) {
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Task copy constructor\n");
  std::lock_guard<std::mutex> lock(source._mutex);
  // Not need deep copy
  // texture remain, source texture will free at destructor
  // data handles
  type = source.type;
  a = source.a;
  position = source.position;
  _duration = source._duration;
}

// copy assignment
Task& Task::operator=(const Task& source) {
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Task copy assignment\n");
  if (this != &source) {
    std::lock(_mutex, source._mutex);
    std::lock_guard<std::mutex> lockThis(_mutex, std::adopt_lock);
    std::lock_guard<std::mutex> lockSource(source._mutex, std::adopt_lock);
    // Not need deep copy
    // texture remain, source texture will free at destructor

    type = source.type;
    a = source.a;
    position = source.position;
    _duration = source._duration;
  }
  return *this;
}

// move constructor
Task::Task(Task&& source) {
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Task move constructor\n");
  std::lock_guard<std::mutex> lock(source._mutex);
  Free();
  texture = std::move(source.texture);
  type = std::move(source.type);
  a = std::move(source.a);
  position = std::move(source.position);
  _duration = source._duration;
  source.texture = NULL;
}

// move assignment
Task& Task::operator=(Task&& source) {
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Task move assignment\n");
  if (this!= &source) {
    std::lock(_mutex, source._mutex);
    std::lock_guard<std::mutex> lockThis(_mutex, std::adopt_lock);
    std::lock_guard<std::mutex> lockSource(source._mutex, std::adopt_lock);
    Free();
    texture = std::move(source.texture);
    type = std::move(source.type);
    a = std::move(source.a);
    position = std::move(source.position);
    _duration = source._duration;
    source.texture = NULL;
  }
  return *this;
}

void Task::GetPosition(int &x, int &y) {
  std::lock_guard<std::mutex> lock(_mutex);
  x = position.x;
  y = position.y;
}

// reset new position and reset fade out
void Task::SetPosition(int x, int y){
  std::lock_guard<std::mutex> lock(_mutex);
  position.x = x;
  position.y = y;
  a = 255;
}

SDL_Texture* Task::GetTexture(){
  std::lock_guard<std::mutex> lock(_mutex);
  return texture;
}

double Task::GetDuration() {
  std::lock_guard<std::mutex> lock(_mutex);
  return _duration;
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
    } else {
      // set standard alpha blending
      SetBlendMode(SDL_BLENDMODE_BLEND);
    }
    // Get rid of old loaded surface
    SDL_FreeSurface(loaded_surface);
    // Start down counter as std::async object to control the fade out effect of the texture
    _future = std::async( std::launch::async | std::launch::deferred, &Task::DownCounter, this, GetDuration());
  }
}

void Task::Free() {
  //Free texture if it exits
  if (GetTexture() != NULL) {
    SDL_DestroyTexture(texture);
    texture = NULL;
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Free texture\n");
  }
}

void Task::DownCounter(double duration) {
  std::chrono::time_point<std::chrono::system_clock> lastUpdate;
  lastUpdate = std::chrono::system_clock::now();
  while(1) {
    long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
    if (timeSinceLastUpdate >= duration) {
      std::lock_guard<std::mutex> lck(_mutex);
      //reduce a
      if (a - 32 < 0) {
        a = 0;
        return;
      } else {
        a -= 32; 
      }
      lastUpdate = std::chrono::system_clock::now();     
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void Task::SetBlendMode(SDL_BlendMode blending) {
  // Set blending function
  SDL_SetTextureBlendMode(texture, blending);
}

void Task::SetAlpha() {
  std::lock_guard<std::mutex> lck(_mutex);
  // Modulate texture alpha
  SDL_SetTextureAlphaMod( texture, a);
}

// return true if this texture has disappeared
bool Task::FadeOut() {
  std::lock_guard<std::mutex> lock(_mutex);
  return (a == 0) ? true : false;
}

// set alpha = 0 to stop async task
void Task::ZeroAlpha() {
  std::lock_guard<std::mutex> lock(_mutex);
  a = 0;
}