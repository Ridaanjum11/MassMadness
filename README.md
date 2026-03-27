# MassMadness

A C++ crowd simulation and graphics rendering project using SFML 3.0.2. Simulates agent-based behavior in a dynamic environment with real-time visualization.

## Features

- **Agent-Based Simulation** — Multiple agents with independent behaviors and interactions
- **Real-time Graphics** — SFML-powered 2D rendering with smooth animations
- **Crowd Dynamics** — Simulate crowd movement and emergent behaviors
- **Modular Architecture** — Separate Agent, Simulation, and Graphics systems
- **Windows Native Build** — Compiled with MinGW g++ on Windows

## Requirements

- **Compiler**: MinGW g++ (C++17 standard or later)
- **SFML 3.0.2** library
- **Windows** (tested on Windows with MinGW)

### Installing Dependencies

#### MinGW g++
Download from [MinGW-w64](https://www.mingw-w64.org/) or use:
```powershell
winget install MinGW
```

#### SFML 3.0.2
1. Download from [SFML Releases](https://github.com/SFML/SFML/releases)
2. Extract to a known location (e.g., `D:/SFML-3.0.2`)
3. Update paths in `build.bat` if needed

## Project Structure

```
MassMadness/
├── include/          # Header files
│   ├── Agent.h       # Agent entity definition
│   ├── Simulation.h  # Simulation engine
│   └── Graphics.h    # Graphics/rendering system
├── src/              # Source files
│   ├── main.cpp      # Entry point
│   ├── Agent.cpp     # Agent implementation
│   ├── Simulation.cpp # Simulation logic
│   └── Graphics.cpp  # Rendering implementation
├── assets/           # Game assets
├── build.bat         # Windows build script
└── README.md         # This file
```

## Building

### On Windows with build.bat

1. Update paths in `build.bat` if SFML location differs:
   ```batch
   set SFML=D:\tools\SFML-3.0.2  (change to your SFML path)
   ```

2. Run the build script:
   ```powershell
   .\build.bat
   ```

3. The script will:
   - Compile source files with g++
   - Copy SFML DLL files to the working directory
   - Automatically launch the executable

### Manual Compilation (Visual Studio Code)

Press **Ctrl+Shift+B** to run the configured build task (requires `.vscode/tasks.json`).

### Manual Compilation (Command Line)

```bash
g++ -fopenmp -std=c++17 -O2 ^
    src/main.cpp src/Agent.cpp src/Simulation.cpp src/Graphics.cpp ^
    -I include ^
    -I "D:/SFML-3.0.2/include" ^
    -L "D:/SFML-3.0.2/lib" ^
    -lsfml-graphics -lsfml-window -lsfml-system ^
    -o MassMadness.exe
```

Then copy SFML DLLs:
```powershell
copy "D:/SFML-3.0.2/bin/sfml-graphics-3.dll" .
copy "D:/SFML-3.0.2/bin/sfml-window-3.dll" .
copy "D:/SFML-3.0.2/bin/sfml-system-3.dll" .
```

## Running

After building, launch the executable:
```powershell
.\MassMadness.exe
```

The `build.bat` script automatically runs the program upon successful compilation.

## Development

### Build Flags

- `-fopenmp` — OpenMP support for parallel processing
- `-std=c++17` — C++17 standard
- `-O2` — Optimization level (balanced speed/compile time)

### Linking

Linked against SFML libraries:
- `sfml-graphics` — 2D graphics
- `sfml-window` — Window and input handling
- `sfml-system` — System utilities

## Troubleshooting

### Build Fails: "SFML not found"
- Verify SFML installation path in `build.bat`
- Ensure include files exist at `${SFML}/include/SFML`
- Check library files exist at `${SFML}/lib`

### DLL Not Found at Runtime
- Ensure `sfml-*.dll` files are in the same directory as `.exe`
- `build.bat` copies them automatically; if using manual compilation, copy them manually

### Compilation Errors
- Verify MinGW g++ is in PATH: `g++ --version`
- Check C++17 is supported: `g++ -std=c++17 --version`

## Contributing

Feel free to fork, modify, and submit pull requests.

## License

MIT License — See LICENSE file (if present) for details.

## Author

Rida Anjum

---

**Last Updated**: March 2026
