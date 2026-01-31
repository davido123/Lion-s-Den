# MSVC Integration Guide for Cursor

## ✅ Setup Complete

MSVC integration has been configured for your project. Here's what was set up:

### Files Created/Updated:
- `.vscode/tasks.json` - Build tasks for MSBuild
- `.vscode/launch.json` - Debug configurations
- `.vscode/settings.json` - Updated to use MSVC
- `.vscode/c_cpp_properties.json` - IntelliSense configuration

## 🚀 How to Use

### Building the Project

1. **Keyboard Shortcut**: Press `Ctrl+Shift+B` (or `Cmd+Shift+B` on Mac)
   - This will build the Release configuration by default

2. **Command Palette**: 
   - Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac)
   - Type "Tasks: Run Task"
   - Select one of:
     - `MSBuild: Build Release` (default)
     - `MSBuild: Build Debug`
     - `MSBuild: Rebuild Release`
     - `MSBuild: Rebuild Debug`
     - `MSBuild: Clean`

3. **Terminal Menu**: 
   - Go to `Terminal > Run Task...`
   - Select the build task you want

### Debugging

1. **Press F5** or click the Debug button in the sidebar
   - This will build (if needed) and launch the Debug configuration

2. **Debug Configurations Available**:
   - **Debug (MSVC)** - Builds and runs Debug build
   - **Release (MSVC)** - Builds and runs Release build

3. **Breakpoints**: Click in the left margin next to any line to set a breakpoint

### IntelliSense

- Code completion, error detection, and hover information should now work with MSVC
- If IntelliSense doesn't work, try:
  1. Reload the window: `Ctrl+Shift+P` → "Developer: Reload Window"
  2. Check that `c_cpp_properties.json` has correct paths

## 📝 Notes

- The build tasks automatically find MSBuild from Visual Studio 2022
- If you have a different VS installation path, update `tasks.json`
- Executables are located at:
  - Debug: `x64/Debug/LionsDen.exe`
  - Release: `x64/Release/LionsDen.exe`

## 🔧 Troubleshooting

### Build fails with "MSBuild not found"
- Update the MSBuild path in `.vscode/tasks.json` to match your Visual Studio installation

### IntelliSense not working
- Check `.vscode/c_cpp_properties.json` has correct include paths
- Reload the window
- Check the C/C++ extension is installed

### Debugger not starting
- Ensure the executable exists at the path specified in `launch.json`
- Check that the build completed successfully
- Verify the working directory (`cwd`) in `launch.json` is correct

