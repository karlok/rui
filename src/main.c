#include "raylib.h"
#include <stdio.h> // only for printf in this demo

#define RUI_IMPLEMENTATION
#include "rui.h"

static void on_menu_item(void *ctx) // callback fired when list button clicked
{
    int index = ctx ? *(int *)ctx : -1; // read item index passed via user data
    printf("Clicked %d\n", index); // print selection to console for demo
}

static void on_volume_changed(float value, void *ctx) // slider callback demo
{
    (void)ctx; // unused context
    printf("Volume %.2f\n", value); // print new value for demonstration
}

static void on_music_toggled(bool enabled, void *ctx) // toggle callback demo
{
    (void)ctx; // unused context
    printf("Music %s\n", enabled ? "ON" : "OFF"); // report toggle state
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Raylib UI Essentials");
    SetTargetFPS(60);

    rui_fade_set_color((Color){0, 0, 0, 255}); // default fade overlay to black

    rui_theme theme = rui_theme_default();
    theme.textFont.size = 22;
    theme.titleFont.size = 20;
    rui_theme_set(&theme);

    // Loading a custom font
    Font ui = LoadFontEx("src/assets/Anonymous_Pro.ttf", 48, NULL, 0);
    theme = rui_theme_default();
    theme.textFont.font = ui;
    theme.textFont.size = 16;
    theme.textFont.spacing = 1.5f;
    theme.titleFont = theme.textFont;
    theme.titleFont.size = 20;
    rui_theme_set(&theme);

    float nameFieldHeight = theme.textFont.size + 10.0f;

    // Game state
    Rectangle player = { screenWidth/2.0f - 20, screenHeight/2.0f - 20, 40, 40 };
    float speed = 200.0f;
    float musicVolume = 0.5f; // sample slider-controlled value
    bool musicEnabled = true;
    bool showInfoPanel = true;
    char nameBuffer[32] = "Player";
    rui_text_input nameInput = rui_text_input_init(nameBuffer, sizeof(nameBuffer));

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        float dt = GetFrameTime();

        bool uiCapturesKeyboard = rui_keyboard_captured();

        if (!uiCapturesKeyboard) {
            if (IsKeyDown(KEY_RIGHT)) player.x += speed * dt;
            if (IsKeyDown(KEY_LEFT)) player.x -= speed * dt;
            if (IsKeyDown(KEY_UP)) player.y -= speed * dt;
            if (IsKeyDown(KEY_DOWN)) player.y += speed * dt;

            if (IsKeyPressed(KEY_F)) rui_fade_out(0.6f); // trigger fade to black when UI not capturing input
            if (IsKeyPressed(KEY_G)) rui_fade_in(0.6f); // trigger fade back in
        }

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangleRec(player, BLUE);

            rui_begin_frame();

            if (showInfoPanel) {
                Rectangle infoPanel = { 400, 50, 200, 100 };
                rui_panel_style infoStyle = {
                    .bodyColor = { 30, 60, 120, 230 }, // soft blue body background
                    .titleColor = { 20, 40, 90, 255 }, // deeper header tone
                    .borderColor = { 255, 200, 30, 255 }, // high-contrast border
                    .titleTextColor = { 255, 255, 255, 255 }, // white title text
                    .labelColor = { 255, 255, 255, 255 }, // prefer white labels for contrast
                    .contentAlign = RUI_ALIGN_LEFT // left aligns label within the panel
                };
                bool closed = rui_panel_begin_ex_closable(infoPanel, "Info", false, infoStyle, NULL);
                if (closed) {
                    showInfoPanel = false;
                } else {
                    rui_panel_label_color("Hello there", WHITE);
                }
                rui_panel_end();
            }

            // Scrollable panel with many, many buttons
            rui_panel_style listStyle = {
                .bodyColor = { 235, 235, 235, 220 }, // gentle fade for scroll body
                .titleColor = { 190, 190, 190, 200 }, // semi-transparent title bar
                .borderColor = { 80, 80, 80, 255 }, // classic dark border
                .titleTextColor = { 40, 40, 40, 255 }, // dark heading text
                .labelColor = { 30, 30, 30, 255 }, // panel labels lean darker for contrast
                .contentAlign = RUI_ALIGN_LEFT // keep list content left aligned
            };
            rui_panel_begin_ex((Rectangle) { 50, 50, 200, 300 }, "Many Buttons", true, listStyle);

            rui_panel_label("Player Name");
            if (rui_panel_text_input(nameFieldHeight, &nameInput)) {
                printf("Name %s\n", nameBuffer);
            }

            rui_panel_spacer(12.0f);
            rui_panel_label("Music Volume");
            musicVolume = rui_panel_slider_call(24.0f, musicVolume, 0.0f, 1.0f, on_volume_changed, NULL); // adjust demo value
            rui_panel_label(TextFormat("%.2f", musicVolume));

            const char *musicLabel = musicEnabled ? "Disable music" : "Enable music";
            musicEnabled = rui_panel_toggle_call(musicEnabled, musicLabel, on_music_toggled, NULL);

            rui_panel_spacer(12.0f);

            for (int i = 0; i < 20; i++) {
                int itemIndex = i + 1; // capture button index for callback context
                rui_panel_button_call(TextFormat("Item %d", itemIndex), 30, on_menu_item, &itemIndex); // invoke callback helper
            }

            rui_panel_end();

            rui_draw_fade();

        EndDrawing();
    }

    // Cleanup
    CloseWindow();
    return 0;
}
