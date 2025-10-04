# Raylib UI Essentials (`rui.h`)

`rui.h` is a single-header UI micro-library for [raylib](https://www.raylib.com/) that covers the basics you need for quick prototypes: panels (with optional scrolling), labels, buttons, sliders, toggles, text inputs, and fade overlays. Everything lives in one file that you can drop into a jam project, tweak, and ship.

Note there is a branch `with_fonts` that includes custom font support and font styling. But I have not tested it, so it is experimental. Use at your own joy and risk! :)

## Installation

1. Copy `src/rui.h` into your project (keeping the `src/` folder is optional).
2. In **one** C file, define the implementation before including the header:

   ```c
   #define RUI_IMPLEMENTATION
   #include "rui.h"
   ```

   In every other translation unit, just `#include "rui.h"`.

3. Link against raylib as usual. The included `Makefile` shows a macOS setup:

   ```bash
   make        # builds ./rui_demo
   ./rui_demo  # run the sample
   ```

   Adjust include/library paths for your platform if needed.

## Frame Flow

Call `rui_begin_frame()` once per frame **after** you clear input but **before** drawing UI. All widget helpers use the mouse state cached here.

```c
BeginDrawing();
    ClearBackground(RAYWHITE);

    rui_begin_frame();

    // UI continues...
EndDrawing();
```

## Panels

```c
// Static panel shell
rui_panel((Rectangle){20, 20, 200, 150}, "Title");

// Auto-layout panel
rui_panel_begin((Rectangle){20, 20, 220, 300}, "Menu", true); // true => scrollable
    rui_panel_label("Inventory");
    if (rui_panel_button("Use", 28)) { /* ... */ }
rui_panel_end();

// Closable variant (returns true when the close button is pressed)
bool closed = rui_panel_begin_closable(bounds, "Info", false, "X");
if (!closed) {
    // contents
}
rui_panel_end();
```

Auto-layout panels expose helpers:

| Helper | Notes |
| --- | --- |
| `rui_panel_button` / `_call` | Buttons that respect padding, spacing, and optionally invoke callbacks. |
| `rui_panel_label` / `_color` | Labels aligned using the panel style. |
| `rui_panel_slider` / `_call` | Horizontal slider stacked in the panel. |
| `rui_panel_toggle` / `_call` | Checkbox-style toggle. |
| `rui_panel_text_input` | Single-line text box (see Keyboard Capture below). |
| `rui_panel_spacer` | Adds vertical gap. |
| `rui_panel_set_content_width` | Temporarily narrow widgets (e.g., to center a smaller control). |

## Buttons & Callbacks

Every button helper returns `true` when clicked. The `_call` variants also invoke a callback:

```c
static void on_start(void *ctx) {
    puts("Start pressed");
}

if (rui_panel_button_call("Start", 30, on_start, NULL)) {
    /* optional extra handling */
}
```

You can pass per-button context by pointing `userData` at your game state (struct, index etc.).

## Sliders & Toggles

```c
float value = 0.5f;
bool enabled = true;

value = rui_panel_slider(24, value, 0.0f, 1.0f);
enabled = rui_panel_toggle(enabled, "Enable music");

// With callbacks (fires only when value changes)
value = rui_panel_slider_call(24, value, 0.0f, 1.0f, on_volume, NULL);
enabled = rui_panel_toggle_call(enabled, "Music", on_music_toggle, NULL);
```

Sliders support dragging or clicking the track; toggles focus the entire row, so clicking the label also flips the state.

## Text Input & Keyboard Capture

```c
char name[32] = "Player";
rui_text_input nameInput = rui_text_input_init(name, sizeof(name));

if (rui_panel_text_input(26, &nameInput)) {
    printf("Name changed: %s\n", name);
}

if (!rui_keyboard_captured()) {
    // Safe to run game-wide hotkeys while UI is not typing.
}
```

`rui_keyboard_captured()` returns `true` while any text input has focus, letting you pause gameplay hotkeys (arrow keys, fade toggles, etc.). Pressing `Esc` or `Enter` exits the field.

## Fade Utility

```c
rui_fade_set_color((Color){0, 0, 0, 255});
rui_fade_out(0.5f); // fade to black over half a second
rui_draw_fade();    // call after UI but before EndDrawing

if (!rui_fade_active()) {
    // fade finished
}
```

`rui_fade_out` and `rui_fade_in` can be triggered back-to-back; the system interpolates from the current alpha.

## Theming

All colours are pulled from a global `rui_themeCurrent`. Fetch, tweak, and set:

```c
rui_theme theme = rui_theme_default();

theme.panel.bodyColor = (Color){30, 30, 40, 230};
theme.button.normal   = (Color){ 80, 90,110,255};
theme.toggle.fillActive = GOLD;

rui_theme_set(&theme);
```

At runtime you can:

- `rui_theme_set(customTheme)` – replace the entire theme (panels, buttons, sliders, toggles, text inputs).
- `rui_theme_reset()` – restore built-in defaults.
- `rui_theme_get()` – inspect the currently active theme.
- `rui_set_default_panel_style(style)` / `rui_get_default_panel_style()` – override just the panel style (alignment, colours) while leaving other widget palettes alone.

Remember to update your panel begin calls if you rely on the default style:

```c
// picks up rui_panelStyleDefault
rui_panel_begin(bounds, "Settings", true);

// custom style per-panel
rui_panel_style tinted = theme.panel;
tinted.bodyColor = Fade(BLUE, 0.5f);
rui_panel_begin_ex(bounds, "Settings", true, tinted);
```

## Scroll Panels & Close Buttons

- Passing `true` to `rui_panel_begin` enables wheel/drag scrolling automatically when content exceeds the viewport.
- For modal panes, use `rui_panel_begin_closable` (or `_ex_closable` for custom styles). The function returns `true` on the frame the close button is pressed—hide or destroy the panel in response.

## Example Structure

The demo (`src/main.c`) includes:

- Closable info panel using a custom style
- Scrollable panel with label, slider, toggle, buttons, and text input
- Callbacks that log interactions
- Fade hotkeys showing how to avoid conflicts with focused inputs

Use it as a reference or prune it down for your own project.

## Tips

- All coordinates are in screen pixels; `rui_panel_set_content_width` helps center narrow controls.
- Widgets return values immediately—store them back into your state (like `musicVolume`) each frame.
- The header is plain C (C99); using it from C++ simply requires the usual `extern "C"` wrapper if you include it from C++ translation units.
- The implementation keeps global state; it’s intentionally simple. If you need multi-context support you can wrap the globals into a struct and fork the header.

## Contributing / Extending

`rui.h` is intentionally straightforward. If you want to extend it:

- Add new widget helpers near the existing ones so they share the layout state.
- Update the theme struct when adding colour-dependent widgets.
- Keep the top-of-file API declarations in sync with the `#ifdef RUI_IMPLEMENTATION` block.

Pull requests and issues are welcome once the repo is public. For now feel free to fork and tailor the header to your game jam or teaching needs.
