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

#ifndef RUI_H // begin include guard
#define RUI_H // define include guard token

#include "raylib.h" // pull in core raylib drawing/input API
#include "raymath.h"   // for Clamp() helper function

// --- API ---
void rui_begin_frame(void); // prepare UI input state for the frame
void rui_label(const char *text, Vector2 pos); // draw a basic label at a position
void rui_label_color(const char *text, Vector2 pos, Color color); // draw a label with explicit color
bool rui_button(const char *text, Rectangle bounds); // draw button and report click

typedef struct rui_panel_style { // bundle configurable panel styling data
    Color bodyColor; // fill color for the panel body (alpha included)
    Color titleColor; // fill color for the optional title bar (alpha included)
    Color borderColor; // outline color for the panel border
    Color titleTextColor; // color used when drawing the panel title text
    Color labelColor; // default text color for labels inside panels
} rui_panel_style;

static const rui_panel_style RUI_PANEL_STYLE_DEFAULT = { // default fully opaque panel style
    .bodyColor = {240, 240, 240, 255}, // light gray body background
    .titleColor = {200, 200, 200, 255}, // medium gray title bar
    .borderColor = {80, 80, 80, 255}, // dark gray border
    .titleTextColor = {0, 0, 0, 255}, // black title text
    .labelColor = {64, 64, 64, 255} // dark gray default label text
};

void rui_panel(Rectangle bounds, const char *title); // draw a static panel with the default style
void rui_panel_ex(Rectangle bounds, const char *title, rui_panel_style style); // draw a static panel with explicit style

// Auto-layout + scroll panels
void rui_panel_begin(Rectangle bounds, const char *title, bool scrollable); // start auto-layout panel with default style
void rui_panel_begin_ex(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style); // start panel with explicit style
bool rui_panel_button(const char *text, float height); // layout-aware button inside current panel
void rui_panel_label(const char *text); // layout-aware label using panel style
void rui_panel_label_color(const char *text, Color color); // layout-aware label with explicit color
void rui_panel_end(void); // finish current panel and draw scrollbar if needed

#ifdef RUI_IMPLEMENTATION // compile implementation when requested
// --- Internal State ---

// Input state
static Vector2 rui_mouse; // mouse position captured each frame
static bool rui_mousePressed; // true if mouse button pressed this frame

// Panel layout state
static Rectangle rui_currentPanel = {0}; // rectangle describing active panel
static float rui_panelCursorY = 0.0f; // current y offset for auto-layout widgets
static float rui_panelPadding = 8.0f; // horizontal padding inside panels
static float rui_panelSpacing = 6.0f; // vertical spacing between widgets
static bool rui_panelActive = false; // flag to guard panel-only calls

// Scroll state
static bool rui_panelScrollable = false; // panel scrolling enabled flag
static float rui_scrollOffset = 0; // current scroll offset in pixels
static float rui_contentHeight = 0; // height consumed by widgets this frame
static float rui_prevContentHeight = 0; // previous frame's content height
static bool rui_hasPrevContent = false; // tracks if previous height is valid
static bool rui_draggingScrollbar = false; // true while thumb is being dragged
static float rui_dragOffsetY = 0; // mouse-to-thumb offset during drag

static rui_panel_style rui_currentPanelStyle = { // active style for panel-driven widgets
    .bodyColor = {240, 240, 240, 255}, // default body color
    .titleColor = {200, 200, 200, 255}, // default title color
    .borderColor = {80, 80, 80, 255}, // default border
    .titleTextColor = {0, 0, 0, 255}, // default title text color
    .labelColor = {64, 64, 64, 255} // default label text color
};

void rui_begin_frame(void) { // grab per-frame input state
    rui_mouse = GetMousePosition(); // cache mouse coordinates
    rui_mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON); // see if left button pressed
}

// --- Basic Widgets ---

void rui_label(const char *text, Vector2 pos) { // draw plain text label with default color
    rui_label_color(text, pos, DARKGRAY); // delegate to colored label helper with default tone
}

void rui_label_color(const char *text, Vector2 pos, Color color) { // draw text label with supplied color
    DrawText(text, (int)pos.x, (int)pos.y, 20, color); // render text in provided color at position
}

bool rui_button(const char *text, Rectangle bounds) { // draw interactive button
    bool hovered = CheckCollisionPointRec(rui_mouse, bounds); // check if mouse over button
    bool pressed = hovered && rui_mousePressed; // register press only when hovered

    Color bg = hovered ? (Color){180, 180, 220, 255} : (Color){200, 200, 200, 255}; // choose hover background color
    if (pressed) bg = (Color){160, 160, 200, 255}; // darken when actively pressed

    DrawRectangleRec(bounds, bg); // fill button background
    DrawRectangleLinesEx(bounds, 2, DARKGRAY); // outline button for contrast

    int textWidth = MeasureText(text, 20); // width of label at font size
    int textX = bounds.x + (bounds.width - textWidth)/2; // center text horizontally
    int textY = bounds.y + (bounds.height - 20)/2; // center text vertically
    DrawText(text, textX, textY, 20, BLACK); // draw button label in black

    return pressed; // return true when clicked
}

// --- Manual Panel ---
void rui_panel(Rectangle bounds, const char *title) { // draw basic panel using default style
    rui_panel_ex(bounds, title, RUI_PANEL_STYLE_DEFAULT); // forward to full implementation with default style
}

void rui_panel_ex(Rectangle bounds, const char *title, rui_panel_style style) { // draw a static panel shell with style
    DrawRectangleRec(bounds, style.bodyColor); // paint panel background with styled color
    DrawRectangleLinesEx(bounds, 2, style.borderColor); // outline panel borders using styled color

    if (title) { // draw optional title bar when provided
        int titleHeight = 24; // fixed title bar height
        Rectangle titleBar = {bounds.x, bounds.y, bounds.width, titleHeight}; // rectangle for title area
        DrawRectangleRec(titleBar, style.titleColor); // fill title bar with styled color
        DrawRectangleLinesEx(titleBar, 1, style.borderColor); // outline title bar for separation
        DrawText(title, (int)(bounds.x + 6), (int)(bounds.y + 4), 16, style.titleTextColor); // render title text using styled color
    }
}

// --- Auto-layout + Scrollable Panels ---
void rui_panel_begin(Rectangle bounds, const char *title, bool scrollable) { // start a managed panel with default style
    rui_panel_begin_ex(bounds, title, scrollable, RUI_PANEL_STYLE_DEFAULT); // call extended version with default style
}

void rui_panel_begin_ex(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style) { // start panel with custom style
    rui_currentPanel = bounds; // remember panel rectangle for children
    rui_panelCursorY = bounds.y + 30.0f; // initialize layout cursor beneath header
    rui_contentHeight = 0; // reset content height for this frame
    rui_panelActive = true; // mark panel as active for child widgets
    rui_panelScrollable = scrollable; // store whether scrolling is enabled
    rui_currentPanelStyle = style; // store style for child widgets rendered this frame

    float viewHeight = bounds.height - 30.0f; // compute visible height excluding header
    if (scrollable) { // only read scroll input for scrollable panels
        float wheel = GetMouseWheelMove(); // get wheel delta for this frame
        if (CheckCollisionPointRec(rui_mouse, bounds)) { // ensure cursor is over panel
            rui_scrollOffset += wheel * 20;   // overshoot allowed to keep wheel responsive
        }

        float maxOffset = rui_prevContentHeight - viewHeight; // maximum scroll based on previous content
        if (maxOffset > 0) { // clamp only when content exceeds view
            rui_scrollOffset = Clamp(rui_scrollOffset, 0, maxOffset); // keep scroll offset within bounds
        } else if (rui_hasPrevContent) { // if previous content was smaller, reset offset
            rui_scrollOffset = 0; // snap back to top when nothing to scroll
        }
    } else { // non-scrollable panels always stay at top
        rui_scrollOffset = 0; // ensure no offset leaks between panels
    }

    rui_panel_ex(bounds, title, style); // draw panel chrome before clipping contents

    BeginScissorMode((int)bounds.x, (int)(bounds.y + 24), (int)bounds.width, (int)(bounds.height - 24)); // clip subsequent draws to panel interior
}

bool rui_panel_button(const char *text, float height) { // add button within active panel
    if (!rui_panelActive) return false; // guard when called outside panel pair

    Rectangle r = { // compute button rectangle inside panel
        rui_currentPanel.x + rui_panelPadding, // apply horizontal padding
        rui_panelCursorY - rui_scrollOffset, // subtract scroll to position correctly
        rui_currentPanel.width - rui_panelPadding*2 - (rui_panelScrollable ? 12 : 0), // reduce width for padding and scrollbar
        height // use provided height for button box
    };

    rui_panelCursorY += height + rui_panelSpacing; // advance layout cursor for next widget
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + 30.0f); // update content height with header baseline

    return rui_button(text, r); // draw button and return click state
}

void rui_panel_label(const char *text) { // add label within active panel using style color
    rui_panel_label_color(text, rui_currentPanelStyle.labelColor); // defer to color-aware helper with style default
}

void rui_panel_label_color(const char *text, Color color) { // add label within active panel using explicit color
    if (!rui_panelActive) return; // ignore calls when no panel is active

    DrawText(text, // render label text within panel
             (int)(rui_currentPanel.x + rui_panelPadding), // offset text from left padding
             (int)(rui_panelCursorY - rui_scrollOffset), // adjust for scroll offset
             20, color); // draw label using requested color

    rui_panelCursorY += 20.0f + rui_panelSpacing; // move layout cursor past label height
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + 30.0f); // refresh content height after label
}

void rui_panel_end(void) { // finish panel rendering and handle scrollbars
    if (rui_panelActive) { // only proceed if a panel was begun
        EndScissorMode(); // stop clipping so scrollbar can draw outside content area

        if (rui_panelScrollable && rui_contentHeight > (rui_currentPanel.height - 30)) { // only show scrollbar when needed
            float viewHeight = rui_currentPanel.height - 30.0f; // visible content height
            float maxOffset = rui_contentHeight - viewHeight; // maximum scroll offset possible
            if (maxOffset < 0) maxOffset = 0; // guard against negatives (precision)

            // Draw track + thumb
            float ratio = viewHeight / rui_contentHeight; // portion of content visible
            float barHeight = viewHeight * ratio; // scrollbar thumb height proportional to ratio
            float trackY = rui_currentPanel.y + 30; // scrollbar track start at content top
            float travel = viewHeight - barHeight; // distance thumb can travel along track

            Rectangle scrollTrack = { // rectangle representing scrollbar track
                rui_currentPanel.x + rui_currentPanel.width - 10, // align track to right edge
                trackY, // start track below panel header
                8, // fixed track width
                viewHeight // track height matches viewable content
            };

            float barY = trackY + (rui_scrollOffset / maxOffset) * travel; // thumb position based on scroll fraction
            Rectangle scrollBar = {scrollTrack.x, barY, scrollTrack.width, barHeight}; // rectangle for draggable thumb

            DrawRectangleRec(scrollTrack, LIGHTGRAY); // draw track background
            bool hovered = CheckCollisionPointRec(rui_mouse, scrollBar); // detect hover over thumb
            Color barColor = rui_draggingScrollbar ? BLUE : (hovered ? GRAY : DARKGRAY); // change color when dragging or hovered
            DrawRectangleRec(scrollBar, barColor); // draw thumb with computed color

            // Drag input
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered) { // start dragging when thumb clicked
                rui_draggingScrollbar = true; // flag dragging state
                rui_dragOffsetY = rui_mouse.y - scrollBar.y; // remember grab offset within thumb
            }

            if (rui_draggingScrollbar) { // while in drag mode
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) { // continue while button is held
                    float newBarY = rui_mouse.y - rui_dragOffsetY; // proposed thumb position following mouse
                    newBarY = Clamp(newBarY, trackY, trackY + travel); // constrain thumb to track bounds
                    if (travel > 0) { // avoid divide-by-zero when no travel
                        rui_scrollOffset = ((newBarY - trackY) / travel) * maxOffset; // convert thumb position to scroll offset
                    }
                } else { // mouse button released
                    rui_draggingScrollbar = false; // exit dragging state
                }
            }

            // ✅ Final clamp for both drag + wheel
            rui_scrollOffset = Clamp(rui_scrollOffset, 0, maxOffset); // enforce valid offset after interactions
        } else {
            rui_scrollOffset = 0; // reset scroll when no scrollbar is needed
        }

        rui_prevContentHeight = rui_contentHeight; // store current content height for next frame
        rui_hasPrevContent = true; // mark that we now have previous content data
        rui_panelActive = false; // clear active flag until next panel begin
    }
}

#endif // RUI_IMPLEMENTATION // end implementation section
#endif // RUI_H // end include guard
