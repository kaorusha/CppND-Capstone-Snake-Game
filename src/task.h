#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#define print(x) std::cout<< "Variable: " << (#x) << " = " << x << "\n"
#include <mutex>
#include <thread>
#include <future>

enum TaskType {
  Food,
  Workout
};

class Task {
 public:
  Task(int x, int y, double duration);
  ~Task();
  Task(const Task& source); // copy constructor
  Task& operator=(const Task& source); // copy assignment
  Task(Task&& source); // move constructor
  Task& operator=(Task&& source); // move assignment

  TaskType type;
  void GetPosition(int &x, int &y);
  void SetPosition(int x, int y);
  SDL_Texture* GetTexture(){ return texture; }
  // Load image at specified path
  void LoadTexture(const std::string filename, SDL_Renderer* sdl_renderer);
  // Deallocates texture
  void Free();
  // set blending
  void SetBlendMode(SDL_BlendMode blending);
  // Set alpha modulation
  void SetAlpha();
  bool FadeOut();

 private:
  void DownCounter(double duration);
  double GetDuration();
  SDL_Texture* texture;
  Uint8 a;
  SDL_Point position;
  double _duration;
  mutable std::mutex _mutex;
  std::future<void> _future;
};

#endif