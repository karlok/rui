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

            DrawRectangleRec(player, BLUE);

            rui_begin_frame();

            // simple panel with label
            Rectangle infoPanel = { 400, 50, 200, 100 };
            rui_panel_style infoStyle = {
                .bodyColor = { 30, 60, 120, 230 }, // soft blue body background
                .titleColor = { 20, 40, 90, 255 }, // deeper header tone (unused without title)
                .borderColor = { 255, 200, 30, 255 }, // high-contrast border
                .titleTextColor = { 255, 255, 255, 255 }, // white title text if added later
                .labelColor = { 255, 255, 255, 255 } // prefer white labels for contrast
            };
            rui_panel_ex(infoPanel, NULL, infoStyle); // draw translucent panel without title
            rui_label_color("Hello there", (Vector2){ infoPanel.x + 12, infoPanel.y + 36 }, WHITE);

            // Scrollable panel with many, many buttons
            rui_panel_style listStyle = {
                .bodyColor = { 235, 235, 235, 220 }, // gentle fade for scroll body
                .titleColor = { 190, 190, 190, 200 }, // semi-transparent title bar
                .borderColor = { 80, 80, 80, 255 }, // classic dark border
                .titleTextColor = { 40, 40, 40, 255 }, // dark heading text
                .labelColor = { 30, 30, 30, 255 } // panel labels lean darker for contrast
            };
            rui_panel_begin_ex((Rectangle) { 50, 50, 200, 300 }, "Many Buttons", true, listStyle);

            for (int i = 0; i < 20; i++) {
                if (rui_panel_button(TextFormat("Item %d", i+1), 30)) { // use TextFormat from raylib to build custom button label
                    printf("Clicked %d\n", i+1); // debug print to ensure it's working
                }
            }

            rui_panel_end();

        EndDrawing();
    }

    // Cleanup
    CloseWindow();
    return 0;
}
