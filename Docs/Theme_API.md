# Zenith Player Deluxe: Theme and Animation API

Themes use the `.zss` extension (Zenith Style Sheet). All theme files live in this folder:
`%APPDATA%\ZenithCollective\ZenithPlayerDeluxe\Themes`

You can open this folder directly from the player: click Settings on the top bar and click "Open Themes Folder".

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
