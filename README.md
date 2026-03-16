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
 - For unsigned int/char use `sharp`.
 - For mute use `demen` as false and `cres` as true. 
 - default values are `""` for bar, `demen` for mute and 0 for the other types.

### Conditions
|C♭| C++ |
|--|--|
| `D𝄕<condition>𝄇𝄋...𝄌` | `if(<condition>){...}` |
| `D𝄍E𝄕<condition>𝄇𝄋...𝄌` | `else if(<condition>){...}` |
| `E𝄋...𝄌` | `else{...}` |
| `A𝄍<param>𝄋...𝄌` | `Switch(param){...}` |
| `C𝄍<value>𝄋...𝄌` | `case value: ...` |
| `div.` | `or` |
| `non div.` | `and` |

### Variables
To create a variable:
`<type> <name> [= value]`

### Loops
#### While
`G𝄕<condition>𝄇𝄋...𝄌`

#### For
There are **two** types of for
 - Increasing
	 - `Fmaj<starting_value>[♯ inc_value][# stop_value (0 by default)][𝄓param_name]𝄋...𝄌`
 - Decreasing
	 - `Fmin<starting_value>[♭ dec_value][# stop_value (0 by default)][𝄓param_name]𝄋...𝄌`
 - param_name is the name of the for's variable, defaults to be named `i`.
 - starting_value is the value of param_name at the start.
 - inc_value is the value param_name is increasing each iteration **of Fmaj only**, defaults to 1.
 - dec_value is the value param_name is decreasing each iteration **of Fmin only**, defaults to 1.
 - stop_value is the value of param_name where the loops breaks if met the following condition:
	 - When **Fmaj** `param_name < stop_value`
	 - When **Fmin** `param_name >= stop_value`

### Arrays
Array is a collection of the same types:
`riff <type>[size=1]`
Where `type` can be any type including array. In that case you can have `riff riff <base_type>[row=1][col=1]` and onwords for 3D, 4D and so on arrays.

The default value of an item inside an array is the default of the type unless said diffrently via the `=` sign whe creating an array, for example:
`riff degree[2] x = 3` will create an array of ints, size 2, when the default value is 3.
`riff	riff degree[3][2] y = -1` will create a 2D array of ints, 3 by 2 where each **item** is -1. 
`riff	riff degree[3][2] z = x` will create a 2D array of ints, 3 by 2 where each **row** is a copy of `x`.

*note: the default value can be either the base type or an array 1 dimantion lower then the declered type.*

### Functions
#### Signiture
`song [©𝄕func_name1, func_name2,...𝄇] <func_name>𝄕[arg1_type arg1_name], ...𝄇 [𝅘𝅥=return_type]`

 - Copyrighted functions are functions names that can be used in this function body.
 - Return type is fermata by default.
 
 #### Body
 - **Each line** in a function's body must **start** with `𝄞` and **end** with `𝄀`.  
  - The last line of a function is ended with `𝄂` **instead** of `𝄀`.
  - For an empty line you must use `𝄽`

#### Return
`B[𝄍return_value]`, by default return value is fermata.

## Tranposer Set-Up 🪟 Windows
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