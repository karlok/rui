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
bool rui_button_call(const char *text, Rectangle bounds, void (*callback)(void *), void *userData); // button that invokes callback when pressed
void rui_fade_set_color(Color color); // choose overlay color for fade effect
void rui_fade_out(float duration); // start fade to opaque overlay
void rui_fade_in(float duration); // start fade to transparent overlay
bool rui_fade_active(void); // report if fade animation currently running
void rui_draw_fade(void); // draw overlay if fade alpha > 0
float rui_slider(Rectangle bounds, float value, float minValue, float maxValue); // horizontal slider control

typedef enum rui_align { // alignment options for panel content
    RUI_ALIGN_LEFT = 0, // place content against the left padding
    RUI_ALIGN_CENTER = 1, // center content within the panel interior
    RUI_ALIGN_RIGHT = 2 // place content against the right padding
} rui_align;

typedef struct rui_panel_style { // bundle configurable panel styling data
    Color bodyColor; // fill color for the panel body (alpha included)
    Color titleColor; // fill color for the optional title bar (alpha included)
    Color borderColor; // outline color for the panel border
    Color titleTextColor; // color used when drawing the panel title text
    Color labelColor; // default text color for labels inside panels
    rui_align contentAlign; // horizontal alignment for auto-laid-out widgets
} rui_panel_style;

static const rui_panel_style RUI_PANEL_STYLE_DEFAULT = { // default fully opaque panel style
    .bodyColor = {240, 240, 240, 255}, // light gray body background
    .titleColor = {200, 200, 200, 255}, // medium gray title bar
    .borderColor = {80, 80, 80, 255}, // dark gray border
    .titleTextColor = {0, 0, 0, 255}, // black title text
    .labelColor = {64, 64, 64, 255}, // dark gray default label text
    .contentAlign = RUI_ALIGN_LEFT // left align widgets by default
};

void rui_panel(Rectangle bounds, const char *title); // draw a static panel with the default style
void rui_panel_ex(Rectangle bounds, const char *title, rui_panel_style style); // draw a static panel with explicit style

// Auto-layout + scroll panels
void rui_panel_begin(Rectangle bounds, const char *title, bool scrollable); // start auto-layout panel with default style
void rui_panel_begin_ex(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style); // start panel with explicit style
bool rui_panel_button(const char *text, float height); // layout-aware button inside current panel
bool rui_panel_button_call(const char *text, float height, void (*callback)(void *), void *userData); // panel button with callback helper
void rui_panel_label(const char *text); // layout-aware label using panel style
void rui_panel_label_color(const char *text, Color color); // layout-aware label with explicit color
void rui_panel_spacer(float height); // advance layout cursor by a vertical gap
void rui_panel_set_content_width(float width); // set desired width for upcoming widgets (0 = full width)
float rui_panel_slider(float height, float value, float minValue, float maxValue); // slider using panel layout
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
    .labelColor = {64, 64, 64, 255}, // default label text color
    .contentAlign = RUI_ALIGN_LEFT // default alignment state
};
static float rui_panelInnerLeft = 0.0f; // cached inner left edge for content placement
static float rui_panelInnerRight = 0.0f; // cached inner right edge for content placement
static float rui_panelContentWidth = 0.0f; // target width for auto-layout widgets
static float rui_scrollOffsetBeforePanel = 0.0f; // backup scroll offset before panel begins

// Fade overlay state
static bool rui_fadeActive = false; // true while fade animation runs
static float rui_fadeAlpha = 0.0f; // current overlay alpha 0-255
static float rui_fadeStartAlpha = 0.0f; // starting alpha for current fade
static float rui_fadeTargetAlpha = 0.0f; // target alpha to reach at end of fade
static float rui_fadeDuration = 0.0f; // total animation time in seconds
static float rui_fadeElapsed = 0.0f; // elapsed time since fade start
static Color rui_fadeColor = {0, 0, 0, 255}; // overlay color (alpha overridden per frame)

void rui_begin_frame(void) { // grab per-frame input state
    rui_mouse = GetMousePosition(); // cache mouse coordinates
    rui_mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON); // see if left button pressed

    if (rui_fadeActive) { // advance fade animation when active
        rui_fadeElapsed += GetFrameTime(); // accrue frame delta time
        if (rui_fadeDuration <= 0.0f) { // handle zero duration edge case
            rui_fadeAlpha = rui_fadeTargetAlpha; // jump straight to target alpha
            rui_fadeActive = false; // stop animation immediately
        } else {
            float t = rui_fadeElapsed / rui_fadeDuration; // normalize elapsed time
            if (t >= 1.0f) { // clamp when animation completes
                t = 1.0f;
                rui_fadeActive = false; // mark animation done
            }
            rui_fadeAlpha = rui_fadeStartAlpha + (rui_fadeTargetAlpha - rui_fadeStartAlpha) * t; // interpolate alpha
        }
    }
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

bool rui_button_call(const char *text, Rectangle bounds, void (*callback)(void *), void *userData) { // draw button and fire callback
    bool pressed = rui_button(text, bounds); // reuse core button drawing logic
    if (pressed && callback) { // invoke user callback only when pressed and provided
        callback(userData); // call user-supplied function with context pointer
    }
    return pressed; // propagate pressed state to caller
}

float rui_slider(Rectangle bounds, float value, float minValue, float maxValue) { // draw horizontal slider and return new value
    if (maxValue < minValue) { // guard against inverted range
        float tmp = minValue;
        minValue = maxValue;
        maxValue = tmp;
    }

    if ( bounds.width < 12.0f ) bounds.width = 12.0f; // ensure room for knob
    float trackHeight = 6.0f; // thickness of slider track
    float trackY = bounds.y + (bounds.height - trackHeight) * 0.5f; // center track vertically
    Rectangle track = { bounds.x, trackY, bounds.width, trackHeight }; // track rectangle
    DrawRectangleRec(track, (Color){180, 180, 180, 255}); // draw track background

    float knobWidth = 12.0f; // knob width
    float travel = bounds.width - knobWidth; // horizontal travel distance for knob
    float clampedValue = Clamp(value, minValue, maxValue); // ensure value within range
    float t = (maxValue - minValue) > 0 ? (clampedValue - minValue) / (maxValue - minValue) : 0.0f; // normalize value 0-1
    float knobX = bounds.x + t * travel; // compute knob position
    Rectangle knob = { knobX, bounds.y + (bounds.height - knobWidth) * 0.5f, knobWidth, knobWidth }; // knob bounds

    bool hovered = CheckCollisionPointRec(rui_mouse, knob) || CheckCollisionPointRec(rui_mouse, track); // hover on knob or track
    static bool dragging = false; // track global dragging state (single slider usage per frame)
    static Rectangle activeSlider = {0}; // remember which slider is active

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        if (!dragging && hovered) { // start drag when first clicking on slider
            dragging = true;
            activeSlider = bounds;
        }
    } else {
        dragging = false; // release drag when button up
    }

    if (dragging && activeSlider.x == bounds.x && activeSlider.y == bounds.y && activeSlider.width == bounds.width) {
        float mouseT = (rui_mouse.x - bounds.x - knobWidth * 0.5f) / travel; // compute based on mouse x
        if (travel <= 0) mouseT = 0;
        if (mouseT < 0) mouseT = 0;
        if (mouseT > 1) mouseT = 1;
        clampedValue = minValue + mouseT * (maxValue - minValue); // update value based on mouse position
        t = mouseT; // update normalized value for knob drawing
        knob.x = bounds.x + t * travel; // update knob position
    }

    Color knobColor = dragging ? (Color){120, 120, 200, 255} : (hovered ? (Color){160, 160, 220, 255} : (Color){140, 140, 180, 255}); // knob tint
    DrawRectangleRec(knob, knobColor); // draw knob
    DrawRectangleLinesEx(knob, 2, DARKGRAY); // outline knob

    return clampedValue; // return potentially updated value
}

static void rui_fade_start(unsigned char targetAlpha, float duration) { // internal helper to configure fade animation
    rui_fadeStartAlpha = rui_fadeAlpha; // remember current alpha as starting point
    rui_fadeTargetAlpha = (float)targetAlpha; // store goal alpha value
    rui_fadeDuration = duration; // store duration in seconds
    rui_fadeElapsed = 0.0f; // reset elapsed timer
    if (duration <= 0.0f) { // if duration zero or negative, snap immediately
        rui_fadeAlpha = rui_fadeTargetAlpha;
        rui_fadeActive = false;
    } else {
        rui_fadeActive = true; // mark fade as active
    }
}

void rui_fade_set_color(Color color) { // choose color for fade overlay
    rui_fadeColor = color; // store desired overlay tint (alpha controlled separately)
}

void rui_fade_out(float duration) { // start fade to fully opaque overlay
    rui_fade_start(255, duration); // animate alpha to 255
}

void rui_fade_in(float duration) { // start fade back to transparent
    rui_fade_start(0, duration); // animate alpha to 0
}

bool rui_fade_active(void) { // expose whether fade animation running
    return rui_fadeActive; // simply return state flag
}

void rui_draw_fade(void) { // draw fade overlay if alpha greater than zero
    if (rui_fadeAlpha <= 0.0f) return; // nothing to draw when alpha 0
    Color overlay = rui_fadeColor; // copy base color
    overlay.a = (unsigned char)Clamp(rui_fadeAlpha, 0.0f, 255.0f); // apply animated alpha
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), overlay); // cover entire render surface
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
    rui_scrollOffsetBeforePanel = rui_scrollOffset; // remember incoming scroll offset so it can be restored for other panels

    float scrollbarWidth = scrollable ? 12.0f : 0.0f; // reserve space for scrollbar when needed
    rui_panelInnerLeft = rui_currentPanel.x + rui_panelPadding; // compute inner left boundary
    rui_panelInnerRight = rui_currentPanel.x + rui_currentPanel.width - rui_panelPadding - scrollbarWidth; // inner right boundary after padding and scrollbar
    if (rui_panelInnerRight < rui_panelInnerLeft) { // guard against inverted bounds
        rui_panelInnerRight = rui_panelInnerLeft; // collapse to avoid negative widths
    }
    rui_panelContentWidth = rui_panelInnerRight - rui_panelInnerLeft; // default content width spans full interior

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
    } else { // non-scrollable panels temporarily reset offset during their draw
        rui_scrollOffset = 0; // use zero offset for content placement
    }

    rui_panel_ex(bounds, title, style); // draw panel chrome before clipping contents

    BeginScissorMode((int)bounds.x, (int)(bounds.y + 24), (int)bounds.width, (int)(bounds.height - 24)); // clip subsequent draws to panel interior
}

bool rui_panel_button(const char *text, float height) { // add button within active panel
    if (!rui_panelActive) return false; // guard when called outside panel pair

    float innerWidth = rui_panelInnerRight - rui_panelInnerLeft; // effective interior width
    float targetWidth = rui_panelContentWidth; // requested width for widgets
    if (targetWidth <= 0.0f || targetWidth > innerWidth) targetWidth = innerWidth; // clamp width to interior

    float x = rui_panelInnerLeft; // default to left alignment
    if (targetWidth < innerWidth) { // adjust based on alignment when there is spare space
        if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_CENTER) {
            x = rui_panelInnerLeft + (innerWidth - targetWidth) * 0.5f; // center within interior
        } else if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_RIGHT) {
            x = rui_panelInnerRight - targetWidth; // stick to right edge
        }
    }

    Rectangle r = { // compute button rectangle inside panel
        x, // horizontal placement based on alignment
        rui_panelCursorY - rui_scrollOffset, // subtract scroll to position correctly
        targetWidth, // width respects alignment settings
        height // use provided height for button box
    };

    rui_panelCursorY += height + rui_panelSpacing; // advance layout cursor for next widget
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + 30.0f); // update content height with header baseline

    return rui_button(text, r); // draw button and return click state
}

bool rui_panel_button_call(const char *text, float height, void (*callback)(void *), void *userData) { // panel-aware button that fires callback
    bool pressed = rui_panel_button(text, height); // draw button via layout helper
    if (pressed && callback) { // fire callback when pressed
        callback(userData); // forward user data pointer to callback
    }
    return pressed; // surface pressed status to caller
}

void rui_panel_label(const char *text) { // add label within active panel using style color
    rui_panel_label_color(text, rui_currentPanelStyle.labelColor); // defer to color-aware helper with style default
}

void rui_panel_label_color(const char *text, Color color) { // add label within active panel using explicit color
    if (!rui_panelActive) return; // ignore calls when no panel is active

    float innerWidth = rui_panelInnerRight - rui_panelInnerLeft; // width available inside panel
    float targetWidth = rui_panelContentWidth; // requested content width
    if (targetWidth <= 0.0f || targetWidth > innerWidth) targetWidth = innerWidth; // clamp to interior

    float containerX = rui_panelInnerLeft; // default placement starts at inner left
    if (targetWidth < innerWidth) { // adjust container placement when width narrower than available
        if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_CENTER) {
            containerX = rui_panelInnerLeft + (innerWidth - targetWidth) * 0.5f; // center container
        } else if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_RIGHT) {
            containerX = rui_panelInnerRight - targetWidth; // anchor to right
        }
    }

    int textWidth = MeasureText(text, 20); // pixel width of label text
    float textX = containerX; // default left alignment
    if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_CENTER) {
        float offset = (targetWidth - (float)textWidth) * 0.5f; // center text inside container
        if (offset < 0.0f) offset = 0.0f; // prevent negative offset when text wider than container
        textX = containerX + offset; // apply centering offset
    } else if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_RIGHT) {
        float offset = targetWidth - (float)textWidth; // align text against right edge
        if (offset < 0.0f) offset = 0.0f; // clamp when text wider than container
        textX = containerX + offset; // place text near right boundary
    }

    DrawText(text, // render label text within panel
             (int)textX, // horizontal position after alignment adjustments
             (int)(rui_panelCursorY - rui_scrollOffset), // adjust for scroll offset
             20, color); // draw label using requested color

    rui_panelCursorY += 20.0f + rui_panelSpacing; // move layout cursor past label height
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + 30.0f); // refresh content height after label
}

void rui_panel_spacer(float height) { // insert vertical space inside active panel
    if (!rui_panelActive) return; // only operate inside a panel
    rui_panelCursorY += height; // advance layout cursor by requested gap
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + 30.0f); // update content height after spacer
}

void rui_panel_set_content_width(float width) { // override width used for subsequent widgets
    if (width <= 0.0f) { // zero or negative restores full interior width
        rui_panelContentWidth = rui_panelInnerRight - rui_panelInnerLeft; // reset to full width
    } else {
        float maxWidth = rui_panelInnerRight - rui_panelInnerLeft; // maximum allowed width inside panel
        if (width > maxWidth) width = maxWidth; // clamp to interior width
        rui_panelContentWidth = width; // store new target width
    }
}

float rui_panel_slider(float height, float value, float minValue, float maxValue) { // slider integrated with panel layout
    if (!rui_panelActive) return value; // bail when no active panel

    float innerWidth = rui_panelInnerRight - rui_panelInnerLeft; // available width
    float targetWidth = rui_panelContentWidth; // requested widget width
    if (targetWidth <= 0.0f || targetWidth > innerWidth) targetWidth = innerWidth; // clamp to interior

    float x = rui_panelInnerLeft; // default placement at left
    if (targetWidth < innerWidth) { // adjust for alignment when width smaller
        if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_CENTER) {
            x = rui_panelInnerLeft + (innerWidth - targetWidth) * 0.5f; // center slider
        } else if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_RIGHT) {
            x = rui_panelInnerRight - targetWidth; // align to right edge
        }
    }

    Rectangle bounds = { x, rui_panelCursorY - rui_scrollOffset, targetWidth, height }; // slider rectangle
    float newValue = rui_slider(bounds, value, minValue, maxValue); // draw slider using core helper

    rui_panelCursorY += height + rui_panelSpacing; // advance cursor after slider
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + 30.0f); // update content height

    return newValue; // return possibly updated value
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
            if (rui_panelScrollable) { // scrollable panel with no overflow resets offset
                rui_scrollOffset = 0; // keep offset zero when content fits
            } else { // restore prior offset after non-scrollable panels so other panels retain their position
                rui_scrollOffset = rui_scrollOffsetBeforePanel; // reinstate offset saved at begin
            }
        }

        if (rui_panelScrollable) { // only update prev height for scrollable panels
            rui_prevContentHeight = rui_contentHeight; // store current content height for next frame
            rui_hasPrevContent = true; // mark that we now have previous content data
        }
        rui_panelActive = false; // clear active flag until next panel begin
        rui_panelContentWidth = 0.0f; // reset content width override after finishing panel
    }
}

#endif // RUI_IMPLEMENTATION // end implementation section
#endif // RUI_H // end include guard
