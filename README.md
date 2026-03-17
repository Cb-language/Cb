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
- Primitive are: `degree`, `freq`, `note` and `mute`
---
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
---
### Variables
To create a variable:
`<type> <name> [= value]`

---
### Loops
#### *While*
`G𝄕<condition>𝄇𝄋...𝄌`

---
#### *For*
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

---
### Arrays
#### Declaration
Array is a collection of the same types:
`riff <type>[size=1]`
Where `type` can be any type including array. In that case you can have `riff riff <base_type>[row=1][col=1]` and onwords for 3D, 4D and so on arrays.

The default value of an item inside an array is the default of the type unless said diffrently via the `=` sign whe creating an array, for example:
`riff degree[2] x = 3` will create an array of ints, size 2, when the default value is 3.
`riff riff degree[3][2] y = -1` will create a 2D array of ints, 3 by 2 where each **item** is -1. 
`riff riff degree[3][2] z = x` will create a 2D array of ints, 3 by 2 where each **row** is a copy of `x`.

*note: the default value can be either the base type or an array 1 dimantion lower then the declered type.*

---
#### Access
`<array_var><[index]>` will give the array's item at index's position (0 based).

The index may be negative, -1 is the last position, -2 second to last and so on.

#### Slicing
`<array_var><[<start_index>:[stop_index][:][step_size]]>` will give back array with the same type having any item from `start_index` until `stop_index` jumping `step_size` each time.

- `stop_index` defaults to -1
- `step_size` defaults to 1

For example:
> ```
> arr[0::] is all the array
> arr[-1:-5:-1] is reversed array until 6th index from last
>```



---
### Operations
Each primitive type can do:
- `+`, `+=`
- `-`, `-=`
- `*`, `*=`
- `/`, `/=`
- `%`, `%=`

A string can do: `+`, `+=`

---
### Functions
#### Signiture
`song [©𝄕func_name1, func_name2,...𝄇] <func_name>𝄕[arg1_type arg1_name], ...𝄇 [𝅘𝅥=return_type]`

 - Copyrighted functions are functions names that can be used in this function body.
 - Return type is fermata by default.
 - No need to (and can't) copyright constructor and methodes of a class.
 
 ---
 #### Body
 - **Each line** in a function's body must **start** with `𝄞` and **end** with `𝄀`.  
  - The last line of a function is ended with `𝄂` **instead** of `𝄀`.
  - For an empty line you must use `𝄽`

 ---
#### Return
`B[𝄍return_value]`, by default return value is fermata.

 ---
 ### Classes
 #### Class Signiture
 `instrument <name> [: <parent_name>]𝄋...𝄌 𝄀`

- All classes are inheriting from `Object` 

*note: each decleration of an attribute[^1] must end with `𝄀`.*

---
#### Access Types
|C♭| C++ |
|--|--|
| `player score` | `private` |
| `conductor score` | `public` |
| `section score` | `protected` |

- `player score` makes the following attribute[^1] unreachable for outside use and inheriting classes.
- `conductor score` makes the following attribute[^1] reachable for outside use and inheriting classes.
- `section score` makes the following attribute[^1] unreachable for outside use but reachble inheriting classes.

[^1]: a field, a methode or a constructor.

---
#### Fields
`<access_type> <type> <name> [= <default_value>]`

---
#### Virtual Types
|C♭| C++ |
|--|--|
| `motif` | virtual |
| `variation` | override |
| `rest` | pure virtual[^2] |

[^2]: Making the class abstract and requiring derived classes to implement it or be abstact themself. Abstuct classes can't be created.

---
#### Methodes
`<access_type> [virtual_type] song [©𝄕func_name1, func_name2,...𝄇] <func_name>𝄕[arg1_type arg1_name], ...𝄇 [𝅘𝅥=return_type]`

- virtual_type is none in default, meaning a regular methode.

---
#### Constructors
`<access_type>` combining up bow[^3] ` <class_name>𝄕[arg1_type arg1_name], ...𝄇 [𝄍bass𝄕[base_arg1_name], ...𝄇]`

- bass is the parent constructor
- bass is the empty constructor by default

---
#### Static
Using the `unison` keyword before a field's type or a methode's virtual type makes the field/methode static.

- A static field is sheared with all the instances of the object.
- A static methode cant use field/methodes from that class.

---
### Usages of classes
#### Creating an instance
There are two ways to create an instance

1. `<class_name1> <name> [` combining up bow[^3] ` <class_name2>𝄕[arg1_name], ...𝄇]`.
2. combining up bow[^3] ` <class_name> <name>𝄕[arg1_name], ...𝄇`.

*note: In the first way `class_name2` must inherite or be `class_name1`.*

[^3]:https://graphemica.com/1D1AB

---
#### Geting a **non static** field/methode
- Field: `<var_name>𝄍<field>`
- Methode: `<var_name>𝄍<methode>𝄕[base_arg1_name], ...𝄇`
---

#### Geting a **static** field/methode
- Field: `<class_name>𝄍<field>`
- Methode: `<class_name>𝄍<methode>𝄕[base_arg1_name], ...𝄇`

---
### Other
#### toString𝄕𝄇
Each variable can do this operation thats turns it into a string, its also automatically called when adding a var to a string type.

You can call that function via: `<var>𝄍toString𝄕𝄇`

---
#### duration and sub duration (length)
Can be done only on array types.
Usage: `[sub ]duration𝄕<array_var>𝄇`

- `duration` is the size of the array (size 5 means array have five items, index are from 0 to 4)

- `sub duration` is the negative size of the array (size -6 means array have five items, index are from -1 to -5)

---
#### Timbre (is)
`<var> timbre <type>` will return true if `var` is `type` or inheriting it.

For this example `AA` inherits from `BB` and: 

`AA a =` combining up bow[^3] `AA𝄕𝄇`

`BB b1 =` combining up bow[^3] `AA𝄕𝄇`

`BB b2 =` combining up bow[^3] `BB𝄕𝄇`


- `a timbre AA` is true
- `b1 timbre AA` is true
- `b2 timbre AA` is false

---
#### transcribe (cast)
`transcribe𝆒<type>𝆓𝄕<expr>𝄇`

This will cast `expr` to `type`. For example `AA` inherits from `BB` and: 

`AA a =` combining up bow[^3] `AA𝄕𝄇`

`BB b1 =` combining up bow[^3] `AA𝄕𝄇`

`BB b2 =` combining up bow[^3] `BB𝄕𝄇`

- `transcribe𝆒<AA>𝆓𝄕a𝄇` will compile and run.
- `transcribe𝆒<BB>𝆓𝄕a𝄇` will compile and run.
- `transcribe𝆒<degree>𝆓𝄕a𝄇` won't compile.
- `transcribe𝆒<AA>𝆓𝄕b1𝄇` will compile and run.
- `transcribe𝆒<AA>𝆓𝄕b2𝄇` will compile **but will crush while running**.

 ---
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
 ---