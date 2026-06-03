# AutoClicker

A modern autoclicker application built with C++ and Qt/QML featuring a clean, dark-themed interface.

## Features

- **Flexible Button Selection**: Choose any mouse button or keyboard key for autoclicking by simply pressing it once
- **Customizable Click Frequency**: Set click intervals and enable random frequency variation between min/max values
- **Two Activation Modes**:
  - **Toggle Mode**: Press the keybind once to start, press again to stop
  - **Hold Mode**: Clicks only while the keybind is held down
- **Multiple Profiles**: Create and manage multiple autoclick profiles, each with its own keybind
- **Comprehensive Key Support**: Supports all standard keyboard keys including letter keys, numbers, function keys, navigation keys, numpad, mouse buttons, media keys, and modifier keys

## Requirements

- **Meson** 1.7 or higher
- **Ninja** (backend for Meson)
- **Qt 6** (Core, Gui, Quick, Qml modules)
- **C++23** compatible compiler
- **Windows** (uses Win32 API for input simulation and global hotkeys)

> **Note:** Some games and applications with anti-cheat or input protection systems (e.g., Genshin Impact) may require running the autoclicker as administrator for the simulated input to register.

## Usage

1. **Launch the application** - The main window displays the autoclicker interface
2. **Add a Profile** - Click the "+ Add Profile" button at the bottom
3. **Configure the Profile**:
   - **Target Button**: Click the "Click to set" button and press any key or mouse button to select what will be autoclicked
   - **Keybind**: Click the "Click to set" button and press any key or mouse button to set the activation keybind
   - **Frequency**: Use the spinbox to set the click interval in seconds
   - **Mode**: Choose between Toggle or Hold mode
4. **Activate**: Press your configured keybind to start/stop (Toggle) or hold (Hold) autoclicking
> For games with anti-cheat or input protection (e.g., Genshin Impact), run the application as administrator.

## Building

### Using Meson

```powershell
meson setup build/meson-release --native-file meson/native/release.ini
meson compile -C build/meson-release
```

```powershell
meson setup build/meson-debug --native-file meson/native/debug.ini
meson compile -C build/meson-debug
```

Deploy directory destination: `build/meson-release/deploy/`

### Using CMake

```bash
cmake --preset cmake-release
cmake --build --preset cmake-release
cmake --build --preset cmake-release-install
```

## Architecture

- **Clicker**: Handles keyboard and mouse input simulation using Windows `SendInput` API. Maintains a comprehensive map of all virtual key codes.
- **KeyBinder**: Implements `QAbstractNativeEventFilter` to capture global keyboard and mouse events. Uses `RegisterHotKey` and low-level hooks for all types of input.
- **AutoClickerController**: `QAbstractListModel` exposing profile data to QML. Manages timers for each active profile.
- **ClickProfile**: Data model representing a single autoclick configuration with properties for target button, keybind, frequency, and mode.

## License

This project is provided as-is for educational and personal use.

## Author

[Murchiz](https://github.com/Murchiz)
