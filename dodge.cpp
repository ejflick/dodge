#include <cstring>
#include <vector>
#include <iostream>
#include <raylib.h>

constexpr float OBSTACLE_SPEED = 400.0 / 60.0;

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 400;

constexpr float FLOOR_HEIGHT = 35;
constexpr float FLOOR_Y = SCREEN_HEIGHT - FLOOR_HEIGHT;

static int Lives = 3;
static int Score = 0;

typedef enum
{
  PLAYING,
  GAME_OVER
} GameState;

GameState GAME_STATE = PLAYING;

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

    remaining -= GetFrameTime();

    if (remaining <= 0)
    {
      done = true;
      running = false;
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

class Obstacle
{
public:
  Vector2 pos;
  Vector2 size;

  Obstacle(Vector2 _size) : size(_size), pos({0, 0})
  {
    pos.x = SCREEN_WIDTH;
    pos.y = FLOOR_Y - size.y;
  }

  void Update()
  {
    pos.x -= OBSTACLE_SPEED;
  }

  void Draw()
  {
    DrawRectangleV(pos, size, DARKGREEN);
  }

  bool OffScreen() const
  {
    return pos.x < 0 - size.x;
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
  Timer invincibilityTimer = Timer(2.5);
  bool invincible;

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

    if (invincible)
    {
      invincibilityTimer.update();

      if (invincibilityTimer.done)
        invincible = false;
    }
  }

  void Draw() const
  {
    DrawRectangleV(pos, size, invincible ? GREEN : RED);
  }

  bool CheckCollision(Obstacle &obstacle) const
  {
    // AABB collision
    return pos.x < obstacle.pos.x + obstacle.size.x && pos.x + size.x > obstacle.pos.x && pos.y < obstacle.pos.y + obstacle.size.y && pos.y + size.y > obstacle.pos.y;
  }

  void CollidedWithObstacle()
  {
    if (!invincible)
    {
      invincibilityTimer.reset();
      invincibilityTimer.start();
      invincible = true;
      Lives--;
    }
  }
};

class World
{
private:
  char scoreStringBuffer[64];
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

  void CheckPlayerCollisionWithObstacles()
  {
    for (int i = 0; i < obstacles.size(); i++)
    {
      if (player.CheckCollision(obstacles[i]))
      {
        player.CollidedWithObstacle();
      }
    }
  }

public:
  World() : player(), obstacles(), createObstacleTimer(4)
  {
  }

  void Init()
  {
    Score = 0;
    Lives = 3;
    player = Player();
    obstacles.clear();
    createObstacleTimer.reset();
    createObstacleTimer.start();
  }

  void Update()
  {
    player.Update();

    // Update obstacles
    for (auto &&ob : obstacles)
    {
      ob.Update();
    }

    // Remove off screen obstacles
    auto iter = obstacles.begin();
    while (iter != obstacles.end())
    {
      if (iter->OffScreen())
      {
        iter = obstacles.erase(iter);
        Score += 1;
      }
      else
      {
        iter++;
      }
    }

    createObstacleTimer.update();

    if (createObstacleTimer.done)
    {
      // Add new obstacle
      auto o = Obstacle({20, 40});
      obstacles.push_back(o);

      // Reset timer
      createObstacleTimer.setTime(GetRandomValue(1, 2) - 0.5);
      createObstacleTimer.reset();
      createObstacleTimer.start();
    }

    CheckPlayerCollisionWithObstacles();

    if (Lives == 0)
    {
      GAME_STATE = GAME_OVER;
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

    sprintf(scoreStringBuffer, "Score: %d", Score);
    DrawText(scoreStringBuffer, 10, 10, 40, BLACK);

    sprintf(scoreStringBuffer, "Lives: %d", Lives);
    DrawText(scoreStringBuffer, 10, 50, 30, BLACK);
  }
};

class Game
{
public:
  bool running = true;
  Camera2D camera;
  World world;

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

      switch (GAME_STATE)
      {
      case PLAYING:
        world.Draw();
        break;
      case GAME_OVER:
        int textSize = MeasureText("GAME OVER!", 50);
        DrawText("GAME OVER!", (SCREEN_WIDTH / 2) - (textSize / 2), (SCREEN_HEIGHT / 2) - 25, 50, MAROON);
        break;
      }

      EndDrawing();

      switch (GAME_STATE)
      {
      case PLAYING:
        world.Update();
        break;
      case GAME_OVER:
        if (IsKeyPressed(KEY_SPACE))
        {
          world.Init();
          GAME_STATE = PLAYING;
        }
        break;
      }
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
