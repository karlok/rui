/******************************************************************
 * Raylib UI Essentials (rui.h) v1.1
 * A tiny header-only UI library for Raylib prototypes.
 *
 * Usage:
 *   #define RUI_IMPLEMENTATION
 *   #include "rui.h"
 *
 * Then in your game loop:
 *   rui_begin_frame();
 *   rui_panel_begin((Rectangle){20,20,200,300}, "Menu", true);
 *   if (rui_panel_button("Start", 30)) { ... }
 *   rui_panel_label("v1.1");
 *   rui_panel_end();
 ******************************************************************/

#ifndef RUI_H
#define RUI_H

#include "raylib.h"
#include "raymath.h"   // for Clamp()

// --- API ---
void rui_begin_frame(void);
void rui_label(const char *text, Vector2 pos);
bool rui_button(const char *text, Rectangle bounds);
void rui_panel(Rectangle bounds, const char *title);

// Auto-layout + scroll panels
void rui_panel_begin(Rectangle bounds, const char *title, bool scrollable);
bool rui_panel_button(const char *text, float height);
void rui_panel_label(const char *text);
void rui_panel_end(void);

#ifdef RUI_IMPLEMENTATION
// --- Internal State ---

// Input state
static Vector2 rui_mouse;
static bool rui_mousePressed;

// Panel layout state
static Rectangle rui_currentPanel = {0};
static float rui_panelCursorY = 0.0f;
static float rui_panelPadding = 8.0f;
static float rui_panelSpacing = 6.0f;
static bool rui_panelActive = false;

// Scroll state
static bool rui_panelScrollable = false;
static float rui_scrollOffset = 0;
static float rui_contentHeight = 0;
static float rui_prevContentHeight = 0;
static bool rui_hasPrevContent = false;
static bool rui_draggingScrollbar = false;
static float rui_dragOffsetY = 0;

void rui_begin_frame(void) {
    rui_mouse = GetMousePosition();
    rui_mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// --- Basic Widgets ---

void rui_label(const char *text, Vector2 pos) {
    DrawText(text, (int)pos.x, (int)pos.y, 20, DARKGRAY);
}

bool rui_button(const char *text, Rectangle bounds) {
    bool hovered = CheckCollisionPointRec(rui_mouse, bounds);
    bool pressed = hovered && rui_mousePressed;

    Color bg = hovered ? (Color){180, 180, 220, 255} : (Color){200, 200, 200, 255};
    if (pressed) bg = (Color){160, 160, 200, 255};

    DrawRectangleRec(bounds, bg);
    DrawRectangleLinesEx(bounds, 2, DARKGRAY);

    int textWidth = MeasureText(text, 20);
    int textX = bounds.x + (bounds.width - textWidth)/2;
    int textY = bounds.y + (bounds.height - 20)/2;
    DrawText(text, textX, textY, 20, BLACK);

    return pressed;
}

// --- Manual Panel ---
void rui_panel(Rectangle bounds, const char *title) {
    DrawRectangleRec(bounds, (Color){240, 240, 240, 255});
    DrawRectangleLinesEx(bounds, 2, DARKGRAY);

    if (title) {
        int titleHeight = 24;
        Rectangle titleBar = {bounds.x, bounds.y, bounds.width, titleHeight};
        DrawRectangleRec(titleBar, (Color){200, 200, 200, 255});
        DrawRectangleLinesEx(titleBar, 1, DARKGRAY);
        DrawText(title, (int)(bounds.x + 6), (int)(bounds.y + 4), 16, BLACK);
    }
}

// --- Auto-layout + Scrollable Panels ---
void rui_panel_begin(Rectangle bounds, const char *title, bool scrollable) {
    rui_currentPanel = bounds;
    rui_panelCursorY = bounds.y + 30.0f;
    rui_contentHeight = 0;
    rui_panelActive = true;
    rui_panelScrollable = scrollable;

    float viewHeight = bounds.height - 30.0f;
    if (scrollable) {
        float wheel = GetMouseWheelMove();
        if (CheckCollisionPointRec(rui_mouse, bounds)) {
            rui_scrollOffset += wheel * 20;   // overshoot allowed
        }

        float maxOffset = rui_prevContentHeight - viewHeight;
        if (maxOffset > 0) {
            rui_scrollOffset = Clamp(rui_scrollOffset, 0, maxOffset);
        } else if (rui_hasPrevContent) {
            rui_scrollOffset = 0;
        }
    } else {
        rui_scrollOffset = 0;
    }

    rui_panel(bounds, title);

    BeginScissorMode((int)bounds.x, (int)(bounds.y + 24), (int)bounds.width, (int)(bounds.height - 24));
}

bool rui_panel_button(const char *text, float height) {
    if (!rui_panelActive) return false;

    Rectangle r = {
        rui_currentPanel.x + rui_panelPadding,
        rui_panelCursorY - rui_scrollOffset,
        rui_currentPanel.width - rui_panelPadding*2 - (rui_panelScrollable ? 12 : 0),
        height
    };

    rui_panelCursorY += height + rui_panelSpacing;
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + 30.0f);

    return rui_button(text, r);
}

void rui_panel_label(const char *text) {
    if (!rui_panelActive) return;

    DrawText(text,
             (int)(rui_currentPanel.x + rui_panelPadding),
             (int)(rui_panelCursorY - rui_scrollOffset),
             20, DARKGRAY);

    rui_panelCursorY += 20.0f + rui_panelSpacing;
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + 30.0f);
}

void rui_panel_end(void) {
    if (rui_panelActive) {
        EndScissorMode();

        if (rui_panelScrollable && rui_contentHeight > (rui_currentPanel.height - 30)) {
            float viewHeight = rui_currentPanel.height - 30.0f;
            float maxOffset = rui_contentHeight - viewHeight;
            if (maxOffset < 0) maxOffset = 0;

            // Draw track + thumb
            float ratio = viewHeight / rui_contentHeight;
            float barHeight = viewHeight * ratio;
            float trackY = rui_currentPanel.y + 30;
            float travel = viewHeight - barHeight;

            Rectangle scrollTrack = {
                rui_currentPanel.x + rui_currentPanel.width - 10,
                trackY,
                8,
                viewHeight
            };

            float barY = trackY + (rui_scrollOffset / maxOffset) * travel;
            Rectangle scrollBar = {scrollTrack.x, barY, scrollTrack.width, barHeight};

            DrawRectangleRec(scrollTrack, LIGHTGRAY);
            bool hovered = CheckCollisionPointRec(rui_mouse, scrollBar);
            Color barColor = rui_draggingScrollbar ? BLUE : (hovered ? GRAY : DARKGRAY);
            DrawRectangleRec(scrollBar, barColor);

            // Drag input
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered) {
                rui_draggingScrollbar = true;
                rui_dragOffsetY = rui_mouse.y - scrollBar.y;
            }

            if (rui_draggingScrollbar) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    float newBarY = rui_mouse.y - rui_dragOffsetY;
                    newBarY = Clamp(newBarY, trackY, trackY + travel);
                    if (travel > 0) {
                        rui_scrollOffset = ((newBarY - trackY) / travel) * maxOffset;
                    }
                } else {
                    rui_draggingScrollbar = false;
                }
            }

            // ✅ Final clamp for both drag + wheel
            rui_scrollOffset = Clamp(rui_scrollOffset, 0, maxOffset);
        } else {
            rui_scrollOffset = 0;
        }

        rui_prevContentHeight = rui_contentHeight;
        rui_hasPrevContent = true;
        rui_panelActive = false;
    }
}

#endif // RUI_IMPLEMENTATION
#endif // RUI_H
