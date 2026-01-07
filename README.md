# Leetcode I/O Helper
## Introduction
This is a tool that can automatically generate the Input/Output logic based on a LeetCode's Solution class.

Supported Language(s): ```cpp```
## Installation script
### Linux
```
curl -fsSL https://raw.githubusercontent.com/TLR2025/Leetcode-I-O-Helper/main/scripts/linux.sh | bash
```
### Windows
```
curl -sSL "https://raw.githubusercontent.com/TLR2025/Leetcode-I-O-Helper/main/scripts/windows.bat" -o "%TEMP%\windows.bat" && cmd /c "%TEMP%\windows.bat"
```
## Usage
```
lioh [OPTIONS]
```
```
OPTIONS:
  -h,     --help              Print this help message and exit 
  -s,     --source FILEPATH REQUIRED 
                              The path of the source file. 
  -o,     --output FILEPATH   The path of the output file (default: original file). 
  -l,     --lang LANGUAGE     The language of the source (default: automatic inference). 
  -v,     --version           Display program version information and exit
```
**The generated code will include input and output logic. You can directly copy Leetcode's JSON-like data and paste it into your terminal.**
## Third-party library
[CLI11](https://github.com/CLIUtils/CLI11)
[Cpp-Inquirer](https://github.com/aelliixx/cpp-inquirer)
