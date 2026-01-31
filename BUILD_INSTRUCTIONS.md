# How to Compile Lion's Den Project in Cursor

## Method 1: Using the Build Script (Recommended)

1. Open the integrated terminal in Cursor (`` Ctrl+` `` or `Terminal > New Terminal`)
2. Run the build script:
   ```powershell
   .\build.ps1
   ```
   Or for Debug build:
   ```powershell
   .\build.ps1 Debug
   ```

## Method 2: Using MSBuild Directly

1. Open the integrated terminal in Cursor
2. Run one of these commands:

### For Release Build:
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "Lion's Den\Lion's Den.sln" /p:Configuration=Release /p:Platform=x64 /t:Build
```

### For Debug Build:
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "Lion's Den\Lion's Den.sln" /p:Configuration=Debug /p:Platform=x64 /t:Build
```

## Method 3: Using Visual Studio

1. Open `Lion's Den.sln` in Visual Studio
2. Select configuration (Debug/Release) and platform (x64) from the toolbar
3. Press `Ctrl+Shift+B` or go to `Build > Build Solution`

## Output Location

- **Release**: `x64\Release\Lion's Den.exe`
- **Debug**: `x64\Debug\Lion's Den.exe`

## Troubleshooting

- If MSBuild is not found, ensure Visual Studio 2022 (or 2020) is installed
- Make sure you're in the project root directory when running commands
- Check that all dependencies (SDL2, Box2D, etc.) are properly configured in the project file

