# SFML Setup Guide

If build fails with "SFML/Graphics.hpp not found", follow these steps:

## 📥 Download SFML 3.0.2

1. **Open browser** and go to:
   ```
   https://github.com/SFML/SFML/releases/tag/3.0.2
   ```

2. **Download** one of these (for Windows):
   - `SFML-3.0.2-windows-vc17-64-bit.zip` (recommended for most Windows systems)
   
   OR
   
   - `SFML-3.0.2-windows-gcc-13.2.0-mingw-64-bit.zip` (if using MinGW)

3. **Extract** to: **`C:\SFML-3.0.2`**

   Result should look like:
   ```
   C:\SFML-3.0.2\
   ├── bin\          (DLL files here)
   ├── lib\          (library files here)
   ├── include\      (SFML header files here)
   └── ...
   ```

## ✅ Verify Installation

Run in PowerShell:
```powershell
Test-Path "C:\SFML-3.0.2\include\SFML"
```

Should output: **True**

## 🔧 Now Build

Run:
```powershell
.\build.bat
```

Game should compile and launch! 🎮

---

**Need Help?**
- Check include path exists: `C:\SFML-3.0.2\include\SFML\Graphics.hpp`
- Check lib path exists: `C:\SFML-3.0.2\lib\libsfml-graphics.a`
- Make sure extract doesn't nest folders (should be `C:\SFML-3.0.2\include`, not `C:\SFML-3.0.2\SFML-3.0.2\include`)
