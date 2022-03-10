#include "game.h"
#include <iostream>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, tasks);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  PlaceFood(x, y);
  tasks.emplace_back(x, y);
  if (GetScore()%2 == 0) {
    tasks.back().type = TaskType::Food;
  } else {
    tasks.back().type = TaskType::Workout;
  }
  return;
}

void Game::PlaceFood(int&x, int&y) {
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y) && !OccupiedCell(x, y)) {     
      return;
    }
  }
}

bool Game::OccupiedCell(int x, int y) {
  SDL_Point position;
  for (auto task : tasks){
    task.GetPosition(position.x, position.y);
    if (x == position.x && y == position.y)
      return true;
  }
  return false;
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  SDL_Point food;
  for (auto it = tasks.begin(); it < tasks.end(); ++it) {
    it->GetPosition(food.x, food.y);
    if (food.x == new_x && food.y == new_y) {
      switch (it->type) {
        case TaskType::Food:
          score++;
          PlaceFood(food.x, food.y);
          it->SetPosition(food.x, food.y);
          for(int i=0; i< snake.size; ++i) {
            PlaceFood();
          }
          // Grow snake and increase speed.
          snake.GrowBody();
          snake.speed += 0.02;
          break;
        
        default:
          PlaceFood(food.x, food.y);
          it->SetPosition(food.x, food.y);
          snake.ShortenBody();
          break;
      }
      return;
    }
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }