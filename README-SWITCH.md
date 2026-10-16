# Hatari for Nintendo Switch

[Hatari](https://github.com/hatari/hatari), the Atari ST/STE/TT/Falcon emulator, built as Nintendo Switch homebrew (`.nro`) with devkitA64, libnx, and hardware-accelerated SDL2.
GitHub Repository: [https://github.com/theheroGAC/hatari-Switch](https://github.com/theheroGAC/hatari-Switch)

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

## Installing

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

Those sub-directories are created automatically on first start. The disk images you want to play go into `games/` and are inserted from Hatari's F12 menu (`Disk Drive A`/`B`, `Cartridge`), and hard disk images into `hdd/`.

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
