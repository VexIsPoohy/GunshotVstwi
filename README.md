# GUNSHOT

GUNSHOT is a JUCE 7 VST3 distortion plugin.

## Build on Windows

Requires Visual Studio 2022 with the Desktop development with C++ workload and CMake.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The VST3 bundle is created at:

```text
build\Gunshot_artefacts\Release\VST3\GUNSHOT.vst3
```

Copy the `GUNSHOT.vst3` folder to:

```text
C:\Program Files\Common Files\VST3\
```

In FL Studio, open **Options > Manage plugins** and scan for installed plugins.

## Build with GitHub Actions

Push the project to GitHub, then open the **Actions** tab and run **Build VST3**.
Download the `GUNSHOT-vst3` artifact from the completed run and extract it. The
artifact is a Windows VST3 bundle.

The first CMake configure downloads JUCE 7.0.12.
