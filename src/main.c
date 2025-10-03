#include "raylib.h"
#include <stdio.h> // only for printf in this demo

#define RUI_IMPLEMENTATION
#include "rui.h"

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Raylib UI Essentials");
    SetTargetFPS(60);

    // Game state
    Rectangle player = { screenWidth/2.0f - 20, screenHeight/2.0f - 20, 40, 40 };
    float speed = 200.0f;

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT)) player.x += speed * dt;
        if (IsKeyDown(KEY_LEFT)) player.x -= speed * dt;
        if (IsKeyDown(KEY_UP)) player.y -= speed * dt;
        if (IsKeyDown(KEY_DOWN)) player.y += speed * dt;

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            rui_begin_frame();

            // Scrollable panel with many, many buttons
            rui_panel_begin((Rectangle) { 50, 50, 200, 300 }, "Many Buttons", true);

            for (int i = 0; i < 20; i++) {
                if (rui_panel_button(TextFormat("Item %d", i+1), 30)) { // use TextFormat from raylib to build custom button label
                    printf("Clicked %d\n", i+1); // debug print to ensure it's working
                }
            }

            rui_panel_end();

            DrawRectangleRec(player, BLUE);

        EndDrawing();
    }

    // Cleanup
    CloseWindow();
    return 0;
}
