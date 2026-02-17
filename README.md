# MojoPunch

A simple Gain plugin built with JUCE 8 and the Pamplejuce template.

## Features

- **Single Gain Parameter**: 0.0 to 1.0 range with vertical slider UI
- **Cross-Platform**: Builds for Windows, macOS, and Linux
- **Plugin Formats**: VST3 and CLAP support
- **CI/CD**: Automated builds via GitHub Actions
- **Testing**: Unit tests with Catch2 and pluginval validation

## Building

### Prerequisites
- CMake 3.25 or higher
- C++23 compiler (MSVC, Clang, or GCC)

### Build Commands
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Output Location
Built plugins will be in:
```
build/MojoPunch_artefacts/Release/
├── VST3/MojoPunch.vst3
└── CLAP/MojoPunch.clap
```

## GitHub Actions

This project uses GitHub Actions to automatically build on all platforms:
- **Windows**: MSVC, generates `.exe` installer
- **macOS**: Universal binary (ARM64 + x86_64), generates `.pkg`
- **Linux**: Clang, generates `.zip` archive

Artifacts are uploaded after each push and can be downloaded from the Actions tab.

## Built with Pamplejuce

This project is based on [Pamplejuce](https://github.com/sudara/pamplejuce), a modern JUCE plugin template.

Pamplejuce provides:
1. JUCE 8.x as a git submodule
2. CPM for dependency management
3. Cross-platform CMake build system
4. Catch2 testing framework
5. Melatonin Inspector for UI debugging
6. GitHub Actions CI/CD with pluginval validation
7. Code signing support for macOS and Windows

For more information, see the [Pamplejuce documentation](https://melatonin.dev/manuals/pamplejuce/).

## License

See LICENSE file for details.
