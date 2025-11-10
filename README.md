# KiryatGat-1401-C♭

## 🪟 Windows
### 📦 Installing vcpkg

1. Open **PowerShell** or **Command Prompt** as Administrator.
2. Navigate to your **C:** drive:

   ```bash
   cd C:\
   ```

3. Clone the official **vcpkg** repository:

   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   ```

4. Run the bootstrap script to build vcpkg:

   ```bash
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

---

### 📚 Installing Dependencies

Once vcpkg is built, install the required package(s):

```bash
.\vcpkg install boost-regex:x64-windows
```

This will automatically download, build, and integrate the **Boost.Regex** library for 64-bit Windows.

---