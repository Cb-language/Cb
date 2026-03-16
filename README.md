# KiryatGat-1401-C♭

## Syntax

### Types
|C♭| C++ |
|--|--|
| degree | int |
| freq| double|
| note| char|
| bar| string|
| mute| bool |
| fermata| void|

 - fermata is only usable in function return value
 - For signed int/char use `flat`.
 - For unsigned int char use `sharp`.
 - Arrays are in the format of `riff <type>[size]` for example: `riff degree[2]` will create an int array with size 2, `riff riff degree[2][3]` will make a 2D 2 by 3 int array. You can add a default value via `=` operator like: `riff degree[x] = x`, will create an int array with x elements being the number x. Size is 1 by default.

### Functions
#### Signiture
`song [©𝄕func_name1, func_name2,...𝄇] <func_name>𝄕[arg1_type arg1_name], ...𝄇 [𝅘𝅥=return_type]`

 - Copyrighted functions are functions names that can be used in this function body.
 - Return type is fermata by default.
 
 #### Body
 - Each line in a function's body must start with `𝄞` and end with `𝄀`.  
  - The last line of a function is ended with `𝄂` instead of `𝄀`.
  - For an empty line you must use `𝄽`

#### Return
`B[𝄍return_value]`, by default return value is fermata.

#### Tranposer Set-Up 🪟 Windows
This step is for running the Transposer solution, if you're running the exe itselft its not neccessery.
 
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