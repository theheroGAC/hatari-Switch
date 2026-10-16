# Hatari Switch

[![Release](https://img.shields.io/badge/Release-v1.00-blue.svg)](https://github.com/theheroGAC/hatari-Switch/releases)
[![Platform](https://img.shields.io/badge/Platform-Nintendo%20Switch-red.svg)](https://www.nintendo.com/)
[![License](https://img.shields.io/badge/License-GPL%20v2.0-green.svg)](gpl.txt)

**Hatari Switch** is an enhanced port of the [Hatari](https://hatari.tuxfamily.org/) Atari ST/STE/TT/Falcon emulator for the **Nintendo Switch**, built with devkitA64, libnx, and hardware-accelerated SDL2.

- **GitHub Repository**: [https://github.com/theheroGAC/hatari-Switch](https://github.com/theheroGAC/hatari-Switch)

---

## Features

- **Full Atari ST/STE/TT/Falcon Emulation**: Powered by the robust Hatari emulation core.
- **Hardware-Accelerated 720p Display**: Crisp, integer-scaled rendering preserving proper Atari aspect ratio.
- **Dual Analog Mouse & Touchscreen**:
  - Right analog stick controls the ST mouse smoothly at all times.
  - Nintendo Switch capacitive touchscreen support: tap and drag the ST mouse pointer natively!
  - `ZL` and `ZR` triggers for Left and Right mouse clicks.
- **On-Screen Virtual Keyboard (UAE4All2 HD style)**:
  - Accessible in-game by pressing **Minus (-)**, **L + R**, or clicking **Left/Right Stick**.
  - Features real Atari ST keys: `ESC`, `F1-F10`, `HELP`, `UNDO`, `Tab`, `Ctrl`, `Alt`, `Caps`, `Return`, `Backspace`, `Delete`, and arrow keys.
  - Sticky modifiers (`Shift`, `Ctrl`, `Alt`) with visual green indicators.
  - Full touchscreen typing support: tap keys directly on screen!
  - Adjust position (Right Stick Up/Down) and transparency (Right Stick Left/Right).
- **Clean Menu Navigation**:
  - Dedicated controller handling for Hatari's F12 menu with debounced button transitions (Button A = Confirm/Select, Button B = Cancel/Exit).
  - Quick Virtual Keyboard toggle available directly from the Keyboard Setup menu.
- **Clean Shutdown**: Dedicated clean exit sequence preventing crashes when closing the emulator.

---

## Nintendo Switch Controls Layout

### 🎮 Standard Switch Button Mapping

- **Left Analog Stick**:
  - *Default*: Atari ST Joystick directions (Up / Down / Left / Right)
  - *With ZL Held*: Controls the Atari ST Mouse pointer
- **Right Analog Stick**:
  - Controls the Atari ST Mouse pointer (smooth analog mouse, always active)
- **Left Stick Click (L3)**:
  - Toggles the on-screen **Virtual Keyboard (VKBD)**
- **Right Stick Click (R3)**:
  - Toggles the on-screen **Virtual Keyboard (VKBD)**
- **D-Pad (Freccette)**:
  - Atari ST Joystick directions (Up / Down / Left / Right)
- **A Button**:
  - *Default*: Atari ST Joystick Fire button
  - *With ZL Held*: Atari ST Mouse Left Click
  - *In Menus*: Confirm / Select focused item
- **B Button**:
  - *Default*: Opens Hatari Options Menu (F12)
  - *With ZL Held*: Atari ST Mouse Right Click
  - *In Menus*: Cancel / Back / Exit dialog
- **X Button**:
  - Atari ST `Space` bar
- **Y Button**:
  - Atari ST `Return` / `Enter` key
- **L Button (Shoulder)**:
  - Atari ST `Escape` (ESC) key
- **R Button (Shoulder)**:
  - Atari ST `Tab` key
- **ZL Button (Trigger)**:
  - Atari ST Mouse Left Click (also activates Mouse Mode for Left Stick and buttons)
- **ZR Button (Trigger)**:
  - Atari ST Mouse Right Click
- **Minus (-) Button**:
  - **Toggles the on-screen Virtual Keyboard (VKBD)**
- **Plus (+) Button**:
  - Quits Hatari Switch cleanly
- **L + R (Pressed together)**:
  - Quick shortcut to toggle the Virtual Keyboard (VKBD)
- **Touchscreen**:
  - Direct capacitive mouse control: move, tap to click, and drag
  - Direct touch on the Virtual Keyboard to type keys immediately

---

### ⌨️ Virtual Keyboard (VKBD) Controls

When the virtual keyboard is displayed on screen:

| Switch Button | Action |
| --- | --- |
| **D-Pad / Left Stick** | Move selection cursor across keyboard keys |
| **A Button** | Type the selected key |
| **X Button** | Toggle `Shift` mode |
| **Y Button** | Quick `Backspace` (delete previous character) |
| **B Button** or **Minus (-)** | Close the Virtual Keyboard |
| **Right Stick Up / Down** | Displace keyboard position vertically |
| **Right Stick Left / Right** | Adjust keyboard transparency (alpha opacity) |
| **Touchscreen** | Tap any key directly on screen to type it instantly |

---

### 📋 Hatari Menu (F12) Controls

| Switch Button | Action |
| --- | --- |
| **D-Pad / Left Stick** | Navigate menu items and options |
| **A Button** | Select / Activate focused item or enter sub-menu |
| **B Button** | Cancel / Go back to previous menu / Exit to game |
| **Touchscreen** | Tap buttons, checkboxes, and lists directly |

---

## Installation

Extract the release archive to your Nintendo Switch SD card:

```
sdmc:/
└── switch/
    └── hatari/
        ├── hatari.nro       (Hatari Switch executable)
        ├── tos.img          (Your Atari TOS ROM BIOS)
        ├── games/           (Folder for .st, .msa, .zip floppy disks and games)
        ├── hdd/             (Folder for hard drive images / GEMDOS drives)
        └── saves/           (Folder for save states)
```

> **Note on TOS ROM**: You must supply your own Atari TOS ROM file (`tos.img`) in `sdmc:/switch/hatari/tos.img`. TOS 1.04 or TOS 2.06 are recommended for standard Atari ST/STE games. [EmuTOS](https://emutos.sourceforge.io/) is also supported.

---

## Credits & Acknowledgements

- **theheroGAC**: Nintendo Switch port, virtual keyboard implementation, analog/touch controls, and build integration.
- **rsn8887**: Virtual keyboard layout and UX design inspiration from [UAE4All2](https://github.com/rsn8887/uae4all2) (Vita/Switch).
- **The Hatari Team**:
  - Nicolas Pomarède
  - Thomas Huth
  - Eero Tamminen
  - Laurent Vogel
  - and all other Hatari contributors ([https://hatari.tuxfamily.org/](https://hatari.tuxfamily.org/)).
- **devkitPro**:
  - Developers of devkitA64, libnx, switch-sdl2, and the Switch homebrew toolchain.

---

## License

Hatari Switch is licensed under the **GNU General Public License v2.0 or later** ([GPL-2.0-or-later](gpl.txt)).
All original Hatari documentation and licenses remain in the source tree.
