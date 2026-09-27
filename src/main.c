#include "raylib.h"
#include "raymath.h"
#include "field.h"
#include <math.h>
#include <string.h>

#define GRID 80
#define EXTENT 10.0f
#define BALL_RADIUS 0.48f
static Vector3 surface[GRID + 1][GRID + 1];

static Vector3 BallPosition(Vector2 ball)
{
    return (Vector3){ball.x, FieldHeight(ball.x, ball.y, ball.x, ball.y) + BALL_RADIUS, ball.y};
}

/* Fixed-height drag plane avoids feedback between picking and deformation. */
static bool PlaneHit(Ray ray, float height, Vector3 *hit)
{
    if (fabsf(ray.direction.y) < 0.0001f) return false;
    float t = (height - ray.position.y)/ray.direction.y;
    if (t < 0.0f) return false;
    *hit = Vector3Add(ray.position, Vector3Scale(ray.direction, t));
    return true;
}

static void DrawField(Vector2 ball, bool filled)
{
    for (int z = 0; z <= GRID; z++)
        for (int x = 0; x <= GRID; x++) {
            float wx = -EXTENT + 2.0f*EXTENT*(float)x/GRID;
            float wz = -EXTENT + 2.0f*EXTENT*(float)z/GRID;
            surface[z][x] = (Vector3){wx, FieldHeight(wx, wz, ball.x, ball.y), wz};
        }
    for (int z = 0; z < GRID; z++)
        for (int x = 0; x < GRID; x++) {
            Vector3 a = surface[z][x], b = surface[z + 1][x];
            Vector3 c = surface[z + 1][x + 1], d = surface[z][x + 1];
            if (filled) {
                float depth = Clamp(-a.y/5.35f, 0, 1);
                Color tint = ColorLerp((Color){22, 43, 55, 255}, (Color){50, 24, 64, 255}, depth);
                DrawTriangle3D(a, b, c, tint);
                DrawTriangle3D(a, c, d, tint);
            }
        }
    for (int z = 0; z <= GRID; z++)
        for (int x = 0; x <= GRID; x++) {
            Vector3 a = surface[z][x];
            a.y += 0.018f; /* Small offset prevents surface/grid z-fighting. */
            Color ink = (Color){53, 113, 133, 255};
            if (x < GRID) {
                Vector3 b = surface[z][x + 1]; b.y += 0.018f;
                DrawLine3D(a, b, ink);
            }
            if (z < GRID) {
                Vector3 b = surface[z + 1][x]; b.y += 0.018f;
                DrawLine3D(a, b, ink);
            }
        }
}

int main(int argc, char **argv)
{
    bool smoke = argc > 1 && strcmp(argv[1], "--smoke-test") == 0;
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "Campo gravitacional | C + raylib");
    if (!IsWindowReady()) return 1;
    SetWindowMinSize(800, 600);
    SetTargetFPS(60);
    Camera3D camera = {0};
    camera.target = (Vector3){0, -1.0f, 0};
    camera.up = (Vector3){0, 1, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;
    Vector2 ball = {4, 3}, offset = {0};
    float yaw = 0.75f, pitch = 0.85f, distance = 27, dragHeight = 0;
    bool dragging = false, filled = true;
    int frames = 0;
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            ball = (Vector2){4, 3}; yaw = 0.75f; pitch = 0.85f;
            distance = 27; dragging = false;
        }
        if (IsKeyPressed(KEY_W)) filled = !filled;
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) || !IsWindowFocused()) dragging = false;
        if (!dragging) {
            distance = Clamp(distance - GetMouseWheelMove()*1.5f, 12, 42);
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                Vector2 delta = GetMouseDelta();
                yaw -= delta.x*0.005f;
                pitch = Clamp(pitch + delta.y*0.005f, 0.35f, 1.45f);
            }
        }
        camera.position = Vector3Add(camera.target, (Vector3){
            distance*cosf(pitch)*sinf(yaw), distance*sinf(pitch),
            distance*cosf(pitch)*cosf(yaw)});
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        Vector3 position = BallPosition(ball), hit;
        bool hover = GetRayCollisionSphere(ray, position, BALL_RADIUS).hit;
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dragHeight = position.y;
            if (PlaneHit(ray, dragHeight, &hit)) {
                dragging = true;
                offset = (Vector2){ball.x - hit.x, ball.y - hit.z};
            }
        }
        if (dragging && PlaneHit(ray, dragHeight, &hit)) {
            ball.x = Clamp(hit.x + offset.x, -9.0f, 9.0f);
            ball.y = Clamp(hit.z + offset.y, -9.0f, 9.0f);
        }
        position = BallPosition(ball);
        SetMouseCursor(dragging || hover ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
        BeginDrawing();
        ClearBackground((Color){9, 14, 23, 255});
        BeginMode3D(camera);
        DrawField(ball, filled);
        /* The black marker remains at the bottom of the permanent central well. */
        Vector3 hole = {0, -3.92f, 0};
        DrawSphere(hole, 0.68f, (Color){2, 2, 6, 255});
        DrawCircle3D(hole, 0.82f, (Vector3){1, 0, 0}, 90, ORANGE);
        DrawCircle3D(hole, 0.91f, (Vector3){1, 0, 0}, 90, (Color){124, 66, 36, 255});
        DrawSphereEx(position, BALL_RADIUS, 20, 28, dragging ? GOLD : SKYBLUE);
        DrawSphereWires(position, BALL_RADIUS + 0.004f, 8, 12, (Color){36, 97, 139, 255});
        EndMode3D();
        DrawRectangle(0, 0, GetScreenWidth(), 100, (Color){9, 14, 23, 240});
        DrawText("CAMPO GRAVITACIONAL", 24, 18, 26, RAYWHITE);
        DrawText("Arraste a esfera azul e observe a depressao na malha.", 24, 54, 18, SKYBLUE);
        DrawText("Modelo visual simplificado", 24, 78, 14, GRAY);
        int footer = GetScreenHeight() - 66;
        DrawRectangle(0, footer, GetScreenWidth(), 66, (Color){9, 14, 23, 240});
        DrawText("Esquerdo: arrastar esfera   |   Direito: orbitar   |   Roda: zoom", 24, footer + 12, 18, RAYWHITE);
        DrawText("R: reiniciar   |   W: superficie / linhas   |   Esc: sair", 24, footer + 38, 16, GRAY);
        DrawFPS(GetScreenWidth() - 90, 22);
        EndDrawing();
        if (smoke && ++frames >= 8) break;
    }
    CloseWindow();
    return 0;
}
