# AutoClicker Application

A modern autoclicker application built with C++ and Qt/QML featuring a clean, dark-themed interface.

## Features

- **Flexible Button Selection**: Choose any mouse button or keyboard key for autoclicking by simply pressing it once
- **Customizable Click Frequency**: Set click intervals from 0.01 seconds to 100 seconds
- **Two Activation Modes**:
  - **Toggle Mode**: Press the keybind once to start, press again to stop
  - **Hold Mode**: Clicks only while the keybind is held down
- **Multiple Profiles**: Create and manage multiple autoclick profiles, each with its own keybind
- **Comprehensive Key Support**: Supports all standard keyboard keys including:
  - A-Z letter keys
  - 0-9 number keys
  - F1-F24 function keys
  - Navigation keys (arrows, home, end, page up/down, etc.)
  - Numpad keys
  - Mouse buttons (left, right, middle, X1, X2)
  - Media and browser keys
  - Modifier keys (Shift, Ctrl, Alt)
  - Symbol and punctuation keys

## Requirements

- **CMake** 3.16 or higher
- **Qt 6** (Core, Gui, Quick modules)
- **C++17** compatible compiler
- **Windows** (uses Win32 API for input simulation and global hotkeys)
- **sccache** (optional, for faster rebuilds — auto-detected when available)

## Building

### Using CMake (Recommended)

1. Open a terminal and navigate to the project directory:
   ```bash
   cd autoclicker
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure the project:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=<path_to_qt6_installation>
   ```

4. Build the project:

```bash
cmake --build . --config Release
```

### Using sccache for Faster Rebuilds

[sccache](https://github.com/mozilla/sccache) is a ccache-like tool that caches compilation outputs, significantly speeding up rebuilds. It is auto-detected and enabled by default when available.

**Install sccache:**

```bash
cargo install sccache
```

Or download pre-built binaries from the [GitHub releases](https://github.com/mozilla/sccache/releases) and add `sccache` to your `PATH`.

**Usage:**

sccache is automatically enabled when CMake detects it on your system. No extra configuration is needed — just make sure `sccache` is in your `PATH` before running CMake.

To explicitly **disable** sccache:

```bash
cmake .. -DUSE_SCCACHE=OFF -DCMAKE_PREFIX_PATH=<path_to_qt6_installation>
```

**Verify sccache is working:**

```bash
sccache --show-stats
```

This displays cache hit/miss statistics. On subsequent builds, you should see a high cache hit rate.

### Using Qt Creator

1. Open Qt Creator
2. Select `File > Open File or Project`
3. Navigate to the project folder and select `CMakeLists.txt`
4. Configure the project with your Qt Kit
5. Build and run

## Usage

1. **Launch the application** - The main window displays the autoclicker interface

2. **Add a Profile** - Click the "+ Add Profile" button at the bottom

3. **Configure the Profile**:
   - **Target Button**: Click the "Click to set" button and press any key or mouse button to select what will be autoclicked
   - **Keybind**: Click the "Click to set" button and press any key or mouse button to set the activation keybind
   - **Frequency**: Use the spinbox to set the click interval in seconds (e.g., 0.50 = 2 clicks per second)
   - **Mode**: Choose between Toggle or Hold mode

4. **Activate**:
   - Click the "Start" button to manually start/stop
   - Or press your configured keybind to activate remotely

5. **Manage Profiles**:
   - Add multiple profiles with different settings
   - Each profile can be active independently
   - Delete profiles using the "✕" button

## Architecture

### Project Structure

```
autoclicker/
├── CMakeLists.txt          # Build configuration
├── src/
│   ├── main.cpp            # Application entry point
│   ├── autoclickercontroller.h/cpp  # Main controller and list model
│   ├── clicker.h/cpp       # Input simulation (keyboard/mouse clicks)
│   ├── keybinder.h/cpp     # Global hotkey registration and key detection
│   └── models/
│       └── clickprofile.h/cpp  # Profile data model
├── qml/
│   ├── main.qml            # Main application window
│   └── ClickProfileItem.qml    # Profile card component
└── resources/
    └── qml.qrc             # QML resource file
```

### Key Components

- **Clicker**: Handles keyboard and mouse input simulation using Windows SendInput API. Maintains a comprehensive map of all virtual key codes.

- **KeyBinder**: Implements `QAbstractNativeEventFilter` to capture global keyboard and mouse events. Registers hotkeys using Windows RegisterHotKey API.

- **AutoClickerController**: Extends `QAbstractListModel` to expose profile data to QML. Manages timers for each active profile.

- **ClickProfile**: Data model representing a single autoclick configuration with properties for target button, keybind, frequency, and mode.

## Customization

### Adding New Keys

The `Clicker::initializeKeyMap()` function contains the complete mapping of virtual key codes to human-readable names. To add support for additional keys:

1. Add the key code and name to the `s_keyNames` hash map in `clicker.cpp`
2. The reverse lookup (`s_nameToKey`) is automatically built

### Modifying the UI

The QML files use a dark theme with the following color scheme:
- Background: `#1e1e1e`
- Cards: `#2d2d2d`
- Accent: `#3d5a80`
- Active state: `#4CAF50`

Modify these colors in the QML files to customize the appearance.

## Known Limitations

- Windows only (uses Win32 API)
- Hotkeys are global and work across all applications
- Mouse button detection requires the mouse event to occur within the application window
- Some special keys may not work as hotkeys due to system restrictions

## Troubleshooting

**Hotkeys not registering**: Some keys are reserved by the system (like F12 for debugging). Try a different key.

**Clicks not working**: Ensure the target application has focus and accepts the type of input you're simulating.

**Build errors**: Verify Qt 6 is properly installed and `CMAKE_PREFIX_PATH` points to your Qt installation directory.

## License

This project is provided as-is for educational and personal use.

## Author

AutoClicker Application - Built with Qt 6 and C++