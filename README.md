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

- **Meson** 1.7 or higher
- **Ninja** (or another Meson backend, Ninja recommended)
- **Qt 6** (Core, Gui, Quick modules)
- **C++23** compatible compiler
- **Windows** (uses Win32 API for input simulation and global hotkeys)
- **sccache** (optional, if you want to wrap the compiler manually)

## Building

### Using Meson

Meson is the primary build system for this project.

1. Open a terminal and navigate to the project directory:
   ```bash
   cd autoclicker
   ```

2. Make sure Qt's tools are visible to Meson. If `qmake6`, `moc`, `rcc`, and `windeployqt` are not already on your `PATH`, prepend the Qt `bin` directory:
   ```bash
   $env:Path = "$env:Qt6_DIR\bin;$env:Path"
   ```

3. Configure a dedicated build directory for the build type you want.

**Debug build**

```bash
meson setup build/meson-debug --native-file meson/native/debug.ini
meson compile -C build/meson-debug
```

**Release build**

```bash
meson setup build/meson-release --native-file meson/native/release.ini
meson compile -C build/meson-release
```

After `meson compile`, the raw executable is in the build directory and the deployed app is in:

```text
build/meson-debug/deploy/
build/meson-release/deploy/
```

That `deploy/` folder is the default output to run or package.

If you also want to use Meson's install step, it still works:

```bash
meson install -C build/meson-release --destdir stage
```

**Important:** use separate build directories for debug and release. If you configured a directory as debug already, do not try to turn it into release by reusing the same commands blindly. Either:

```bash
meson setup build/meson-release --native-file meson/native/release.ini
```

or reconfigure/wipe the existing directory:

```bash
meson setup builddir --wipe --native-file meson/native/release.ini
```

If you want a staged Meson install tree without touching the default prefix, use `--destdir`:

```bash
meson install -C build/meson-release --destdir stage
```

That produces an install tree under `build/meson-release/stage/`.

### Meson Native Files

Meson's closest equivalent to `CMakePresets.json` is a checked-in native file plus a stable setup command:

```bash
meson setup build/meson-debug --native-file meson/native/debug.ini
meson setup build/meson-release --native-file meson/native/release.ini
```

These native files are included in the repository and currently define the build type for the common debug and release configurations. In practice:

- `meson/native/debug.ini` -> debug build
- `meson/native/release.ini` -> release build

If you want to inspect the active options in an existing build directory:

```bash
meson configure build/meson-release
```

### CMake Presets

The repository also includes `CMakePresets.json` for the legacy CMake build:

```bash
cmake --preset cmake-debug
cmake --build --preset cmake-debug
cmake --build --preset cmake-debug-install
```

```bash
cmake --preset cmake-release
cmake --build --preset cmake-release
cmake --build --preset cmake-release-install
```

The `*-install` build presets run CMake's `install` target, and the CMake install step deploys Qt runtime dependencies automatically on supported CMake/Qt versions.

### Using sccache for Faster Rebuilds

[sccache](https://github.com/mozilla/sccache) is a ccache-like tool that caches compilation outputs, significantly speeding up rebuilds.

**Install sccache:**

```bash
cargo install sccache
```

Or download pre-built binaries from the [GitHub releases](https://github.com/mozilla/sccache/releases) and add `sccache` to your `PATH`.

**Usage:**

sccache can be enabled by wrapping the compiler before running `meson setup`. For example, in a Visual Studio developer shell:

```bash
$env:CC = "sccache cl"
$env:CXX = "sccache cl"
meson setup builddir --wipe
```

**Verify sccache is working:**

```bash
sccache --show-stats
```

This displays cache hit/miss statistics. On subsequent builds, you should see a high cache hit rate.

### Using Qt Creator

1. Open Qt Creator
2. Select `File > Open File or Project`
3. Navigate to the project folder and select `meson.build`
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
├── meson.build             # Meson build configuration
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
    └── qml.qrc             # Embedded QML resource file
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

**Build errors**: Verify Qt 6 is properly installed and the Qt `bin` directory is on your `PATH` before running `meson setup`.

**Release build still looks like debug**: You probably reused a build directory that was originally configured as debug. Create a fresh release build directory or rerun `meson setup <builddir> --wipe --native-file meson/native/release.ini`.

**I built successfully but couldn't find the runnable app**: Use the `deploy/` subdirectory inside the build directory. For example, a release build ends up in `build/meson-release/deploy/`.

**`meson install` writes to an unexpected place**: Use `meson install -C <builddir> --destdir <stage-dir>` if you want a local staged install tree for testing or packaging.

**Installed app won't launch on another machine**: Make sure you installed via `meson install` or `cmake --install` so the Qt deployment step runs. If you only copy the raw `.exe`, the required Qt runtime DLLs and plugins won't be beside it.

## License

This project is provided as-is for educational and personal use.

## Author

AutoClicker Application - Built with Qt 6 and C++
