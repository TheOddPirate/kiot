# KIOT Plugin: Macro & Virtual Keyboard (odd-macros)

A modular plugin for **KIOT** that creates a Home Assistant notification entity, listens for macro command sequences, and executes them as virtual keyboard or mouse inputs using `/dev/uinput`.

Derived from the standalone `odd-macros` project, this plugin allows you to trigger complex desktop automations directly from your Home Assistant dashboards and automations.

---

## ⚠️ Security Warning

This plugin uses direct `uinput` injection, which **bypasses the input security isolation of Wayland and X11**. 
- It allows global keyboard and mouse control.
- Because it listens to messages from Home Assistant / MQTT, **ensure your MQTT broker and Home Assistant instance are properly secured**.
- By default, the plugin is **disabled** and must be manually enabled in KIOT settings after reviewing compatibility.

---


## Finding Key Names & Supported Keys

If you need to know the exact name or keycode of a key to use in your macros, you can trigger a dump of all registered keys to the system logs. 

### How to print keys:
Simply send an empty message or the literal string `PRINTKEYS` to your KIOT macro notification entity from Home Assistant. 


<details>
<summary>Example output in the terminal:</summary>

```text
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] === Registered Keys in VirtualKeyboardDevice ===
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    1          -> KEY_ESC
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    2          -> KEY_1
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    3          -> KEY_2
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    4          -> KEY_3
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    5          -> KEY_4
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    6          -> KEY_5
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    7          -> KEY_6
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    8          -> KEY_7
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:    9          -> KEY_8
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   10          -> KEY_9
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   11          -> KEY_0
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   12          -> KEY_MINUS
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   13          -> KEY_EQUAL
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   14          -> KEY_BACKSPACE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   15          -> KEY_TAB
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   16          -> KEY_Q
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   17          -> KEY_W
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   18          -> KEY_E
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   19          -> KEY_R
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   20          -> KEY_T
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   21          -> KEY_Y
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   22          -> KEY_U
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   23          -> KEY_I
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   24          -> KEY_O
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   25          -> KEY_P
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   26          -> KEY_LEFTBRACE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   27          -> KEY_RIGHTBRACE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   28          -> KEY_ENTER
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   29          -> KEY_LEFTCTRL
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   30          -> KEY_A
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   31          -> KEY_S
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   32          -> KEY_D
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   33          -> KEY_F
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   34          -> KEY_G
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   35          -> KEY_H
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   36          -> KEY_J
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   37          -> KEY_K
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   38          -> KEY_L
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   39          -> KEY_SEMICOLON
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   40          -> KEY_APOSTROPHE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   41          -> KEY_GRAVE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   42          -> KEY_LEFTSHIFT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   43          -> KEY_BACKSLASH
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   44          -> KEY_Z
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   45          -> KEY_X
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   46          -> KEY_C
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   47          -> KEY_V
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   48          -> KEY_B
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   49          -> KEY_N
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   50          -> KEY_M
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   51          -> KEY_COMMA
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   52          -> KEY_DOT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   53          -> KEY_SLASH
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   54          -> KEY_RIGHTSHIFT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   86          -> KEY_102ND
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   57          -> KEY_SPACE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   58          -> KEY_CAPSLOCK
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   56          -> KEY_LEFTALT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  100          -> KEY_RIGHTALT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  125          -> KEY_LEFTMETA
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  126          -> KEY_RIGHTMETA
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   97          -> KEY_RIGHTCTRL
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  127          -> KEY_COMPOSE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   59          -> KEY_F1
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   60          -> KEY_F2
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   61          -> KEY_F3
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   62          -> KEY_F4
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   63          -> KEY_F5
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   64          -> KEY_F6
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   65          -> KEY_F7
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   66          -> KEY_F8
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   67          -> KEY_F9
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   68          -> KEY_F10
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   87          -> KEY_F11
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   88          -> KEY_F12
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  183          -> KEY_F13
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  184          -> KEY_F14
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  185          -> KEY_F15
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  186          -> KEY_F16
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  187          -> KEY_F17
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  188          -> KEY_F18
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  189          -> KEY_F19
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  190          -> KEY_F20
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   99          -> KEY_SYSRQ
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   70          -> KEY_SCROLLLOCK
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  119          -> KEY_PAUSE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  110          -> KEY_INSERT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  102          -> KEY_HOME
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  104          -> KEY_PAGEUP
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  111          -> KEY_DELETE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  107          -> KEY_END
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  109          -> KEY_PAGEDOWN
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  106          -> KEY_RIGHT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  105          -> KEY_LEFT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  108          -> KEY_DOWN
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  103          -> KEY_UP
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   69          -> KEY_NUMLOCK
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   98          -> KEY_KPSLASH
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   55          -> KEY_KPASTERISK
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   74          -> KEY_KPMINUS
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   78          -> KEY_KPPLUS
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   96          -> KEY_KPENTER
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   83          -> KEY_KPDOT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  117          -> KEY_KPEQUAL
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   82          -> KEY_KP0
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   79          -> KEY_KP1
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   80          -> KEY_KP2
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   81          -> KEY_KP3
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   75          -> KEY_KP4
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   76          -> KEY_KP5
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   77          -> KEY_KP6
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   71          -> KEY_KP7
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   72          -> KEY_KP8
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:   73          -> KEY_KP9
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  113          -> KEY_MUTE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  114          -> KEY_VOLUMEDOWN
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  115          -> KEY_VOLUMEUP
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  116          -> KEY_POWER
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  142          -> KEY_SLEEP
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  143          -> KEY_WAKEUP
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  164          -> KEY_PLAYPAUSE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  163          -> KEY_NEXTSONG
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  165          -> KEY_PREVIOUSSONG
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  166          -> KEY_STOPCD
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  140          -> KEY_CALC
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  155          -> KEY_MAIL
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  272          -> BTN_LEFT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  273          -> BTN_RIGHT
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] Keycode:  274          -> BTN_MIDDLE
[2026-09-23T11:07:20] [INFO] [kiot.Plugins.VirtualKeyboardDevice] ================================================
```
</details>

## Prerequisites & Permissions

The plugin requires write access to `/dev/uinput` to inject events. Your user account running the KIOT daemon must be part of the `input` group:

```bash
sudo usermod -aG input $USER
```

(Note: You must log out and log back in for group changes to take effect).


## Usage in Home Assistant

Once the plugin is enabled, it registers a notification entity in Home Assistant (e.g., `notify.kiot_macros` or similar, depending on your setup).

You can trigger macros by sending a message payload to this notification entity.

### Special Commands

* Send `PRINTKEYS` (or leave the message empty) to dump a full list of all supported Linux keycodes and their mappings to the KIOT system logs.

---

## Macro Sequence Syntax

When sending a macro string, separate individual steps with commas `,`.

### Supported Action Prefixes:

* `down:KEY_NAME` or `press:KEY_NAME` – Press and hold a key down (e.g., `down:KEY_LEFTCTRL`)
* `up:KEY_NAME` or `release:KEY_NAME` – Release a held key (e.g., `up:KEY_LEFTCTRL`)
* `click:KEY_NAME` – Perform a full key press and release cycle
* `delay:MS` – Pause execution for specified milliseconds (e.g., `delay:150`)
* `type:TEXT` – Type out an entire text string character by character
* `mouse_move:X;Y` – Move the mouse cursor relatively (e.g., `mouse_move:100;-50`)
* `scroll:STEPS` – Scroll the mouse wheel (positive = up, negative = down)

### Single Key Shortcut:

If you just send a key name directly without a prefix (e.g., `KEY_MUTE`), it will perform a standard click.

---

## Examples

### 1. Advanced Sequence (Window Tapping / Navigation)

```text
down:KEY_LEFTCTRL, delay:50, click:KEY_TAB, delay:200, up:KEY_LEFTCTRL

```

### 2. Typing Text with Delays

```text
type:Hello from Home Assistant!, delay:100, click:KEY_ENTER

```

### 3. Mouse Movement and Clicking

```text
mouse_move:500;200, delay:50, click:BTN_LEFT

```

---


## Related Projects

* Ported and adapted from **[odd-macros](https://github.com/TheOddPirate/odd-macros)**. 

> **Want a standalone setup?** Check out the original repo at [github.com/TheOddPirate/odd-macros](https://github.com/TheOddPirate/odd-macros), which can be run independently as a daemon and still be triggered from Home Assistant using KIOT alongside its shortcuts plugin!