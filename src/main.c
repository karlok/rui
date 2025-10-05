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

#define MAX_ITEM_PANELS 4

typedef struct PanelFade {
    bool visible;
    bool closing;
    float alpha;
    float speed;
    Rectangle bounds;
    int itemIndex;
    const char *emoji;
    char title[32];
} PanelFade;

static float move_towards(float value, float target, float maxDelta)
{
    if (value < target) {
        value += maxDelta;
        if (value > target) value = target;
    } else if (value > target) {
        value -= maxDelta;
        if (value < target) value = target;
    }
    return value;
}

static void panel_fade_update(PanelFade *panel, float dt)
{
    if (!panel->visible && panel->alpha <= 0.0f) return;
    float target = panel->closing ? 0.0f : 1.0f;
    panel->alpha = move_towards(panel->alpha, target, panel->speed * dt);
    if (panel->closing && panel->alpha <= 0.01f) {
        panel->alpha = 0.0f;
        panel->closing = false;
        panel->visible = false;
    }
}

static const char *ITEM_EMOJIS[] = {
    "🍎", "🗡️", "🛡️", "🧪", "💎", "🔥", "⚙️", "🌟"
};
static const int ITEM_EMOJI_COUNT = (int)(sizeof(ITEM_EMOJIS) / sizeof(ITEM_EMOJIS[0]));

static void open_item_panel(PanelFade *panels, int count, const Rectangle *slots, int itemIndex)
{
    int slot = -1;
    for (int i = 0; i < count; ++i) {
        if (!panels[i].visible && panels[i].alpha <= 0.01f) {
            slot = i;
            break;
        }
    }
    if (slot == -1) slot = 0;

    PanelFade *panel = &panels[slot];
    panel->visible = true;
    panel->closing = false;
    panel->alpha = 0.0f;
    panel->speed = 4.0f;
    panel->bounds = slots[slot];
    panel->itemIndex = itemIndex;
    panel->emoji = ITEM_EMOJIS[itemIndex % ITEM_EMOJI_COUNT];
    snprintf(panel->title, sizeof(panel->title), "Item %d", itemIndex);
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
    if (ui.texture.id != 0) {
        theme = rui_theme_default();
        theme.textFont.font = ui;
        theme.textFont.size = 16;
        theme.textFont.spacing = 1.5f;
        theme.titleFont = theme.textFont;
        theme.titleFont.size = 20;
        rui_theme_set(&theme);
    }

    int emojiCodes[] = {
        0x1F34E, // 🍎
        0x1F5E1, // 🗡
        0x1F6E1, // 🛡
        0x1F9EA, // 🧪
        0x1F48E, // 💎
        0x1F525, // 🔥
        0x2699,  // ⚙
        0x1F31F, // 🌟
        0x0000
    };
    Font emojiFont = LoadFontEx("src/assets/NotoEmoji-Regular.ttf", 64, emojiCodes, (sizeof(emojiCodes) / sizeof(int)) - 1);

    float nameFieldHeight = theme.textFont.size + 10.0f;

    Rectangle itemPanelSlots[MAX_ITEM_PANELS] = {
        {280, 220, 200, 160},
        {520, 220, 200, 160},
        {280, 400, 200, 160},
        {520, 400, 200, 160}
    };
    PanelFade itemPanels[MAX_ITEM_PANELS] = {0};
    for (int i = 0; i < MAX_ITEM_PANELS; ++i) {
        itemPanels[i].bounds = itemPanelSlots[i];
        itemPanels[i].speed = 4.0f;
        itemPanels[i].alpha = 0.0f;
        itemPanels[i].visible = false;
        itemPanels[i].closing = false;
        itemPanels[i].emoji = ITEM_EMOJIS[i % ITEM_EMOJI_COUNT];
        itemPanels[i].title[0] = '\0';
    }

    bool infoVisible = true;
    bool infoClosing = false;
    float infoAlpha = 1.0f;
    float infoFadeSpeed = 4.0f;

    // Game state
    Rectangle player = { screenWidth/2.0f - 20, screenHeight/2.0f - 20, 40, 40 };
    float speed = 200.0f;
    float musicVolume = 0.5f; // sample slider-controlled value
    bool musicEnabled = true;
    char nameBuffer[32] = "Player";
    rui_text_input nameInput = rui_text_input_init(nameBuffer, sizeof(nameBuffer));

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_I)) {
            infoVisible = true;
            infoClosing = false;
            if (infoAlpha <= 0.0f) infoAlpha = 0.0f;
        }

        float infoTarget = infoVisible ? 1.0f : 0.0f;
        infoAlpha = move_towards(infoAlpha, infoTarget, infoFadeSpeed * dt);
        if (!infoVisible && infoAlpha <= 0.01f) {
            infoAlpha = 0.0f;
            infoClosing = false;
        } else if (!infoVisible && infoAlpha > 0.0f) {
            infoClosing = true;
        } else if (infoVisible && infoAlpha >= 0.99f) {
            infoClosing = false;
        }

        for (int i = 0; i < MAX_ITEM_PANELS; ++i) {
            panel_fade_update(&itemPanels[i], dt);
        }

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

            if (infoVisible || infoClosing || infoAlpha > 0.01f) {
                Rectangle infoPanel = { 400, 50, 200, 100 };
                rui_panel_style infoStyle = {
                    .bodyColor = { 30, 60, 120, 230 }, // soft blue body background
                    .titleColor = { 20, 40, 90, 255 }, // deeper header tone
                    .borderColor = { 255, 200, 30, 255 }, // high-contrast border
                    .titleTextColor = { 255, 255, 255, 255 }, // white title text
                    .labelColor = { 255, 255, 255, 255 }, // prefer white labels for contrast
                    .contentAlign = RUI_ALIGN_LEFT // left aligns label within the panel
                };
                bool closed = rui_panel_begin_ex_closable_fade(infoPanel, "Info", false, infoStyle, infoAlpha, NULL);
                if (closed) {
                    infoVisible = false;
                    infoClosing = true;
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
                bool pressed = rui_panel_button_call(TextFormat("Item %d", itemIndex), 30, on_menu_item, &itemIndex); // invoke callback helper
                if (pressed) {
                    open_item_panel(itemPanels, MAX_ITEM_PANELS, itemPanelSlots, itemIndex);
                }
            }

            rui_panel_end();

            for (int i = 0; i < MAX_ITEM_PANELS; ++i) {
                PanelFade *panel = &itemPanels[i];
                if (!panel->visible && !panel->closing && panel->alpha <= 0.01f) continue;

                rui_panel_style itemStyle = listStyle;
                const char *title = panel->title[0] ? panel->title : TextFormat("Item %d", panel->itemIndex);
                bool closed = rui_panel_begin_ex_closable_fade(panel->bounds, title, false, itemStyle, panel->alpha, NULL);
                if (closed) {
                    panel->visible = false;
                    panel->closing = true;
                }

                rui_panel_spacer(6.0f);
                rui_panel_label(TextFormat("Selected item %d", panel->itemIndex));
                rui_panel_spacer(6.0f);

                rui_theme savedTheme = *rui_theme_get();
                rui_theme bigTheme = savedTheme;
                bigTheme.textFont.size = savedTheme.textFont.size * 3;
                bigTheme.textFont.spacing = savedTheme.textFont.spacing * 2.0f;
                if (emojiFont.texture.id != 0) {
                    bigTheme.textFont.font = emojiFont;
                }
                rui_theme_set(&bigTheme);
                rui_panel_label_color(panel->emoji ? panel->emoji : "✨", savedTheme.panel.labelColor);
                rui_theme_set(&savedTheme);

                rui_panel_spacer(6.0f);
                rui_panel_label("Click the X to close");
                rui_panel_end();
            }

            rui_draw_fade();

        EndDrawing();
    }

    // Cleanup
    if (ui.texture.id != 0) UnloadFont(ui);
    if (emojiFont.texture.id != 0) UnloadFont(emojiFont);
    CloseWindow();
    return 0;
}
