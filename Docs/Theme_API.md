# Zenith Player Deluxe: Theme API

Themes are plain **Lua scripts** with a `.lua` extension. Each file returns one Lua table that describes how the player looks and moves. The player runs the script, reads that table, and applies everything automatically.

Theme files live here:

```
%APPDATA%\ZenithCollective\ZenithPlayerDeluxe\Themes
```

You can open that folder directly from the player: click **Settings** in the top bar, then **Open Themes Folder**.

---

## 1. File Structure

A theme file is a Lua script that ends with `return { ... }`. Every key inside that table is optional, so you can start small and add more later.

```lua
return {
    meta = {
        name        = "Linear Deluxe",
        author      = "yourname",
        version     = "1.0",
        description = "Raycast and Linear inspired fluid aesthetic",
    },

    font     = "Segoe UI",
    bg_image = "https://example.com/wallpaper.jpg",

    colors = {
        bg_main          = "#090a0f",
        panel_bg         = "#11131c",
        surface          = "#181b28",
        accent           = "#8b5cf6",
        accent_secondary = "#38bdf8",
        accent_glow      = "#6366f1",
        text_main        = "#f8fafc",
        text_dim         = "#79829c",
        border           = "#222638",
    },

    text = {
        window_title = "ZENITH // LINEAR DELUXE",
    },

    animations = {
        {
            name     = "title_chroma",
            target   = "title",
            property = "color",
            from     = "#8b5cf6",
            to       = "#38bdf8",
            duration = 3500,
            loop     = true,
            easing   = "in-out-quad",
        },
    },
}
```

---

## 2. meta

Theme metadata shown in the theme picker.

```lua
meta = {
    name        = "My Theme",   -- display name
    author      = "you",        -- your name or handle
    version     = "1.0",        -- any version string
    description = "...",        -- short description
},
```

All four keys are strings. None of them affect how the theme looks.

---

## 3. font

Sets the font for the whole player.

```lua
font = "Segoe UI",
```

Two ways to use this:

- **System font** — write the font family name exactly as installed: `"Segoe UI"`, `"Consolas"`, `"Arial"`.
- **Custom font file** — drop a `.ttf` or `.otf` file into the Themes folder, then write the filename: `"MyFont.ttf"`. The player registers and loads it automatically.

---

## 4. bg_image

Sets a background wallpaper behind the main stage.

```lua
bg_image = "https://example.com/wallpaper.jpg",
-- or a local absolute path:
bg_image = "C:/Users/you/Pictures/bg.jpg",
```

The player validates and caches remote images. Local files are checked before loading. Unsupported or invalid files are silently ignored.

---

## 5. colors

A table of color keys. Every value is a hex color string (`"#rrggbb"`).

```lua
colors = {
    bg_main          = "#08090d",
    panel_bg         = "#10121a",
    surface          = "#181a26",
    accent           = "#8b5cf6",
    accent_secondary = "#38bdf8",
    accent_glow      = "#3b2d66",
    text_main        = "#f8fafc",
    text_dim         = "#717b94",
    border           = "#1e2235",
},
```

| Key | What it paints |
| :--- | :--- |
| `bg_main` | Main window background |
| `panel_bg` | Sidebar, cards and floating panels |
| `surface` | Inputs, buttons, list rows and sliders |
| `accent` | Active items, slider handles, highlights |
| `accent_secondary` | Gradients, spectrum bars, secondary fills |
| `accent_glow` | Ambient glow behind artwork and blobs |
| `text_main` | Titles, track names, primary labels |
| `text_dim` | Timestamps, path text, subtle hints |
| `border` | Dividers and outlines |

You can omit any key and the player falls back to the built-in default for that slot.

---

## 6. text

Overrides the text labels shown in the UI.

```lua
text = {
    window_title = "ZENITH // MY THEME",
},
```

| Key | What it changes |
| :--- | :--- |
| `window_title` | The app title in the top bar |

---

## 7. animations

A Lua array of animation tables. Each entry targets one UI element and one property, and plays when a trigger condition fires.

```lua
animations = {
    {
        name     = "play_pop",
        target   = "play_button",
        property = "scale",
        from     = 1.0,
        to       = 1.15,
        duration = 180,
        trigger  = "hover",
        easing   = "spring",
    },
    -- more animations...
},
```

### 7.1 Animation Keys

| Key | Type | Default | What it does |
| :--- | :--- | :--- | :--- |
| `name` | string | `"anim_N"` | Identifier for this animation. Not shown in the UI. |
| `target` | string | required | The UI element to animate. See section 8 for all names. |
| `property` | string | required | Which property changes. See section 7.2. |
| `from` | number or string | required | Starting value. Use a number for numeric properties, a `"#rrggbb"` string for colors. |
| `to` | number or string | required | Ending value. Same format as `from`. |
| `duration` | number | `1000` | How long one pass takes, in milliseconds. |
| `loop` | boolean | `false` | `true` makes the animation repeat forever. |
| `direction` | string | `"alternate"` | `"alternate"` ping-pongs between `from` and `to`. `"forward"` restarts from `from` each time. |
| `trigger` | string | `"none"` | What starts the animation. See section 7.3. |
| `easing` | string | `"spring"` | Movement curve. See section 7.4. |
| `delay` | number | `0` | Wait this many milliseconds before the animation starts. |

### 7.2 Animatable Properties

| Property | Value format | Notes |
| :--- | :--- | :--- |
| `color` | `"#rrggbb"` | Smoothly blends between two colors. |
| `border.color` | `"#rrggbb"` | Border or outline color. |
| `scale` | number | `1.0` is normal size. `1.2` is 20% bigger. Scales from center. |
| `rotation` | number | Degrees, 0 to 360. Great for spinning icons and vinyl records. |
| `opacity` | number | `0.0` is invisible. `1.0` is fully visible. |
| `y` | number | Vertical offset in pixels. Negative moves up, positive moves down. |
| `x` | number | Horizontal offset in pixels. |
| `letterSpacing` | number | Gap between letters in a text element. |
| `radius` | number | Corner rounding in pixels. |

### 7.3 Triggers

| Value | When the animation plays |
| :--- | :--- |
| `"none"` or `"always"` | Runs immediately if `loop = true`. Does nothing if not looping. |
| `"hover"` | Plays forward when the mouse enters the element, reverses when it leaves. |
| `"playing"` | Runs while audio is playing. Reverses to resting state when paused or stopped. |

### 7.4 Easing Curves

| Value | Feel |
| :--- | :--- |
| `"spring"` | Snappy bounce with a small overshoot. The Linear and Raycast feel. |
| `"bounce"` | Rubber ball landing. Big bounces at the end. |
| `"out-back"` | Overshoots the target slightly, then settles. |
| `"out-cubic"` | Fast start, soft landing. |
| `"in-out-quad"` | Gentle acceleration in and out. Good for color loops. |
| `"linear"` | Constant speed. Perfect for spinning records. |
| `"elastic"` | Rubber band snap. Big initial overshoot. |

---

## 8. UI Elements (target names)

These are all the strings you can use in `target`.

### Top Bar

| Name | What it is |
| :--- | :--- |
| `title` | The main app title text. Supports `color`, `scale`, `opacity`, `y`, `letterSpacing`. |
| `title_bar` | The entire top header strip. |
| `search_bar` | The search input capsule container. |
| `search_input` | The text field inside the search bar. |
| `rpc_btn` | The Discord Rich Presence button. |
| `settings_btn` | The settings gear button. Looks great with `rotation`. |
| `btn_min` | The minimize window button. |
| `btn_max` | The maximize window button. |
| `btn_close` | The close window button. |

### Sidebar and Library

| Name | What it is |
| :--- | :--- |
| `sidebar` | The left navigation container. |
| `sidebar_header` | The "LIBRARY" heading inside the sidebar. |
| `btn_add_url` | The URL stream add button. |
| `btn_add_folder` | The local folder add button. |
| `folder_list` | The folder and track list area. |
| `splitter` | The vertical drag handle between sidebar and main stage. |

### Main Stage

| Name | What it is |
| :--- | :--- |
| `main_stage` | The entire center content area. |
| `background_image` | The custom wallpaper behind everything. |
| `spectrum` | The audio spectrum bars in the background. |
| `card_cover` | The card holding artwork and track metadata. |
| `cover_glow` | The ambient glow blob behind the artwork. |
| `cover` | The artwork image box. Use `rotation` to spin it like a vinyl record. |
| `track_title` | The current track title text. |
| `track_artist` | The current artist name text. |
| `track_path` | The file path or stream URL text. |

### Equalizer

| Name | What it is |
| :--- | :--- |
| `card_eq` | The whole equalizer card. |
| `eq_header` | The equalizer section title. |
| `eq_reset_btn` | The "RESET EQ" button. |
| `eq_slider` | All 7 EQ sliders at once. |
| `eq_slider_0` | 60 Hz — sub bass. |
| `eq_slider_1` | 170 Hz — bass. |
| `eq_slider_2` | 350 Hz — low mid. |
| `eq_slider_3` | 1 kHz — mid. |
| `eq_slider_4` | 3.5 kHz — high mid. |
| `eq_slider_5` | 10 kHz — treble. |
| `eq_slider_6` | 14 kHz — air / high treble. |

### Bottom Player Bar

| Name | What it is |
| :--- | :--- |
| `bottom_bar` | The entire lower player bar. |
| `bottom_cover` | The mini artwork thumbnail. |
| `bottom_title` | Track title in the bottom bar. |
| `bottom_artist` | Artist name in the bottom bar. |
| `play_button` | The main Play / Pause circle button. |
| `btn_prev` | The previous track button. |
| `btn_next` | The next track button. |
| `time_pos` | Current playback position text. |
| `time_dur` | Total track duration text. |
| `progress_slider` | The timeline scrub bar. |
| `waveform` | The waveform amplitude display. |
| `progress_handle` | The draggable position marker on the timeline. |
| `vol_label` | The volume icon and label. |
| `vol_slider` | The volume slider bar. |
| `vol_input` | The volume percentage text box. |
| `spd_label` | The speed icon and label. |
| `spd_slider` | The speed slider bar. |
| `spd_input` | The speed percentage text box. |

### Modals

| Name | What it is |
| :--- | :--- |
| `url_modal` | The web stream URL input dialog. |
| `rpc_modal` | The Discord Client ID input dialog. |

---

## 9. Full Example Themes

### Minimal Starter

```lua
return {
    meta = {
        name    = "Minimal",
        author  = "you",
        version = "1.0",
    },
    font   = "Segoe UI",
    colors = {
        bg_main   = "#0d0d0d",
        panel_bg  = "#141414",
        surface   = "#1c1c1c",
        accent    = "#ffffff",
        text_main = "#ffffff",
        text_dim  = "#666666",
        border    = "#2a2a2a",
    },
    animations = {
        {
            name     = "play_pop",
            target   = "play_button",
            property = "scale",
            from     = 1.0,
            to       = 1.12,
            duration = 160,
            trigger  = "hover",
            easing   = "spring",
        },
    },
}
```

### Hyprland / Niri Spring Theme

Fluid spring animations on every interactive element.

```lua
return {
    meta = {
        name        = "Hyprland Niri",
        author      = "Zenith Deluxe",
        version     = "3.0",
        description = "Spring curves on everything",
    },
    font   = "Consolas",
    colors = {
        bg_main          = "#06070a",
        panel_bg         = "#0c0e14",
        surface          = "#141722",
        accent           = "#00f0ff",
        accent_secondary = "#ff007f",
        accent_glow      = "#00f0ff",
        text_main        = "#f0f6fc",
        text_dim         = "#768390",
        border           = "#23293e",
    },
    text = {
        window_title = "ZENITH // HYPRLAND [NIRI-WS-01]",
    },
    animations = {
        {
            name     = "title_chroma",
            target   = "title",
            property = "color",
            from     = "#00f0ff",
            to       = "#ff007f",
            duration = 3000,
            loop     = true,
            easing   = "in-out-quad",
        },
        {
            name     = "title_expand",
            target   = "title",
            property = "letterSpacing",
            from     = 2.0,
            to       = 5.0,
            duration = 220,
            trigger  = "hover",
            easing   = "spring",
        },
        {
            name     = "play_pop",
            target   = "play_button",
            property = "scale",
            from     = 1.0,
            to       = 1.20,
            duration = 160,
            trigger  = "hover",
            easing   = "spring",
        },
        {
            name     = "prev_pop",
            target   = "btn_prev",
            property = "scale",
            from     = 1.0,
            to       = 1.25,
            duration = 150,
            trigger  = "hover",
            easing   = "spring",
        },
        {
            name     = "next_pop",
            target   = "btn_next",
            property = "scale",
            from     = 1.0,
            to       = 1.25,
            duration = 150,
            trigger  = "hover",
            easing   = "spring",
        },
        {
            name     = "gear_spin",
            target   = "settings_btn",
            property = "rotation",
            from     = 0,
            to       = 90,
            duration = 220,
            trigger  = "hover",
            easing   = "spring",
        },
        {
            name      = "vinyl_spin",
            target    = "cover",
            property  = "rotation",
            from      = 0,
            to        = 360,
            duration  = 4500,
            loop      = true,
            direction = "forward",
            trigger   = "playing",
            easing    = "linear",
        },
        {
            name     = "cover_card_lift",
            target   = "card_cover",
            property = "y",
            from     = 0,
            to       = -6,
            duration = 200,
            trigger  = "hover",
            easing   = "spring",
        },
    },
}
```

### Animation Recipes

**Infinite color loop:**
```lua
{
    name     = "title_color_shift",
    target   = "title",
    property = "color",
    from     = "#8b5cf6",
    to       = "#06b6d4",
    duration = 3200,
    loop     = true,
    easing   = "in-out-quad",
},
```

**Pulsing opacity while playing:**
```lua
{
    name     = "wave_glow",
    target   = "waveform",
    property = "opacity",
    from     = 0.55,
    to       = 1.0,
    duration = 750,
    loop     = true,
    trigger  = "playing",
    easing   = "in-out-quad",
},
```

**Bouncing EQ slider while playing:**
```lua
{
    name     = "eq_bounce_bass",
    target   = "eq_slider_0",
    property = "y",
    from     = 0,
    to       = -10,
    duration = 650,
    loop     = true,
    trigger  = "playing",
    easing   = "out-bounce",
},
```

**Delayed stagger:**
```lua
{
    name     = "sidebar_fade",
    target   = "sidebar",
    property = "opacity",
    from     = 0.6,
    to       = 1.0,
    duration = 400,
    trigger  = "hover",
    delay    = 80,
    easing   = "out-cubic",
},
```

---

## 1. Theme File Structure

A theme file contains metadata headers, font and background directives, color definitions and animation blocks.

```css
#^ Author: Zenith Deluxe
#^ ThemeName: Linear Deluxe
#^ Version: 2.0
#^ Description: Raycast and Linear inspired fluid aesthetic with spring animations

@font "Segoe UI"
@bg "https://example.com/wallpaper.jpg"

colors {
    bg_main: #090a0f;
    panel_bg: #11131c;
    surface: #181b28;
    accent: #8b5cf6;
    accent_secondary: #38bdf8;
    accent_glow: #6366f1;
    text_main: #f8fafc;
    text_dim: #79829c;
    border: #222638;
}

text {
    window_title: "ZENITH // LINEAR DELUXE";
}

animation title_chroma {
    target: "title";
    property: "color";
    from: "#8b5cf6";
    to: "#38bdf8";
    duration: 3500;
    loop: infinite;
    easing: in-out-quad;
}
```

---

## 2. Fonts (@font)

Set your font at the top of the file:

```css
@font "Segoe UI"
```

Font rules:
- Use installed system fonts like `"Segoe UI"`, `"Consolas"` or `"Arial"`.
- Put `.ttf` or `.otf` files straight into the `Themes` folder and write their filename: `@font "MyFont.ttf"`. The app registers and loads them automatically.
- Both `@font "Name"`, `@font: Name;` and `font-family: "Name";` work.

---

## 3. Color Palette (colors)

Write colors in standard hex format (`#rrggbb`):

| Key | Description |
| :--- | :--- |
| `bg_main` | Main window background color |
| `panel_bg` | Background for panels, sidebar and cards |
| `surface` | Background for inputs, buttons and lists |
| `accent` | Primary accent color for sliders, active items and highlights |
| `accent_secondary` | Secondary accent color for gradients and spectrum bars |
| `accent_glow` | Color for background ambient glows |
| `text_main` | Primary text color for titles, track names and labels |
| `text_dim` | Secondary text color for timestamps and subtle hints |
| `border` | Color for outline borders and dividers |

---

## 4. Animation Engine (animation)

Write each animation in its own block:

```css
animation play_button_pop {
    target: "play_button";
    property: "scale";
    from: 1.0;
    to: 1.15;
    duration: 180;
    trigger: hover;
    easing: spring;
}
```

### Animation Parameters

* `target`: the name of the UI element to animate (see section 5 for the full list).
* `property`: the property that changes over time:
  * `color`: background or text color (`#rrggbb`).
  * `border.color`: border color.
  * `scale`: size multiplier (1.0 is default size, 1.15 is 15% bigger). Elements scale from their exact center.
  * `rotation`: angle in degrees (0 to 360).
  * `opacity`: transparency from 0.0 (hidden) to 1.0 (fully visible).
  * `y`: vertical offset in pixels (negative numbers move up, positive move down).
  * `x`: horizontal offset in pixels.
  * `letterSpacing`: spacing between text letters (for titles and labels).
  * `radius`: corner rounding in pixels.
* `from`: starting value (number or `#rrggbb` color).
* `to`: ending value (number or `#rrggbb` color).
* `duration`: animation time in milliseconds (1000 = 1 second).
* `loop`: accepts `infinite` (runs forever) or `false` (runs once).
* `direction`: movement direction:
  * `alternate`: ping-pong back and forth (from -> to -> from). Good for breathing, pulsing and color shifts.
  * `forward`: runs from `from` to `to` and restarts from `from`. Good for 360 degree vinyl spins.
* `trigger`: what starts the animation:
  * `none` (or `always`): runs continuously if `loop: infinite` is set.
  * `hover`: triggers when the mouse enters the element. The element moves to `to` with snappy spring physics. When the mouse leaves, it returns to `from` with a smooth 160ms curve.
  * `playing`: runs while audio plays. When paused, it returns to its resting state.
* `easing`: movement curve:
  * `spring`: snappy bouncy spring feel with slight overshoot (Linear and Raycast feel).
  * `bounce`: realistic rubber ball bounce.
  * `out-back`: travels slightly past the target and settles back.
  * `out-cubic`: fast takeoff with soft deceleration.
  * `in-out-quad`: smooth acceleration and deceleration.
  * `linear`: constant steady speed from start to finish (great for spinning records).
  * `elastic`: rubber band physics.

---

## 5. UI Elements List (target)

You can target and animate all of these elements:

### Top Bar
* `title`: the main app title text ("ZENITH PLAYER"). Supports `color`, `scale`, `opacity`, `y` and `letterSpacing`.
* `title_bar`: the entire top header strip.
* `search_bar`: the search input container capsule.
* `search_input`: the search text input field.
* `rpc_btn`: the Discord Rich Presence button.
* `settings_btn`: the settings button (animates nicely with `rotation`).
* `btn_min`: the minimize window button.
* `btn_max`: the maximize window button.
* `btn_close`: the close window button.

### Sidebar and Library
* `sidebar`: the left navigation container.
* `sidebar_header`: the "LIBRARY" title header.
* `btn_add_url`: the URL stream button.
* `btn_add_folder`: the local folder button.
* `folder_list`: the folder and track list area.
* `splitter`: the vertical divider line between sidebar and main stage.

### Main Stage
* `main_stage`: the main center area.
* `background_image`: the custom background wallpaper.
* `spectrum`: the animated audio spectrum bars in the background.
* `card_cover`: the large card holding artwork and metadata.
* `cover_glow`: the glowing ambient backdrop behind the cover.
* `cover`: the artwork box (rotates like a vinyl record with `rotation`).
* `track_title`: current track title text.
* `track_artist`: current artist name text.
* `track_path`: file path or web stream source text.

### Equalizer
* `card_eq`: the equalizer card container.
* `eq_header`: the equalizer header title.
* `eq_reset_btn`: the reset button ("RESET EQ").
* `eq_slider`: controls all 7 equalizer sliders together.
* `eq_slider_0`: 60 Hz slider (sub bass).
* `eq_slider_1`: 170 Hz slider (bass).
* `eq_slider_2`: 350 Hz slider (low mid).
* `eq_slider_3`: 1 kHz slider (mid).
* `eq_slider_4`: 3.5 kHz slider (high mid).
* `eq_slider_5`: 10 kHz slider (treble).
* `eq_slider_6`: 14 kHz slider (air / high treble).

### Bottom Player Bar
* `bottom_bar`: the entire lower player bar.
* `bottom_cover`: the mini artwork thumbnail.
* `bottom_title`: the track title in the bottom bar.
* `bottom_artist`: the artist name in the bottom bar.
* `play_button`: the main Play / Pause circle button.
* `btn_prev`: the previous track button.
* `btn_next`: the next track button.
* `time_pos`: current playback position text.
* `time_dur`: total duration text.
* `progress_slider`: the timeline track bar.
* `waveform`: the audio waveform amplitude peaks canvas.
* `progress_handle`: the draggable playback position handle.
* `vol_label`: the volume icon and label.
* `vol_slider`: the volume slider bar.
* `vol_input`: the volume percentage input box.
* `spd_label`: the speed icon and label.
* `spd_slider`: the speed slider bar.
* `spd_input`: the speed percentage input box.

### Modals
* `url_modal`: the web stream input modal dialog.
* `rpc_modal`: the Discord Client ID modal dialog.

---

## 6. Ready-to-Use Animation Recipes

### Infinite Shimmering Title Text
```css
animation title_color_shift {
    target: "title";
    property: "color";
    from: "#8b5cf6";
    to: "#06b6d4";
    duration: 3200;
    loop: infinite;
    easing: in-out-quad;
}
```

### Expanding Title Kerning on Hover
```css
animation title_expand {
    target: "title";
    property: "letterSpacing";
    from: 2.0;
    to: 4.5;
    duration: 250;
    trigger: hover;
    easing: out-back;
}
```

### Spinning Vinyl Artwork During Playback
```css
animation vinyl_spin {
    target: "cover";
    property: "rotation";
    from: 0;
    to: 360;
    duration: 6000;
    loop: infinite;
    direction: forward;
    trigger: playing;
    easing: linear;
}
```

### Bouncy Spring Play Button on Hover
```css
animation play_pop {
    target: "play_button";
    property: "scale";
    from: 1.0;
    to: 1.15;
    duration: 180;
    trigger: hover;
    easing: spring;
}
```

### Settings Gear Rotation on Hover
```css
animation gear_spin {
    target: "settings_btn";
    property: "rotation";
    from: 0;
    to: 45;
    duration: 220;
    trigger: hover;
    easing: out-cubic;
}
```

### Pulsing Waveform While Audio Plays
```css
animation wave_glow {
    target: "waveform";
    property: "opacity";
    from: 0.55;
    to: 1.0;
    duration: 750;
    loop: infinite;
    trigger: playing;
    easing: in-out-quad;
}
```

### Bouncing Equalizer Sliders in Rhythm
```css
animation eq_bounce_bass {
    target: "eq_slider_0";
    property: "y";
    from: 0;
    to: -10;
    duration: 650;
    loop: infinite;
    trigger: playing;
    easing: out-bounce;
}

animation eq_bounce_high {
    target: "eq_slider_6";
    property: "y";
    from: 0;
    to: -8;
    duration: 550;
    loop: infinite;
    trigger: playing;
    easing: out-bounce;
}
```
