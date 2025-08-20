#include "raylib.h"
#include "raymath.h"

int main() {
  // Initialize Window
  const int screenWidth = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "raylib basic window");

  // Initialize Camera
  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 0.0f, 0.0f};
  camera.target = (Vector3){-5.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // Load Model
  Model eye = LoadModel("assets/20250820_website-eye_AARIALMA.glb");

  Vector3 eyePosition = {-5.0f, 0.0f, 0.0f};
  float eyeRadius = 1.0f;

  Vector2 mousePosition = GetMousePosition();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    mousePosition = GetMousePosition();

    eye.transform =
        MatrixRotateXYZ((Vector3){mousePosition.x, mousePosition.y, 0.0f});

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawModel(eye, eyePosition, eyeRadius, WHITE);
    DrawGrid(10, 1.0f);
    EndMode3D();

    DrawRectangle(10, 10, 250, 113, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(10, 10, 250, 113, BLUE);

    DrawText("Statistics", 20, 20, 10, BLACK);
    DrawText(TextFormat("Mouse X Position: %.2f", mousePosition.x), 40, 40, 10,
             DARKGRAY);
    DrawText(TextFormat("Mouse Y Position: %.2f", mousePosition.y), 40, 60, 10,
             DARKGRAY);

    EndDrawing();
  }
  UnloadModel(eye);
  CloseWindow();
  return 0;
}
