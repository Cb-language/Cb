# KiryatGat-1401-C♭

#### 🪟 Windows

Install `vcpkg` directly under your C drive:

```powershell
cd C:\

# Clone the repository
git clone https://github.com/microsoft/vcpkg.git

# Run the bootstrap script
cd vcpkg; .\bootstrap-vcpkg.bat
```

When done, install the following dependencies:

```powershell
.\vcpkg install boost-regex:x64-windows
```