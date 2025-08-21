#include "raylib.h"
#include "raymath.h"
#include <math.h>

int main() {
  // Initialize Window
  const int screenWidth = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "raylib basic window");

  // Initialize Camera
  Camera3D camera = {0};
  camera.position = (Vector3){5.0f, 0.0f, 0.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // Load Model
  Model eye = LoadModel("assets/20250820_website-eye_AARIALMA.glb");

  Vector3 eyePosition = {0.0f, 0.0f, 0.0f};
  float eyeRadius = 1.0f;
  float eyePitch = 0.0f;
  float eyeYaw = 0.0f;
  Ray mouseRay = {0};

  // Render Loop
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    // Rotation Math
    mouseRay = GetScreenToWorldRay(GetMousePosition(), camera);
    eyeYaw = atan2f(mouseRay.direction.z, -mouseRay.direction.x);
    eyePitch = atan2f(mouseRay.direction.y, -mouseRay.direction.x);
    eye.transform = MatrixRotateXYZ((Vector3){0.0f, -eyeYaw, eyePitch});

    // Initialize Drawing
    BeginDrawing();
    ClearBackground(DARKBLUE);
    // 3D
    BeginMode3D(camera);

    // Render Eye
    DrawModel(eye, eyePosition, eyeRadius, WHITE);
    DrawGrid(10, 1.0f);
    EndMode3D();

    // 2D
    // Diagnostics
    DrawRectangle(10, 10, 250, 250, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(10, 10, 250, 250, BLUE);

    DrawText("Diagnostics", 20, 20, 10, BLACK);
    DrawText(TextFormat("Mouse X Position: %.2f", GetMousePosition().x), 40, 40,
             10, DARKGRAY);
    DrawText(TextFormat("Mouse Y Position: %.2f", GetMousePosition().y), 40, 60,
             10, DARKGRAY);
    DrawText(TextFormat("Mouse Ray X Position: %.2f", mouseRay.position.x), 40,
             80, 10, DARKGRAY);
    DrawText(TextFormat("Mouse Ray Y Position: %.2f", mouseRay.position.y), 40,
             100, 10, DARKGRAY);
    DrawText(TextFormat("Mouse Ray Z Position: %.2f", mouseRay.position.z), 40,
             120, 10, DARKGRAY);
    DrawText(TextFormat("Mouse Ray X Direction: %.2f", mouseRay.direction.x),
             40, 140, 10, DARKGRAY);
    DrawText(TextFormat("Mouse Ray Y Direction: %.2f", mouseRay.direction.y),
             40, 160, 10, DARKGRAY);
    DrawText(TextFormat("Mouse Ray Z Direction: %.2f", mouseRay.direction.z),
             40, 180, 10, DARKGRAY);
    DrawText(TextFormat("Mouse Ray Pitch: %.2f", eyePitch), 40, 200, 10,
             DARKGRAY);
    DrawText(TextFormat("Mouse Ray Yaw: %.2f", eyeYaw), 40, 220, 10, DARKGRAY);

    // Termination
    EndDrawing();
  }
  UnloadModel(eye);
  CloseWindow();
  return 0;
}
