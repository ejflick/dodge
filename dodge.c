#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

const float WorldVelocity = 50.0f / 60.0f;

typedef struct
{
  /// @brief Player's position.
  Vector3 pos;

  /// @brief How much the player is rotated.
  float rotation;
} Player;

Player *CreatePlayer()
{
  Player *p = malloc(sizeof(Player));

  Vector3 pos = {0, 0, 0};
  p->pos = pos;

  p->rotation = 0.0f;

  return p;
}

void DrawPlayer(Player *player)
{
}

void UpdatePlayer(Player *player)
{
  // Rotate the player/stop their rotation.
}

void DestroyPlayer(Player *player)
{
  free(player);
}

typedef struct
{
  float width;
  float height;
  Vector2 pos;
} Obstacle;

Obstacle *CreateObstacle(int width, int height, float x, float y)
{
  Obstacle *o = malloc(sizeof(Obstacle));

  Vector2 pos = {x, y};
  o->pos = pos;
  o->width = width;
  o->height = height;

  return o;
}

typedef struct
{
  ObstacleNode *head;
  ObstacleNode *tail;
} ObstacleList;

typedef struct
{
  Obstacle *val;
  ObstacleNode *next;
  ObstacleNode *prev;
} ObstacleNode;

ObstacleList *CreateObstacleList()
{
  ObstacleList *list = malloc(sizeof(Obstacle));

  list->head = NULL;
  list->tail = NULL;

  return list;
}

void AddObstacle(ObstacleList *list, Obstacle *ob)
{
  ObstacleNode *newNode = malloc(sizeof(Obstacle));

  newNode->next = NULL;
  newNode->prev = list->tail;
  newNode->val = ob;

  // If there's a previous node, update that one
  if (newNode->prev)
  {
    ObstacleNode prev = newNode->prev;
    prev.next = newNode;
  }

  list->tail = newNode;

  if (list->head == NULL)
  {
    list->head = list->tail;
  }
}

void RemoveObstacle(ObstacleList *list, ObstacleNode *node)
{
  if (list->head == node)
  {
    list->head = NULL;
  }

  if (list->tail == node)
  {
    list->tail = NULL;
  }

  if (node->next)
  {
    ObstacleNode *next;
    next->prev = node->prev;
  }

  if (node->prev)
  {
    ObstacleNode *prev;
    prev->next = node->next;
  }

  free(node);
}

void UpdateObstacle(Obstacle *obstacle)
{
  obstacle->pos.x -= WorldVelocity;
}

void UpdateObstacles(ObstacleList *list)
{
  for (ObstacleNode **head = list->head; head != NULL; head = (*head)->next)
  {
    Obstacle *ob = (*head)->val;

    if (ob->pos.x + ob->width < 0)
    {
      RemoveObstacle(list, *head);
    }
    else
    {
      UpdateObstacle((*head)->val);
    }
  }
}

typedef struct
{
  Player *player;
  ObstacleList *obstacles;
} World;

World *world;

void InitWorld()
{
  world = malloc(sizeof(World));

  world->player = CreatePlayer();
  world->obstacles = CreateObstacleList();
}

void DrawWorld()
{
  DrawPlayer(world->player);
}

void UpdateWorld()
{
  UpdatePlayer(world->player);
  UpdateObstacles(world->obstacles);
}

void DestroyWorld()
{
  free(world->player);
  free(world);
}

int main(int argc, char const *argv[])
{
  InitWindow(800, 600, "Dodge");

  SetExitKey(0);
  SetTargetFPS(60);

  InitWorld();

  while (!WindowShouldClose())
  {
    BeginDrawing();
    {
      ClearBackground(LIGHTGRAY);
      DrawWorld();
    }
    EndDrawing();

    UpdateWorld();
  }

  DestroyWorld();
  CloseWindow();

  return 0;
}
