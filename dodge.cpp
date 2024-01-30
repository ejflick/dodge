#include <string>
#include <iostream>
#include <raylib.h>
#include <rlgl.h>
#include <vector>

constexpr float OBSTACLE_SPEED = 400.0 / 60.0;

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 400;

constexpr float FLOOR_HEIGHT = 35;
constexpr float FLOOR_Y = SCREEN_HEIGHT - FLOOR_HEIGHT;

class Timer
{
private:
  float time;
  float remaining;
  bool running = true;

public:
  bool done = false;

  Timer(float timeInSeconds) : time(timeInSeconds), remaining(timeInSeconds) {}

  void update()
  {
    if (!running)
    {
      return;
    }

    time -= GetFrameTime();

    if (time <= 0)
    {
      done = true;
    }
  }

  void start()
  {
    running = true;
  }

  void pause()
  {
    running = false;
  }

  void reset()
  {
    remaining = time;
    done = false;
  }

  void setTime(float timeInSeconds)
  {
    time = timeInSeconds;
  }
};

class Player
{
private:
  Vector2 pos = {80, FLOOR_Y - 30};
  Vector2 size = {30, 30};
  Vector2 vel = {0, 0};
  float gravity = 50.0 / 60.0;
  float rotation = 0;
  bool rotating = false;
  bool canJump = false;

public:
  void Update()
  {
    if (rotating)
    {
      rotation += 6;
    }

    if (IsKeyPressed(KEY_SPACE) && canJump)
    {
      vel.y = -12.5;
      canJump = false;
    }

    vel.y += gravity;
    pos.y += vel.y;

    if (pos.y + size.y > FLOOR_Y)
    {
      pos.y = FLOOR_Y - size.y;
      vel.y = 0;
      canJump = true;
    }
  }

  void Draw()
  {
    DrawRectangleV(pos, size, RED);
  }
};

class Obstacle
{
private:
  Vector2 pos;
  Vector2 size;

public:
  Obstacle(Vector2 _size) : size(_size), pos({0, 0})
  {
    pos.x = SCREEN_WIDTH;
    pos.y = FLOOR_Y - size.y;
  }

  bool Update()
  {
    pos.x -= OBSTACLE_SPEED;
  }

  void Draw()
  {
    DrawRectangleV(pos, size, DARKGREEN);
  }
};

class World
{
private:
  Player player;
  std::vector<Obstacle> obstacles;
  Timer createObstacleTimer;

  void DrawFloor()
  {
    DrawRectangle(
        0, FLOOR_Y,
        SCREEN_WIDTH, FLOOR_HEIGHT,
        DARKGRAY);
  }

public:
  World() : player(), obstacles(), createObstacleTimer(4)
  {
  }

  void Init()
  {
    createObstacleTimer.start();
  }

  void Update()
  {
    player.Update();

    for (auto &&obstacle : obstacles)
    {
      obstacle.Update();
    }

    createObstacleTimer.update();

    if (createObstacleTimer.done)
    {
      // Add new obstacle
      auto o = Obstacle({20, 40});
      obstacles.push_back(o);
      std::cout << "Created obstacle" << std::endl;

      // Reset timer
      createObstacleTimer.setTime(GetRandomValue(0.5, 2.5));
      createObstacleTimer.reset();
      createObstacleTimer.start();
    }
  }

  void Draw()
  {
    player.Draw();
    DrawFloor();

    for (auto &&obstacle : obstacles)
    {
      obstacle.Draw();
    }
  }
};

class Game
{
public:
  bool running = true;
  Camera2D camera;
  World world;
  int score = 0;

  Game() : running(false), camera(), world() {}

  void Init()
  {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dodge!");
    SetExitKey(0);
    SetTargetFPS(60);

    world.Init();
  }

  void Run()
  {
    Init();

    while (!WindowShouldClose())
    {
      BeginDrawing();

      ClearBackground(LIGHTGRAY);

      world.Draw();

      DrawText("Score: 0", 10, 10, 40, BLACK);

      EndDrawing();

      world.Update();
    }

    running = false;
  }
};

int main(int argc, char const *argv[])
{
  auto game = Game();

  game.Run();

  return 0;
}
