# How to Compile LionsDen Project in Cursor

## One-time: Remove apostrophe from paths (if not done yet)

To avoid PowerShell/script issues with the apostrophe in "Lion's Den", run once from repo root:

```powershell
.\rename_to_lionsden.ps1
```

This renames the inner project folder to `LionsDen`, removes old `Lion's Den.*` files, and deletes `Lion's Den.sln`. After that, use `LionsDen.sln` and `LionsDen.exe` everywhere.

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
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "LionsDen.sln" /p:Configuration=Release /p:Platform=x64 /t:Build
```

### For Debug Build:
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "LionsDen.sln" /p:Configuration=Debug /p:Platform=x64 /t:Build
```

## Method 3: Using Visual Studio

1. Open `Lion's Den.sln` in Visual Studio
2. Select configuration (Debug/Release) and platform (x64) from the toolbar
3. Press `Ctrl+Shift+B` or go to `Build > Build Solution`

## Output Location

- **Release**: `x64\Release\LionsDen.exe`
- **Debug**: `x64\Debug\LionsDen.exe`

## Troubleshooting

- If MSBuild is not found, ensure Visual Studio 2022 (or 2020) is installed
- Make sure you're in the project root directory when running commands
- Check that all dependencies (SDL2, Box2D, etc.) are properly configured in the project file

