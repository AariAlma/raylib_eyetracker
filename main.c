#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include <math.h>

#if defined(PLATFORM_WEB)
#define GLSL_VERSION 100
#include <emscripten/emscripten.h>
#else
#define GLSL_VERSION 330
#endif

void UpdateDrawFrame(void);

// Initialize Window
const int screenWidth = 800;
const int screenHeight = 450;

// Static Variables
static Camera3D camera = {0};
static Model eye = {0};
static Shader shader = {0};
static Light lights[MAX_LIGHTS] = {0};
static Vector3 eyePosition = {0.0f, 0.0f, 0.0f};
static float eyeRadius = 1.0f;
typedef struct EyeTracker {
  Vector3 position;
  Vector3 velocity;
  Vector3 acceleration;
  Vector3 direction;
} EyeTracker;
static EyeTracker eyeTracker;

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Eye Tracker Test");
  SetWindowMinSize(320, 240);

  // Initialize Camera
  camera.position = (Vector3){5.0f, 0.0f, 0.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // Load Model
  eye = LoadModel("assets/20250820_website-eye_AARIALMA.glb");

  // Load Shader
  shader = LoadShader(TextFormat("assets/shaders/lighting.vert", GLSL_VERSION),
                      TextFormat("assets/shaders/lighting.frag", GLSL_VERSION));
  shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
  int ambientLoc = GetShaderLocation(shader, "ambient");
  const float ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
  SetShaderValue(shader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);
  for (int i = 0; i < eye.materialCount; i++)
    eye.materials[i].shader = shader;

  // Initialize Eye Tracker
  eyeTracker.position = Vector3Zero();
  eyeTracker.velocity = Vector3Zero();
  eyeTracker.acceleration = Vector3Zero();
  eyeTracker.direction = Vector3Zero();

  // Create Light
  lights[0] = CreateLight(LIGHT_POINT, (Vector3){0.0f, 2.0f, 0.0f},
                          Vector3Zero(), GREEN, shader);
  lights[1] = CreateLight(LIGHT_POINT, (Vector3){0.0f, -1.0f, -1.732f},
                          Vector3Zero(), RED, shader);
  lights[2] = CreateLight(LIGHT_POINT, (Vector3){0.0f, -1.0f, 1.732f},
                          Vector3Zero(), BLUE, shader);

  lights[3] = CreateLight(LIGHT_POINT, (Vector3){10.0f, 0.0f, 0.0f},
                          Vector3Zero(), WHITE, shader);
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, true);
#else
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif
  UnloadModel(eye);
  UnloadShader(shader);
  CloseWindow();
  return 0;
}

void UpdateDrawFrame(void) {
  float dt = GetFrameTime();

  // Spring Values
  const float stiffness = 100.0f;
  const float damping = 2.5f * sqrtf(stiffness);

  // Update camera view vector for shader
  float cameraPos[3] = {camera.position.x, camera.position.y,
                        camera.position.z};
  SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos,
                 SHADER_UNIFORM_VEC3);

  // Rotation Math
  Ray mouseRay = GetScreenToWorldRay(GetMousePosition(), camera);

  // PD controller for eye tracking
  if (!IsCursorOnScreen() && (GetTouchPointCount() == 0))
    mouseRay.direction = (Vector3){-1.0f, 0.0f, 0.0f};

  Vector3 err = Vector3Subtract(mouseRay.direction, eyeTracker.position);
  eyeTracker.acceleration = Vector3Subtract(
      Vector3Scale(err, stiffness), Vector3Scale(eyeTracker.velocity, damping));
  eyeTracker.velocity = Vector3Add(eyeTracker.velocity,
                                   Vector3Scale(eyeTracker.acceleration, dt));
  eyeTracker.position =
      Vector3Add(eyeTracker.position, Vector3Scale(eyeTracker.velocity, dt));

  if (fabsf(eyeTracker.acceleration.x) > 100.0f ||
      fabsf(eyeTracker.acceleration.y) > 100.0f ||
      fabs(eyeTracker.acceleration.z) > 100.0f)
    eyeTracker.acceleration = (Vector3){-1.0f, 0.0f, 0.0f};
  if (fabsf(eyeTracker.velocity.x) > 100.0f ||
      fabsf(eyeTracker.velocity.y) > 100.0f ||
      fabs(eyeTracker.velocity.z) > 100.0f)
    eyeTracker.velocity = (Vector3){-1.0f, 0.0f, 0.0f};
  if (fabsf(eyeTracker.position.x) > 100.0f ||
      fabsf(eyeTracker.position.y) > 100.0f ||
      fabs(eyeTracker.position.z) > 100.0f)
    eyeTracker.position = (Vector3){-1.0f, 0.0f, 0.0f};

  eyeTracker.direction = Vector3Normalize((Vector3){
      eyeTracker.position.x, eyeTracker.position.y, eyeTracker.position.z});

  float eyeYaw = atan2f(eyeTracker.direction.z, eyeTracker.direction.x);
  float eyePitch = atan2f(eyeTracker.position.y, eyeTracker.direction.x);
  eye.transform = MatrixRotateXYZ((Vector3){0.0f, eyeYaw, eyePitch});

  // Initialize Drawing
  BeginDrawing();
  ClearBackground(BLACK);
  // 3D
  BeginMode3D(camera);
  BeginShaderMode(shader);

  // Render Eye
  DrawModel(eye, eyePosition, eyeRadius, GRAY);

  EndShaderMode();
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].enabled)
      DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
    else
      DrawSphereWires(lights[i].position, 0.2f, 8, 8,
                      ColorAlpha(lights[i].color, 0.3f));
  }
  EndMode3D();

  // 2D
  DrawRectangle(10, 10, 250, 300, Fade(SKYBLUE, 0.5f));
  DrawRectangleLines(10, 10, 250, 300, BLUE);

  Color textColor = WHITE;
  DrawText("Diagnostics", 20, 20, 10, RAYWHITE);
  DrawText(TextFormat("Mouse X Position: %.2f", GetMousePosition().x), 40, 40,
           10, textColor);
  DrawText(TextFormat("Mouse Y Position: %.2f", GetMousePosition().y), 40, 60,
           10, textColor);
  DrawText(TextFormat("Mouse Ray X Position: %.2f", mouseRay.direction.x), 40,
           80, 10, textColor);
  DrawText(TextFormat("Mouse Ray Y Position: %.2f", mouseRay.direction.y), 40,
           100, 10, textColor);
  DrawText(TextFormat("Eye Tracker X Position: %.2f", eyeTracker.position.x),
           40, 120, 10, textColor);
  DrawText(TextFormat("Eye Tracker Y Position: %.2f", eyeTracker.position.y),
           40, 140, 10, textColor);
  DrawText(TextFormat("Eye Tracker X Velocity: %.2f", eyeTracker.velocity.x),
           40, 160, 10, textColor);
  DrawText(TextFormat("Eye Tracker Y Velocity: %.2f", eyeTracker.velocity.y),
           40, 180, 10, textColor);
  DrawText(TextFormat("Eye Tracker X Direction: %.2f", eyeTracker.direction.x),
           40, 200, 10, textColor);
  DrawText(TextFormat("Eye Tracker Y Direction: %.2f", eyeTracker.direction.y),
           40, 220, 10, textColor);
  DrawText(TextFormat("Eye Tracker Z Direction: %.2f", eyeTracker.direction.z),
           40, 240, 10, textColor);
  DrawText(TextFormat("Mouse Ray Pitch: %.2f", eyePitch), 40, 260, 10,
           textColor);
  DrawText(TextFormat("Mouse Ray Yaw: %.2f", eyeYaw), 40, 280, 10, textColor);

  // Termination
  EndDrawing();
}
