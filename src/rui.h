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
#include <math.h> // for fmodf used in caret blinking

typedef struct rui_text_input { // state for single-line text input
    char *buffer; // pointer to caller-provided character buffer
    int capacity; // capacity of buffer including terminating null
    int length; // current length of text (excluding null)
    int cursor; // caret index within text
    bool active; // whether this input currently has focus
    float blinkTimer; // timer used for caret blinking
} rui_text_input;

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

typedef struct rui_button_style { // configurable button colours
    Color normal; // default fill
    Color hover; // fill when hovered
    Color pressed; // fill when pressed
    Color border; // outline colour
    Color text; // label colour
} rui_button_style;

typedef struct rui_slider_style { // configurable slider colours
    Color track; // background track colour
    Color knob; // idle knob colour
    Color knobHover; // knob colour while hovered
    Color knobDrag; // knob colour while dragging
} rui_slider_style;

typedef struct rui_toggle_style { // configurable toggle/checkbox colours
    Color border; // default border
    Color borderHover; // border when hovered
    Color fill; // interior when inactive
    Color fillActive; // interior when active
    Color label; // label text colour
} rui_toggle_style;

typedef struct rui_font_style { // font + sizing info
    Font font; // raylib font handle
    int size; // pixel size
    float spacing; // extra spacing for DrawTextEx
} rui_font_style;

typedef struct rui_text_input_style { // colours for text inputs
    Color background; // box fill
    Color border; // default border
    Color borderHover; // border when hovered
    Color borderActive; // border when focused
    Color text; // text colour
    Color caret; // caret colour
} rui_text_input_style;

typedef struct rui_theme { // aggregate theme configuration
    rui_panel_style panel; // default panel styling
    rui_button_style button; // shared button styling
    rui_slider_style slider; // slider colours
    rui_toggle_style toggle; // toggle colours
    rui_text_input_style textInput; // text input colours
    rui_font_style textFont; // main UI font
    rui_font_style titleFont; // panel title font
} rui_theme;

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
float rui_slider_call(Rectangle bounds, float value, float minValue, float maxValue, void (*callback)(float, void *), void *userData); // slider that notifies callback
bool rui_toggle(Rectangle bounds, bool value, const char *label); // checkbox-style toggle
bool rui_toggle_call(Rectangle bounds, bool value, const char *label, void (*callback)(bool, void *), void *userData); // toggle with callback
rui_text_input rui_text_input_init(char *buffer, int capacity); // initialize text input state
bool rui_text_input_box(Rectangle bounds, rui_text_input *input); // draw text box and handle input
bool rui_keyboard_captured(void); // true when UI currently owns keyboard focus
rui_theme rui_theme_default(void); // fetch library default theme values
void rui_theme_set(const rui_theme *theme); // replace global theme with custom settings
const rui_theme *rui_theme_get(void); // get pointer to current theme
void rui_theme_reset(void); // restore theme to defaults
void rui_set_default_panel_style(rui_panel_style style); // override default panel style
rui_panel_style rui_get_default_panel_style(void); // read current default panel style

static const rui_theme RUI_THEME_DEFAULT = {
    .panel = {
        .bodyColor = {240, 240, 240, 255},
        .titleColor = {200, 200, 200, 255},
        .borderColor = {80, 80, 80, 255},
        .titleTextColor = {0, 0, 0, 255},
        .labelColor = {64, 64, 64, 255},
        .contentAlign = RUI_ALIGN_LEFT
    },
    .button = {
        .normal = {200, 200, 200, 255},
        .hover = {180, 180, 220, 255},
        .pressed = {160, 160, 200, 255},
        .border = {80, 80, 80, 255},
        .text = {0, 0, 0, 255}
    },
    .slider = {
        .track = {180, 180, 180, 255},
        .knob = {140, 140, 180, 255},
        .knobHover = {160, 160, 220, 255},
        .knobDrag = {120, 120, 200, 255}
    },
    .toggle = {
        .border = {100, 100, 100, 255},
        .borderHover = {60, 60, 60, 255},
        .fill = {230, 230, 230, 255},
        .fillActive = {120, 170, 220, 255},
        .label = {80, 80, 80, 255}
    },
    .textInput = {
        .background = {245, 245, 245, 255},
        .border = {110, 110, 140, 255},
        .borderHover = {140, 140, 160, 255},
        .borderActive = {80, 120, 200, 255},
        .text = {70, 70, 90, 255},
        .caret = {80, 80, 120, 255}
    },
    .textFont = {0},
    .titleFont = {0}
};

static rui_theme rui_themeCurrent = {
    .panel = {
        .bodyColor = {240, 240, 240, 255},
        .titleColor = {200, 200, 200, 255},
        .borderColor = {80, 80, 80, 255},
        .titleTextColor = {0, 0, 0, 255},
        .labelColor = {64, 64, 64, 255},
        .contentAlign = RUI_ALIGN_LEFT
    },
    .button = {
        .normal = {200, 200, 200, 255},
        .hover = {180, 180, 220, 255},
        .pressed = {160, 160, 200, 255},
        .border = {80, 80, 80, 255},
        .text = {0, 0, 0, 255}
    },
    .slider = {
        .track = {180, 180, 180, 255},
        .knob = {140, 140, 180, 255},
        .knobHover = {160, 160, 220, 255},
        .knobDrag = {120, 120, 200, 255}
    },
    .toggle = {
        .border = {100, 100, 100, 255},
        .borderHover = {60, 60, 60, 255},
        .fill = {230, 230, 230, 255},
        .fillActive = {120, 170, 220, 255},
        .label = {80, 80, 80, 255}
    },
    .textInput = {
        .background = {245, 245, 245, 255},
        .border = {110, 110, 140, 255},
        .borderHover = {140, 140, 160, 255},
        .borderActive = {80, 120, 200, 255},
        .text = {70, 70, 90, 255},
        .caret = {80, 80, 120, 255}
    },
    .textFont = {0},
    .titleFont = {0}
};

static rui_panel_style rui_panelStyleDefault = {
    .bodyColor = {240, 240, 240, 255},
    .titleColor = {200, 200, 200, 255},
    .borderColor = {80, 80, 80, 255},
    .titleTextColor = {0, 0, 0, 255},
    .labelColor = {64, 64, 64, 255},
    .contentAlign = RUI_ALIGN_LEFT
};

void rui_panel(Rectangle bounds, const char *title); // draw a static panel with the default style
void rui_panel_ex(Rectangle bounds, const char *title, rui_panel_style style); // draw a static panel with explicit style

// Auto-layout + scroll panels
void rui_panel_begin(Rectangle bounds, const char *title, bool scrollable); // start auto-layout panel with default style
void rui_panel_begin_ex(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style); // start panel with explicit style
void rui_panel_begin_fade(Rectangle bounds, const char *title, bool scrollable, float alpha); // start panel with fade alpha
void rui_panel_begin_ex_fade(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style, float alpha); // styled panel with fade alpha
bool rui_panel_button(const char *text, float height); // layout-aware button inside current panel
bool rui_panel_button_call(const char *text, float height, void (*callback)(void *), void *userData); // panel button with callback helper
void rui_panel_label(const char *text); // layout-aware label using panel style
void rui_panel_label_color(const char *text, Color color); // layout-aware label with explicit color
void rui_panel_spacer(float height); // advance layout cursor by a vertical gap
void rui_panel_set_content_width(float width); // set desired width for upcoming widgets (0 = full width)
float rui_panel_slider(float height, float value, float minValue, float maxValue); // slider using panel layout
float rui_panel_slider_call(float height, float value, float minValue, float maxValue, void (*callback)(float, void *), void *userData); // panel slider with callback
bool rui_panel_toggle(bool value, const char *label); // toggle integrated with panel layout
bool rui_panel_toggle_call(bool value, const char *label, void (*callback)(bool, void *), void *userData); // panel toggle with callback
bool rui_panel_text_input(float height, rui_text_input *input); // text input integrated with panel layout
bool rui_panel_begin_closable(Rectangle bounds, const char *title, bool scrollable, const char *closeLabel); // begin panel with close button, returns true when pressed
bool rui_panel_begin_ex_closable(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style, const char *closeLabel); // styled closable panel begin helper
bool rui_panel_begin_closable_fade(Rectangle bounds, const char *title, bool scrollable, float alpha, const char *closeLabel); // closable panel with fade alpha
bool rui_panel_begin_ex_closable_fade(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style, float alpha, const char *closeLabel); // styled closable panel with fade alpha
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
static bool rui_panelHasTitle = false; // indicates current panel has a title bar
static bool rui_panelCloseRequested = false; // flag indicating we want to draw a close button
static bool rui_panelClosePressed = false; // remembers if close button was clicked this frame
static const char *rui_panelCloseLabel = NULL; // optional custom label for close button
static float rui_panelHeaderHeight = 24.0f; // active panel header height

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
static rui_text_input *rui_activeTextInput = NULL; // currently focused text input box
static bool rui_keyboardCaptured = false; // true when UI takes keyboard focus
static bool rui_themeInitialized = false; // tracks if theme defaults applied
static float rui_alphaStack[8] = {1.0f}; // alpha stack for nested fades
static int rui_alphaTop = 0; // alpha stack index
static float rui_alphaCurrent = 1.0f; // current multiplied alpha
static bool rui_panelAlphaApplied = false; // true when current panel pushed alpha

rui_theme rui_theme_default(void); // forward declare helper for header calc

static float rui_calculate_header_height(bool hasTitle) {
    const rui_font_style *titleFont = &rui_themeCurrent.titleFont;
    float padding = 6.0f;
    float height = hasTitle ? ((float)titleFont->size + padding * 2.0f)
                            : ((float)rui_panelPadding + padding * 1.5f);
    if (height < 18.0f) height = 18.0f;
    return height;
}

static float rui_clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static void rui_push_alpha(float alpha) {
    alpha = rui_clamp01(alpha);
    float combined = rui_alphaCurrent * alpha;
    if (rui_alphaTop < (int)(sizeof(rui_alphaStack)/sizeof(rui_alphaStack[0])) - 1) {
        rui_alphaTop++;
        rui_alphaStack[rui_alphaTop] = combined;
    }
    rui_alphaCurrent = combined;
}

static void rui_pop_alpha(void) {
    if (rui_alphaTop > 0) {
        rui_alphaTop--;
        rui_alphaCurrent = rui_alphaStack[rui_alphaTop];
    } else {
        rui_alphaCurrent = rui_alphaStack[0];
    }
}

static Color rui_apply_alpha(Color color) {
    float a = (float)color.a * rui_alphaCurrent;
    if (a < 0.0f) a = 0.0f;
    if (a > 255.0f) a = 255.0f;
    color.a = (unsigned char)a;
    return color;
}

rui_theme rui_theme_default(void) { // expose default theme values
    rui_theme theme = RUI_THEME_DEFAULT;
    Font defaultFont = GetFontDefault();
    theme.textFont.font = defaultFont;
    theme.textFont.size = 20;
    theme.textFont.spacing = 1.0f;
    theme.titleFont = theme.textFont;
    theme.titleFont.size = 18;
    return theme;
}

static void rui_apply_font_defaults(rui_font_style *style, const rui_font_style *fallback) {
    if (style->font.texture.id == 0) {
        style->font = fallback ? fallback->font : GetFontDefault();
    }
    if (style->size <= 0) {
        style->size = fallback ? fallback->size : 20;
    }
    if (style->spacing == 0.0f) {
        style->spacing = fallback ? fallback->spacing : 1.0f;
    }
}

void rui_theme_set(const rui_theme *theme) { // replace current theme
    if (!theme) return;
    rui_themeCurrent = *theme;

    rui_apply_font_defaults(&rui_themeCurrent.textFont, NULL);
    rui_apply_font_defaults(&rui_themeCurrent.titleFont, &rui_themeCurrent.textFont);

    rui_panelStyleDefault = rui_themeCurrent.panel;
    rui_themeInitialized = true;
}

const rui_theme *rui_theme_get(void) { // access current theme pointer
    return &rui_themeCurrent;
}

void rui_theme_reset(void) { // restore default theme
    rui_theme theme = rui_theme_default();
    rui_theme_set(&theme);
    rui_alphaStack[0] = 1.0f;
    rui_alphaTop = 0;
    rui_alphaCurrent = 1.0f;
}

void rui_set_default_panel_style(rui_panel_style style) { // override default panel style independent of theme
    rui_panelStyleDefault = style;
}

rui_panel_style rui_get_default_panel_style(void) { // read current default panel style
    return rui_panelStyleDefault;
}

void rui_begin_frame(void) { // grab per-frame input state
    if (!rui_themeInitialized) {
        rui_theme_reset();
    }

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
    rui_label_color(text, pos, rui_themeCurrent.panel.labelColor); // delegate to colored label helper with theme tone
}

void rui_label_color(const char *text, Vector2 pos, Color color) { // draw text label with supplied color
    const rui_font_style *fs = &rui_themeCurrent.textFont;
    DrawTextEx(fs->font, text, (Vector2){ pos.x, pos.y }, (float)fs->size, fs->spacing, rui_apply_alpha(color)); // render text with themed font
}

bool rui_button(const char *text, Rectangle bounds) { // draw interactive button
    const rui_button_style *bs = &rui_themeCurrent.button; // fetch theme colours
    bool hovered = CheckCollisionPointRec(rui_mouse, bounds); // check if mouse over button
    bool pressed = hovered && rui_mousePressed; // register press only when hovered

    Color bg = hovered ? bs->hover : bs->normal; // choose hover background color
    if (pressed) bg = bs->pressed; // darken when actively pressed

    DrawRectangleRec(bounds, rui_apply_alpha(bg)); // fill button background
    DrawRectangleLinesEx(bounds, 2, rui_apply_alpha(bs->border)); // outline button for contrast

    const rui_font_style *fs = &rui_themeCurrent.textFont;
    Vector2 textSize = MeasureTextEx(fs->font, text, (float)fs->size, fs->spacing);
    float textX = bounds.x + (bounds.width - textSize.x) * 0.5f;
    float textY = bounds.y + (bounds.height - textSize.y) * 0.5f;
    DrawTextEx(fs->font, text, (Vector2){ textX, textY }, (float)fs->size, fs->spacing, rui_apply_alpha(bs->text)); // draw button label

    return pressed; // return true when clicked
}

bool rui_button_call(const char *text, Rectangle bounds, void (*callback)(void *), void *userData) { // draw button and fire callback
    bool pressed = rui_button(text, bounds); // reuse core button drawing logic
    if (pressed && callback) { // invoke user callback only when pressed and provided
        callback(userData); // call user-supplied function with context pointer
    }
    return pressed; // propagate pressed state to caller
}

static bool rui_draw_close_button(Rectangle bounds, const char *label, Color borderColor) { // render close button in panel chrome
    bool hovered = CheckCollisionPointRec(rui_mouse, bounds); // hover detection
    bool pressed = hovered && rui_mousePressed; // click detection

    Color base = hovered ? (Color){210, 80, 80, 255} : (Color){190, 60, 60, 255}; // background tint
    if (pressed) base = (Color){150, 40, 40, 255}; // darker when pressed

    DrawRectangleRec(bounds, rui_apply_alpha(base)); // fill button
    DrawRectangleLinesEx(bounds, 2, rui_apply_alpha(borderColor)); // outline to match panel

    const char *text = label ? label : "X"; // default label
    const rui_font_style *tf = &rui_themeCurrent.titleFont;
    float baseSize = (float)tf->size;
    if (baseSize <= 0.0f) baseSize = 18.0f;
    float drawSize = baseSize;
    if (drawSize > bounds.height - 4.0f) drawSize = bounds.height - 4.0f; // fit inside button
    if (drawSize < 8.0f) drawSize = 8.0f;
    float spacing = tf->spacing;
    if (baseSize != drawSize) {
        float scale = drawSize / baseSize;
        spacing *= scale;
    }
    Vector2 textSize = MeasureTextEx(tf->font, text, drawSize, spacing);
    Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) * 0.5f,
        bounds.y + (bounds.height - textSize.y) * 0.5f
    };
    DrawTextEx(tf->font, text, textPos, drawSize, spacing, rui_apply_alpha(WHITE)); // draw label centered

    return pressed; // signal click
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
    DrawRectangleRec(track, rui_apply_alpha(rui_themeCurrent.slider.track)); // draw track background

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

    Color knobColor = dragging ? rui_themeCurrent.slider.knobDrag
                      : (hovered ? rui_themeCurrent.slider.knobHover : rui_themeCurrent.slider.knob); // knob tint
    DrawRectangleRec(knob, rui_apply_alpha(knobColor)); // draw knob
    DrawRectangleLinesEx(knob, 2, rui_apply_alpha(rui_themeCurrent.button.border)); // outline knob using button border colour

    return clampedValue; // return potentially updated value
}

bool rui_toggle(Rectangle bounds, bool value, const char *label) { // draw checkbox and label
    float boxSize = bounds.height; // square checkbox fitting height
    if (boxSize > bounds.width) boxSize = bounds.width; // ensure fits inside bounds
    Rectangle box = { bounds.x, bounds.y, boxSize, boxSize }; // checkbox rectangle
    Rectangle textBounds = { bounds.x + boxSize + 8.0f, bounds.y, bounds.width - boxSize - 8.0f, bounds.height }; // text area

    bool hovered = CheckCollisionPointRec(rui_mouse, bounds); // hover when cursor over total bounds
    bool toggled = hovered && rui_mousePressed; // flip state only on press while hovered
    if (toggled) value = !value; // toggle value on click

    Color border = hovered ? rui_themeCurrent.toggle.borderHover : rui_themeCurrent.toggle.border; // border tint when hovered
    DrawRectangleLinesEx(box, 2, rui_apply_alpha(border)); // outline checkbox
    Color fillColor = value ? rui_themeCurrent.toggle.fillActive : rui_themeCurrent.toggle.fill; // fill based on state
    DrawRectangleRec((Rectangle){ box.x + 3, box.y + 3, box.width - 6, box.height - 6 }, rui_apply_alpha(fillColor));

    if (label) { // draw optional label text
        const rui_font_style *fs = &rui_themeCurrent.textFont;
        Vector2 textSize = MeasureTextEx(fs->font, label, (float)fs->size, fs->spacing);
        Vector2 pos = {
            textBounds.x,
            bounds.y + (bounds.height - textSize.y) * 0.5f
        };
        DrawTextEx(fs->font, label, pos, (float)fs->size, fs->spacing, rui_apply_alpha(rui_themeCurrent.toggle.label));
    }

    return value; // return possibly toggled value
}

float rui_slider_call(Rectangle bounds, float value, float minValue, float maxValue, void (*callback)(float, void *), void *userData) { // slider with callback
    float newValue = rui_slider(bounds, value, minValue, maxValue); // draw slider and get new value
    if (callback && newValue != value) { // invoke callback when value changes
        callback(newValue, userData); // pass updated value and user context
    }
    return newValue; // return current slider value
}

bool rui_toggle_call(Rectangle bounds, bool value, const char *label, void (*callback)(bool, void *), void *userData) { // toggle with callback
    bool newValue = rui_toggle(bounds, value, label); // draw toggle and get new state
    if (callback && newValue != value) { // invoke callback on state change
        callback(newValue, userData); // pass new state to user handler
    }
    return newValue; // return current toggle state
}

rui_text_input rui_text_input_init(char *buffer, int capacity) { // convenience initializer for text input state
    rui_text_input input = {0}; // zero initialize struct
    input.buffer = buffer; // assign caller buffer
    input.capacity = capacity; // set capacity
    if (buffer && capacity > 0) { // ensure buffer valid
        input.length = (int)TextLength(buffer); // pre-populate length from existing text
        if (input.length >= capacity) { // clamp length to capacity - 1
            input.length = capacity - 1;
            buffer[input.length] = '\0';
        }
        input.cursor = input.length; // place cursor at end of text
    }
    input.active = false; // start unfocused
    input.blinkTimer = 0.0f; // reset blink timer
    return input; // return initialized struct
}

static void rui_text_input_set_cursor(rui_text_input *input, int position) { // helper to clamp cursor position
    if (!input) return;
    if (position < 0) position = 0;
    if (position > input->length) position = input->length;
    input->cursor = position;
}

static void rui_text_input_insert_char(rui_text_input *input, int character) { // insert Unicode codepoint into buffer
    if (!input || !input->buffer) return;
    if (character == 0 || character == 127) return; // ignore null and delete key
    if (character < 32) return; // skip non-printable control characters
    if (input->length + 1 >= input->capacity) return; // avoid overflow

    for (int i = input->length; i >= input->cursor; --i) { // shift characters to make space
        input->buffer[i + 1] = input->buffer[i];
    }

    input->buffer[input->cursor] = (char)character; // insert character at cursor
    input->length++;
    rui_text_input_set_cursor(input, input->cursor + 1); // advance cursor
}

static void rui_text_input_backspace(rui_text_input *input) { // handle backspace deletion
    if (!input || input->cursor <= 0) return;
    for (int i = input->cursor - 1; i <= input->length; ++i) { // shift characters left
        input->buffer[i] = input->buffer[i + 1];
    }
    input->length--;
    rui_text_input_set_cursor(input, input->cursor - 1); // move cursor back
}

static void rui_text_input_delete(rui_text_input *input) { // handle delete key
    if (!input || input->cursor >= input->length) return;
    for (int i = input->cursor; i <= input->length; ++i) {
        input->buffer[i] = input->buffer[i + 1];
    }
    input->length--;
}

static void rui_text_input_set_active(rui_text_input *input) { // centralize active input management
    if (rui_activeTextInput && rui_activeTextInput != input) {
        rui_activeTextInput->active = false; // deactivate previous input
    }
    rui_activeTextInput = input; // store new active input (may be NULL)
    rui_keyboardCaptured = (input != NULL); // update capture flag
    if (input) input->active = true; // mark new input as active
}

bool rui_text_input_box(Rectangle bounds, rui_text_input *input) { // draw text box and handle typing
    if (!input || !input->buffer || input->capacity <= 1) return false;

    const rui_font_style *fs = &rui_themeCurrent.textFont;

    bool hovered = CheckCollisionPointRec(rui_mouse, bounds); // detect mouse over box
    if (rui_mousePressed && hovered) { // focus when clicked
        rui_text_input_set_active(input);
        float relativeX = rui_mouse.x - bounds.x - 4.0f; // rough cursor placement
        int caret = input->length;
        float accumulated = 0.0f;
        for (int i = 0; i < input->length; ++i) {
            Vector2 glyph = MeasureTextEx(fs->font, (const char[]){input->buffer[i], '\0'}, (float)fs->size, fs->spacing);
            float charWidth = glyph.x;
            if (accumulated + charWidth * 0.5f >= relativeX) { caret = i; break; }
            accumulated += charWidth;
        }
        rui_text_input_set_cursor(input, caret);
    } else if (rui_mousePressed && !hovered) {
        if (rui_activeTextInput == input) { // clicking elsewhere defocuses
            input->active = false;
            rui_text_input_set_active(NULL);
        }
    }

    bool changed = false;
    if (rui_activeTextInput == input) { // handle keyboard input only when active
        int key = GetKeyPressed();
        while (key > 0) { // process key presses
            if (key == KEY_BACKSPACE) {
                int prevLen = input->length;
                rui_text_input_backspace(input);
                changed |= (prevLen != input->length);
            } else if (key == KEY_DELETE) {
                int prevLen = input->length;
                rui_text_input_delete(input);
                changed |= (prevLen != input->length);
            } else if (key == KEY_LEFT) {
                rui_text_input_set_cursor(input, input->cursor - 1);
            } else if (key == KEY_RIGHT) {
                rui_text_input_set_cursor(input, input->cursor + 1);
            } else if (key == KEY_HOME) {
                rui_text_input_set_cursor(input, 0);
            } else if (key == KEY_END) {
                rui_text_input_set_cursor(input, input->length);
            } else if (key == KEY_ESCAPE || key == KEY_ENTER || key == KEY_KP_ENTER) {
                input->active = false;
                rui_text_input_set_active(NULL); // exit focus on escape/enter
            }
            key = GetKeyPressed();
        }

        int ch = GetCharPressed();
        while (ch > 0) { // process character input
            int prevLen = input->length;
            rui_text_input_insert_char(input, ch);
            changed |= (prevLen != input->length);
            ch = GetCharPressed();
        }

        input->blinkTimer += GetFrameTime(); // advance caret blink
        if (input->blinkTimer > 1.0f) input->blinkTimer -= 1.0f; // wrap timer
    } else {
        input->blinkTimer = 0.0f; // reset when not active
    }

    const rui_text_input_style *tis = &rui_themeCurrent.textInput; // theme colours
    Color borderColor = (rui_activeTextInput == input) ? tis->borderActive
                        : (hovered ? tis->borderHover : tis->border); // highlight when focused
    DrawRectangleRec(bounds, rui_apply_alpha(tis->background)); // draw background
    DrawRectangleLinesEx(bounds, 2, rui_apply_alpha(borderColor)); // draw border

    float textHeight = (float)fs->size;
    Vector2 textPos = { bounds.x + 4.0f, bounds.y + (bounds.height - textHeight) * 0.5f }; // baseline for text
    DrawTextEx(fs->font,
               input->buffer ? input->buffer : "",
               textPos,
               (float)fs->size,
               fs->spacing,
               rui_apply_alpha(tis->text)); // draw text contents

    if (rui_activeTextInput == input) { // draw caret when active
        float caretX = textPos.x;
        if (input->cursor > 0) {
            char temp = input->buffer[input->cursor];
            input->buffer[input->cursor] = '\0';
            caretX += MeasureTextEx(fs->font, input->buffer, (float)fs->size, fs->spacing).x;
            input->buffer[input->cursor] = temp;
        }

        if (fmodf(input->blinkTimer, 1.0f) < 0.5f) { // blink on for half the time
            DrawRectangle((int)caretX, (int)textPos.y, 2, fs->size, rui_apply_alpha(tis->caret));
        }
    }

    return changed; // report whether text changed this frame
}

bool rui_keyboard_captured(void) { // let callers know UI owns keyboard this frame
    return rui_keyboardCaptured;
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
    rui_panel_ex(bounds, title, rui_panelStyleDefault); // forward to full implementation with default style
}

void rui_panel_ex(Rectangle bounds, const char *title, rui_panel_style style) { // draw a static panel shell with style
    if (!rui_themeInitialized) {
        rui_theme_reset();
    }

    DrawRectangleRec(bounds, rui_apply_alpha(style.bodyColor)); // paint panel background with styled color
    DrawRectangleLinesEx(bounds, 2, rui_apply_alpha(style.borderColor)); // outline panel borders using styled color

    if (title) { // draw optional title bar when provided
        float headerHeight = rui_calculate_header_height(true);
        Rectangle titleBar = { bounds.x, bounds.y, bounds.width, headerHeight };
        DrawRectangleRec(titleBar, rui_apply_alpha(style.titleColor));
        DrawRectangleLinesEx(titleBar, 1, rui_apply_alpha(style.borderColor));

        const rui_font_style *tf = &rui_themeCurrent.titleFont;
        float paddingY = (headerHeight - (float)tf->size) * 0.5f;
        if (paddingY < 0.0f) paddingY = 0.0f;
        DrawTextEx(tf->font,
                   title,
                   (Vector2){ bounds.x + 6.0f, bounds.y + paddingY },
                   (float)tf->size,
                   tf->spacing,
                   rui_apply_alpha(style.titleTextColor));
    }
}

// --- Auto-layout + Scrollable Panels ---
static void rui_panel_begin_internal(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style, float alpha) {
    if (!rui_themeInitialized) {
        rui_theme_reset();
    }

    rui_panelAlphaApplied = false;
    float clampedAlpha = rui_clamp01(alpha);
    if (clampedAlpha != 1.0f) {
        rui_push_alpha(clampedAlpha);
        rui_panelAlphaApplied = true;
    }

    rui_currentPanel = bounds; // remember panel rectangle for children
    rui_panelHasTitle = (title != NULL); // store whether title bar exists
    rui_panelHeaderHeight = rui_calculate_header_height(rui_panelHasTitle);
    rui_panelCursorY = bounds.y + rui_panelHeaderHeight + rui_panelPadding; // initialize layout cursor beneath header
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

    float viewHeight = bounds.height - rui_panelHeaderHeight; // compute visible height excluding header
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

    if (rui_panelCloseRequested && rui_panelHasTitle) { // optionally draw close button on title bar
        float buttonSize = 18.0f;
        Rectangle closeBounds = {
            bounds.x + bounds.width - buttonSize - 6.0f,
            bounds.y + (rui_panelHeaderHeight - buttonSize) * 0.5f,
            buttonSize,
            buttonSize
        };
        rui_panelClosePressed = rui_draw_close_button(closeBounds, rui_panelCloseLabel, style.borderColor);
    } else {
        rui_panelClosePressed = false;
    }
    rui_panelCloseRequested = false; // clear request flag
    rui_panelCloseLabel = NULL; // reset custom label

    BeginScissorMode((int)bounds.x,
                     (int)(bounds.y + rui_panelHeaderHeight),
                     (int)bounds.width,
                     (int)(bounds.height - rui_panelHeaderHeight)); // clip subsequent draws to panel interior
}

void rui_panel_begin(Rectangle bounds, const char *title, bool scrollable) { // start a managed panel with default style
    rui_panel_begin_internal(bounds, title, scrollable, rui_panelStyleDefault, 1.0f);
}

void rui_panel_begin_ex(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style) { // start panel with custom style
    rui_panel_begin_internal(bounds, title, scrollable, style, 1.0f);
}

void rui_panel_begin_fade(Rectangle bounds, const char *title, bool scrollable, float alpha) { // start default panel with fade alpha
    rui_panel_begin_internal(bounds, title, scrollable, rui_panelStyleDefault, alpha);
}

void rui_panel_begin_ex_fade(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style, float alpha) { // start styled panel with fade alpha
    rui_panel_begin_internal(bounds, title, scrollable, style, alpha);
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
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + rui_panelHeaderHeight); // update content height with header baseline

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

    const rui_font_style *fs = &rui_themeCurrent.textFont;
    Vector2 textSize = MeasureTextEx(fs->font, text, (float)fs->size, fs->spacing);
    float textX = containerX; // default left alignment
    if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_CENTER) {
        float offset = (targetWidth - textSize.x) * 0.5f; // center text inside container
        if (offset < 0.0f) offset = 0.0f; // prevent negative offset when text wider than container
        textX = containerX + offset; // apply centering offset
    } else if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_RIGHT) {
        float offset = targetWidth - textSize.x; // align text against right edge
        if (offset < 0.0f) offset = 0.0f; // clamp when text wider than container
        textX = containerX + offset; // place text near right boundary
    }

    DrawTextEx(fs->font,
               text,
               (Vector2){ textX, rui_panelCursorY - rui_scrollOffset },
               (float)fs->size,
               fs->spacing,
               color);

    rui_panelCursorY += textSize.y + rui_panelSpacing; // move layout cursor past label height
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + rui_panelHeaderHeight); // refresh content height after label
}

void rui_panel_spacer(float height) { // insert vertical space inside active panel
    if (!rui_panelActive) return; // only operate inside a panel
    rui_panelCursorY += height; // advance layout cursor by requested gap
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + rui_panelHeaderHeight); // update content height after spacer
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
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + rui_panelHeaderHeight); // update content height

    return newValue; // return possibly updated value
}

float rui_panel_slider_call(float height, float value, float minValue, float maxValue, void (*callback)(float, void *), void *userData) { // panel slider helper with callback
    float newValue = rui_panel_slider(height, value, minValue, maxValue); // reuse layout slider
    if (callback && newValue != value) {
        callback(newValue, userData); // notify user when value changes
    }
    return newValue; // return slider value
}

bool rui_panel_toggle(bool value, const char *label) { // toggle integrated with panel layout
    if (!rui_panelActive) return value; // ignore when no panel active

    float height = (float)rui_themeCurrent.textFont.size + 8.0f; // default toggle height from font
    if (height < 24.0f) height = 24.0f;
    float innerWidth = rui_panelInnerRight - rui_panelInnerLeft; // usable width
    float targetWidth = rui_panelContentWidth; // requested width
    if (targetWidth <= 0.0f || targetWidth > innerWidth) targetWidth = innerWidth; // clamp

    float x = rui_panelInnerLeft; // base x position
    if (targetWidth < innerWidth) {
        if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_CENTER) {
            x = rui_panelInnerLeft + (innerWidth - targetWidth) * 0.5f;
        } else if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_RIGHT) {
            x = rui_panelInnerRight - targetWidth;
        }
    }

    Rectangle bounds = { x, rui_panelCursorY - rui_scrollOffset, targetWidth, height }; // overall toggle bounds
    bool newValue = rui_toggle(bounds, value, label); // draw toggle using base helper

    rui_panelCursorY += height + rui_panelSpacing; // advance cursor
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + rui_panelHeaderHeight); // update content height

    return newValue; // return toggle state
}

bool rui_panel_toggle_call(bool value, const char *label, void (*callback)(bool, void *), void *userData) { // panel toggle helper with callback
    bool newValue = rui_panel_toggle(value, label); // draw toggle using layout helper
    if (callback && newValue != value) {
        callback(newValue, userData); // notify about state change
    }
    return newValue; // return toggle state
}

bool rui_panel_text_input(float height, rui_text_input *input) { // integrate text input with panel layout
    if (!rui_panelActive) return false; // ignore when panel inactive

    float innerWidth = rui_panelInnerRight - rui_panelInnerLeft; // usable width
    float targetWidth = rui_panelContentWidth; // requested width
    if (targetWidth <= 0.0f || targetWidth > innerWidth) targetWidth = innerWidth; // clamp width

    float x = rui_panelInnerLeft; // base x coordinate
    if (targetWidth < innerWidth) {
        if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_CENTER) {
            x = rui_panelInnerLeft + (innerWidth - targetWidth) * 0.5f;
        } else if (rui_currentPanelStyle.contentAlign == RUI_ALIGN_RIGHT) {
            x = rui_panelInnerRight - targetWidth;
        }
    }

    Rectangle bounds = { x, rui_panelCursorY - rui_scrollOffset, targetWidth, height }; // text box bounds
    bool changed = rui_text_input_box(bounds, input); // draw input and handle typing

    rui_panelCursorY += height + rui_panelSpacing; // advance cursor after input field
    rui_contentHeight = rui_panelCursorY - (rui_currentPanel.y + rui_panelHeaderHeight); // update content height

    return changed; // return whether text changed
}

bool rui_panel_begin_closable(Rectangle bounds, const char *title, bool scrollable, const char *closeLabel) { // begin default-styled closable panel
    return rui_panel_begin_closable_fade(bounds, title, scrollable, 1.0f, closeLabel);
}

bool rui_panel_begin_ex_closable(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style, const char *closeLabel) { // styled panel with close button
    return rui_panel_begin_ex_closable_fade(bounds, title, scrollable, style, 1.0f, closeLabel);
}

bool rui_panel_begin_closable_fade(Rectangle bounds, const char *title, bool scrollable, float alpha, const char *closeLabel) {
    rui_panelCloseRequested = true;
    rui_panelCloseLabel = closeLabel;
    rui_panelClosePressed = false;
    rui_panel_begin_fade(bounds, title, scrollable, alpha);
    return rui_panelClosePressed;
}

bool rui_panel_begin_ex_closable_fade(Rectangle bounds, const char *title, bool scrollable, rui_panel_style style, float alpha, const char *closeLabel) {
    rui_panelCloseRequested = true;
    rui_panelCloseLabel = closeLabel;
    rui_panelClosePressed = false;
    rui_panel_begin_ex_fade(bounds, title, scrollable, style, alpha);
    return rui_panelClosePressed;
}

void rui_panel_end(void) { // finish panel rendering and handle scrollbars
    if (rui_panelActive) { // only proceed if a panel was begun
        EndScissorMode(); // stop clipping so scrollbar can draw outside content area

        if (rui_panelScrollable && rui_contentHeight > (rui_currentPanel.height - rui_panelHeaderHeight)) { // only show scrollbar when needed
            float viewHeight = rui_currentPanel.height - rui_panelHeaderHeight; // visible content height
            float maxOffset = rui_contentHeight - viewHeight; // maximum scroll offset possible
            if (maxOffset < 0) maxOffset = 0; // guard against negatives (precision)

            // Draw track + thumb
            float ratio = viewHeight / rui_contentHeight; // portion of content visible
            float barHeight = viewHeight * ratio; // scrollbar thumb height proportional to ratio
            float trackY = rui_currentPanel.y + rui_panelHeaderHeight; // scrollbar track start at content top
            float travel = viewHeight - barHeight; // distance thumb can travel along track

            Rectangle scrollTrack = { // rectangle representing scrollbar track
                rui_currentPanel.x + rui_currentPanel.width - 10, // align track to right edge
                trackY, // start track below panel header
                8, // fixed track width
                viewHeight // track height matches viewable content
            };

            float barY = trackY + (maxOffset > 0 ? (rui_scrollOffset / maxOffset) * travel : 0.0f); // thumb position based on scroll fraction
            Rectangle scrollBar = {scrollTrack.x, barY, scrollTrack.width, barHeight}; // rectangle for draggable thumb

            DrawRectangleRec(scrollTrack, rui_apply_alpha(LIGHTGRAY)); // draw track background
            bool hovered = CheckCollisionPointRec(rui_mouse, scrollBar); // detect hover over thumb
            Color barColor = rui_draggingScrollbar ? BLUE : (hovered ? GRAY : DARKGRAY); // change color when dragging or hovered
            DrawRectangleRec(scrollBar, rui_apply_alpha(barColor)); // draw thumb with computed color

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

        if (rui_panelAlphaApplied) {
            rui_pop_alpha();
            rui_panelAlphaApplied = false;
        }
    }
}

#endif // RUI_IMPLEMENTATION // end implementation section
#endif // RUI_H // end include guard
