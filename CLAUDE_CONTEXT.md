# MojoPunch - JUCE Plugin Development Context

**Last Updated:** 2026-02-17
**Current Version:** 0.0.1
**Status:** Fresh template, ready for custom DSP development

---

## Project Overview

**MojoPunch** is a JUCE-based audio plugin built on the **Pamplejuce** template. It's a production-ready foundation for creating VST3/CLAP audio effects or instruments for DAWs like Bitwig Studio.

**Current State:** Template implementation with NO custom DSP yet. All infrastructure is in place, ready for audio algorithm development.

---

## Quick Facts

| Property | Value |
|----------|-------|
| **Project Name** | MojoPunch |
| **Company** | MojoPunch |
| **Bundle ID** | com.mojopunch.mojopunch |
| **Version** | 0.0.1 |
| **Manufacturer Code** | Mojo |
| **Plugin Code** | Mp01 |
| **C++ Standard** | C++23 |
| **Frameworks** | JUCE 8.x, Catch2 v3.8.1 |
| **Plugin Formats** | VST3, CLAP |
| **Build System** | CMake 3.25+, Ninja |

---

## Repository Location

```
/root/MojoPunch/
```

---

## Key Source Files (Where to Start Development)

### 1. Audio Processing (DSP)
**File:** `source/PluginProcessor.cpp` (187 lines)

This is where audio magic happens. Currently empty stub:

```cpp
void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear extra output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // TODO: Add your DSP algorithm here
    // Example: buffer.applyGain(0.5f); // Simple volume reduction
}
```

**Key methods to implement:**
- `processBlock()` - Main audio processing loop
- `prepareToPlay()` - Initialize DSP state (sample rate, buffer size)
- `releaseResources()` - Cleanup on plugin unload
- Parameter getters/setters for user controls

### 2. User Interface
**File:** `source/PluginEditor.cpp` (49 lines)

Currently displays "Hello from MojoPunch v0.0.1" with Inspector button.

```cpp
class PluginEditor : public juce::AudioProcessorEditor
{
private:
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    // TODO: Add your UI components here
    // - Knobs (juce::Slider)
    // - Buttons (juce::TextButton)
    // - Labels (juce::Label)
    // - Custom visualizations (juce::Component)
};
```

**Window Size:** 400x300 pixels (configurable in constructor)

### 3. Build Configuration
**File:** `CMakeLists.txt` (170 lines)

**To enable additional plugin formats**, modify line 38:
```cmake
set(FORMATS VST3)  # Current
# Add more: VST3 AU AUv3 AAX Standalone
```

**Plugin metadata is set here:**
- PRODUCT_NAME: "MojoPunch"
- COMPANY_NAME: "MojoPunch"
- PLUGIN_CODE: "Mp01"
- PLUGIN_MANUFACTURER_CODE: "Mojo"

---

## Project Structure

```
/root/MojoPunch/
├── source/                       # Plugin source code (START HERE)
│   ├── PluginProcessor.h/.cpp    # DSP/audio processing
│   └── PluginEditor.h/.cpp       # User interface
│
├── tests/                        # Unit tests (Catch2)
│   ├── PluginBasics.cpp          # Basic instantiation tests
│   └── helpers/test_helpers.h    # UI testing utilities
│
├── benchmarks/                   # Performance benchmarks
│   └── Benchmarks.cpp            # Constructor/editor performance
│
├── assets/                       # Binary assets (embedded in plugin)
│   └── images/                   # Image resources
│
├── modules/                      # Git submodules
│   ├── clap-juce-extensions/     # CLAP format support
│   └── melatonin_inspector/      # Live UI debugging tool
│
├── JUCE/                         # JUCE framework (submodule, v8.x)
│
├── cmake/                        # CMake build scripts
│   ├── JUCEDefaults.cmake
│   ├── Tests.cmake
│   ├── Benchmarks.cmake
│   └── [12 other modules]
│
├── packaging/                    # Platform installers
│   ├── icon.png
│   ├── installer.iss             # Windows NSIS
│   ├── dmg.json                  # macOS DMG
│   └── distribution.xml.template # macOS PKG
│
├── Builds/                       # Build output (gitignored)
│   ├── MojoPunch_artefacts/Release/
│   │   ├── VST3/MojoPunch.vst3/
│   │   └── CLAP/MojoPunch.clap
│   ├── Tests                     # Test executable
│   └── Benchmarks                # Benchmark executable
│
├── .github/workflows/            # CI/CD pipeline
│   └── build_and_test.yml        # Multi-platform builds + tests
│
├── CMakeLists.txt                # Main build configuration
├── VERSION                       # Version file (0.0.1)
├── README.md                     # Pamplejuce docs
├── .clang-format                 # Code style config
└── .gitignore                    # Git ignore rules
```

---

## Build System

### Requirements
- **CMake 3.25+**
- **C++23 compiler:**
  - Linux: Clang 16+ or GCC 13+
  - macOS: Xcode 15+
  - Windows: MSVC 2022
- **Ninja** (recommended) or Make
- **Git** (for submodules)

### Build Commands

```bash
# 1. Initialize submodules (first time only)
cd /root/MojoPunch
git submodule update --init --recursive

# 2. Configure build
cmake -B Builds -DCMAKE_BUILD_TYPE=Release -G Ninja

# 3. Build all targets
cmake --build Builds

# 4. Run tests
cd Builds
ctest --verbose --output-on-failure

# Or run tests directly:
./Tests

# 5. Run benchmarks
./Benchmarks
```

### Build Outputs

After building, find your plugins:

**Linux:**
```
Builds/MojoPunch_artefacts/Release/VST3/MojoPunch.vst3/
Builds/MojoPunch_artefacts/Release/CLAP/MojoPunch.clap
```

**macOS:**
```
~/Library/Audio/Plug-Ins/VST3/MojoPunch.vst3
~/Library/Audio/Plug-Ins/CLAP/MojoPunch.clap
```

**Windows:**
```
C:\Program Files\Common Files\VST3\MojoPunch.vst3
C:\Program Files\Common Files\CLAP\MojoPunch.clap
```

---

## Plugin Formats

### Currently Enabled

1. **VST3** (Steinberg, industry standard)
   - Cross-platform (macOS, Windows, Linux)
   - Supported by: Bitwig, Cubase, Reaper, FL Studio, etc.
   - Location: `Builds/.../VST3/MojoPunch.vst3/`

2. **CLAP** (Community, modern format)
   - Cross-platform
   - Lower latency, better parameter handling
   - Supported by: Bitwig, Reaper, Ardour
   - Location: `Builds/.../CLAP/MojoPunch.clap`

### Available (Disabled by Default)

To enable, edit `CMakeLists.txt` line 38:

```cmake
set(FORMATS VST3 AU AUv3 AAX Standalone)
```

- **AU** - Audio Units (macOS only, Logic Pro, GarageBand)
- **AUv3** - Audio Units v3 (iOS/macOS, modern AU)
- **AAX** - Avid format (Pro Tools) - requires AAX SDK
- **Standalone** - Desktop application (no DAW needed)

---

## Current Git Status

```
Branch: master
Last Commit: 006248c "Initial import from Pamplejuce template"
Date: 2026-02-14 20:54:04 +0100
```

### Staged Changes (ready to commit)
- .gitmodules (submodule config)
- JUCE framework
- cmake build scripts
- clap-juce-extensions module
- melatonin_inspector module

### Modified (not staged)
- .github/workflows/build_and_test.yml - CI/CD pipeline tweaks
- CMakeLists.txt - Build config changes
- JUCE - Updated to newer commits
- packaging/ - Installer script modifications
- tests/PluginBasics.cpp - Custom test additions

**Recommendation:** Commit these changes before starting development:
```bash
git add -A
git commit -m "Configure MojoPunch project from Pamplejuce template"
```

---

## Development Workflow

### 1. Typical Development Cycle

```bash
# Edit source code
vim source/PluginProcessor.cpp

# Rebuild
cmake --build Builds

# Test in Bitwig
# VST3 is auto-copied to plugin folder on macOS
# Linux/Windows: manually copy from Builds/

# Run unit tests
./Builds/Tests

# Run benchmarks (if performance matters)
./Builds/Benchmarks
```

### 2. Hot Reload in Bitwig

Bitwig Studio can reload plugins without restarting:
1. Build your plugin
2. In Bitwig, remove the plugin from the track
3. Add it back (Bitwig rescans on add)

Or use Bitwig's "Rescan" button in the browser.

### 3. Debugging

**Option A: Melatonin Inspector (Built-in)**
- Click "Inspect the UI" button in plugin
- Live UI debugging with component boundaries
- No IDE needed

**Option B: IDE Debugger**
```bash
# Build with debug symbols
cmake -B Builds -DCMAKE_BUILD_TYPE=Debug

# Launch Bitwig from debugger
lldb -- /Applications/Bitwig\ Studio.app/Contents/MacOS/BitwigStudio
# or
gdb --args bitwig-studio
```

Set breakpoints in `PluginProcessor::processBlock()`.

---

## Testing Framework

### Unit Tests (Catch2 v3.8.1)

**File:** `tests/PluginBasics.cpp`

**Current tests:**
1. Plugin instantiation (verifies name is "MojoPunch")
2. Intel IPP version check (if compiled with IPP)

**To add new tests:**
```cpp
TEST_CASE("My new test")
{
    PluginProcessor processor;
    // Your test assertions here
    REQUIRE(processor.getSampleRate() > 0);
}
```

**Run tests:**
```bash
./Builds/Tests
# or
ctest --verbose -C Release
```

### Benchmarks

**File:** `benchmarks/Benchmarks.cpp`

**Current benchmarks:**
1. Processor constructor performance
2. Processor destructor performance
3. Editor open/close roundtrip

**Run benchmarks:**
```bash
./Builds/Benchmarks
```

---

## JUCE Modules Linked

```cmake
target_link_libraries(SharedCode INTERFACE
    juce_audio_utils           # Audio I/O utilities
    juce_audio_processors      # Plugin base classes
    juce_dsp                   # DSP algorithms (filters, oscillators, etc.)
    juce_gui_basics            # Core UI components
    juce_gui_extra             # Additional widgets
    melatonin_inspector        # UI debugging
    Assets                     # Binary data (images, etc.)
)
```

**Key JUCE classes you'll use:**
- `juce::AudioProcessor` - Base class for plugins
- `juce::AudioProcessorEditor` - Base class for UI
- `juce::AudioBuffer<float>` - Audio sample buffer
- `juce::dsp::*` - DSP building blocks (filters, oscillators, etc.)
- `juce::Slider`, `juce::Button`, `juce::Label` - UI controls

---

## Compiler Optimizations

### Release Builds

**Aggressive optimizations enabled:**
```cmake
# Clang/GCC
-Ofast -ffast-math

# MSVC
/Ox /fp:fast
```

**What this means:**
- Maximum speed optimization
- Fast math (may break IEEE 754 compliance)
- Link-Time Optimization (LTO) enabled
- Perfect for audio DSP (real-time performance critical)

**Trade-offs:**
- Slightly larger binaries
- Longer compile times
- Non-deterministic floating point (usually fine for audio)

### Debug Builds

**No optimizations:**
- Full debugging symbols
- All assertions enabled
- Easier to step through code

---

## Intel IPP Integration (Optional)

**Intel Integrated Performance Primitives** - Optimized SIMD math/DSP library.

**Status:** Conditionally compiled if available
- macOS (x86_64): IPP 2021.9.1
- Linux/Windows: IPP 2022.3.0

**Usage in code:**
```cpp
#ifdef PAMPLEJUCE_IPP
    // Use IPP functions for faster DSP
    ippsAdd_32f(src1, src2, dst, len);
#else
    // Fallback to standard implementation
    for (int i = 0; i < len; ++i)
        dst[i] = src1[i] + src2[i];
#endif
```

**To disable IPP:** Comment out IPP sections in CMakeLists.txt

---

## CI/CD Pipeline (GitHub Actions)

**File:** `.github/workflows/build_and_test.yml`

### Build Matrix

Automatically builds and tests on:
- **Linux** (Ubuntu 22.04, Clang, x86_64)
- **macOS** (macOS 14, Universal binary: arm64 + x86_64)
- **Windows** (Latest, MSVC, x86_64)

### Pipeline Stages

1. **Checkout** - Clone repo + submodules
2. **Setup** - Install dependencies (JUCE libs, Intel IPP, etc.)
3. **Build** - CMake configure + Ninja build
4. **Test** - Run unit tests + benchmarks
5. **Validate** - pluginval v1.0.3 (strictness level 10)
6. **Package** - Platform-specific installers
7. **Upload** - GitHub Actions artifacts

### Artifacts Generated

- Linux: `.zip` archive
- macOS: `.pkg` installer (notarized if secrets configured)
- Windows: `.exe` installer (signed if Azure configured)

### Code Signing (Optional)

**Requires GitHub Secrets:**

**macOS:**
- DEV_ID_APP_CERT
- DEV_ID_APP_PASSWORD
- DEVELOPER_ID_APPLICATION
- NOTARIZATION_USERNAME
- NOTARIZATION_PASSWORD
- TEAM_ID

**Windows:**
- AZURE_TENANT_ID
- AZURE_CLIENT_ID
- AZURE_CLIENT_SECRET
- AZURE_CERT_PROFILE_NAME

**Without these secrets:** Pipeline builds but doesn't sign. Plugins work but show "unsigned" warnings.

---

## Common Development Tasks

### Change Plugin Name

Edit `CMakeLists.txt` lines 26-29:
```cmake
set(PRODUCT_NAME "MojoPunch")           # Display name
set(COMPANY_NAME "MojoPunch")           # Your company
set(PLUGIN_MANUFACTURER_CODE "Mojo")    # 4-char manufacturer code
set(PLUGIN_CODE "Mp01")                 # 4-char plugin code (must be unique)
```

**Important:** Changing PLUGIN_CODE breaks preset compatibility!

### Add Parameters (Knobs, Sliders)

**1. In PluginProcessor.h**, add AudioProcessorValueTreeState:
```cpp
private:
    juce::AudioProcessorValueTreeState parameters;
```

**2. In PluginProcessor.cpp constructor**, create parameters:
```cpp
PluginProcessor::PluginProcessor()
    : parameters(*this, nullptr, "PARAMETERS",
        {
            std::make_unique<juce::AudioParameterFloat>(
                "gain",           // Parameter ID
                "Gain",           // Display name
                0.0f, 1.0f,       // Range: min, max
                0.5f              // Default value
            )
        })
{
}
```

**3. In PluginEditor.cpp**, add slider:
```cpp
private:
    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

// In constructor:
addAndMakeVisible(gainSlider);
gainAttachment = std::make_unique<...>(
    processorRef.parameters, "gain", gainSlider
);
```

**4. In processBlock()**, use parameter:
```cpp
float gainValue = *parameters.getRawParameterValue("gain");
buffer.applyGain(gainValue);
```

### Add Binary Assets (Images, Audio Files)

1. Place files in `assets/` directory:
   ```
   assets/logo.png
   assets/background.jpg
   ```

2. Access in code:
   ```cpp
   auto logoData = BinaryData::logo_png;
   auto logoSize = BinaryData::logo_pngSize;

   juce::Image logo = juce::ImageFileFormat::loadFrom(logoData, logoSize);
   ```

CMake automatically generates `BinaryData.h` from `assets/`.

### Increase Window Size

Edit `source/PluginEditor.cpp` constructor:
```cpp
setSize(800, 600);  // Was 400x300
```

### Enable More Plugin Formats

Edit `CMakeLists.txt` line 38:
```cmake
set(FORMATS VST3 AU Standalone)  # Add AU and Standalone
```

Rebuild:
```bash
cmake -B Builds -DCMAKE_BUILD_TYPE=Release
cmake --build Builds
```

---

## Bitwig Studio Integration

### Installing Plugins

**VST3 (Linux):**
```bash
mkdir -p ~/.vst3/
cp -r Builds/MojoPunch_artefacts/Release/VST3/MojoPunch.vst3 ~/.vst3/
```

**CLAP (Linux):**
```bash
mkdir -p ~/.clap/
cp Builds/MojoPunch_artefacts/Release/CLAP/MojoPunch.clap ~/.clap/
```

**Then in Bitwig:**
1. Open Settings → Locations → VST3/CLAP paths
2. Verify paths are included
3. Click "Rescan" button

### Using the Plugin

1. Create new track (Instrument or Audio FX)
2. Browser → Plug-ins → MojoPunch
3. Drag onto track
4. "Hello from MojoPunch" window appears
5. Click "Inspect the UI" to debug layout

---

## Known Issues / Limitations

### Current State

✓ **Working:**
- Project compiles successfully
- VST3 + CLAP plugins build
- Tests pass
- Benchmarks run
- UI displays with inspector

✗ **Not Implemented:**
- No DSP algorithm (passes audio through unchanged)
- No user controls (just placeholder UI)
- No parameters (no knobs/sliders)
- No presets
- No state saving

### Template Limitations

- **AAX format requires AAX SDK** (not included, must download from Avid)
- **macOS code signing requires Apple Developer account** ($99/year)
- **Windows signing requires Azure Trusted Signing** (paid service)
- **JUCE is GPL3 licensed** - commercial use requires JUCE license ($40/month) OR GPL3 your plugin

---

## Next Steps

### Immediate (Get Started)

1. **Build the project** - Verify everything compiles
   ```bash
   cmake -B Builds -DCMAKE_BUILD_TYPE=Release -G Ninja
   cmake --build Builds
   ```

2. **Test in Bitwig** - Load the plugin and see the placeholder UI

3. **Commit initial state** - Save the working template
   ```bash
   git add -A
   git commit -m "Initial MojoPunch project setup"
   ```

### Development (Add Functionality)

4. **Design your DSP algorithm** - What should the plugin do?
   - Audio effect (reverb, delay, distortion, EQ, etc.)
   - Instrument (synthesizer, sampler, etc.)
   - MIDI effect (arpeggiator, chord generator, etc.)

5. **Implement processBlock()** - Write the audio processing code

6. **Add parameters** - Create knobs/sliders for user control

7. **Design UI** - Make it look professional

8. **Add presets** - Save/load common settings

9. **Write tests** - Verify your DSP works correctly

10. **Optimize** - Profile and improve performance if needed

### Distribution (Share Your Plugin)

11. **Version bump** - Update VERSION file (0.0.1 → 0.1.0)

12. **Configure signing** - Add GitHub Secrets for code signing

13. **Create release** - Tag and push to trigger CI/CD
    ```bash
    git tag -a v0.1.0 -m "First release"
    git push origin v0.1.0
    ```

14. **Download installers** - From GitHub Actions artifacts

15. **Distribute** - Share your plugin with the world!

---

## Resources

### JUCE Documentation
- Official Docs: https://docs.juce.com/
- JUCE Forum: https://forum.juce.com/
- Tutorials: https://juce.com/learn/tutorials

### Pamplejuce Template
- GitHub: https://github.com/sudara/pamplejuce
- Wiki: Extensive documentation on CI/CD, signing, packaging

### Audio DSP
- "Designing Audio Effect Plugins in C++" by Will Pirkle
- "The Audio Programming Book" by Boulanger & Lazzarini
- DSP Stack Exchange: https://dsp.stackexchange.com/

### Bitwig-Specific
- Bitwig VST3 guide: Check Bitwig manual
- CLAP specification: https://github.com/free-audio/clap

---

## Quick Reference

### Rebuild & Test
```bash
cmake --build Builds && ./Builds/Tests
```

### Clean Rebuild
```bash
rm -rf Builds
cmake -B Builds -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build Builds
```

### Install to Bitwig (Linux)
```bash
cp -r Builds/MojoPunch_artefacts/Release/VST3/MojoPunch.vst3 ~/.vst3/
cp Builds/MojoPunch_artefacts/Release/CLAP/MojoPunch.clap ~/.clap/
```

### Update Submodules
```bash
git submodule update --remote --merge
```

### Format Code
```bash
find source tests -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

---

**End of Context Document**
