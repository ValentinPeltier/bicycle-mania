## Installation

*No release is available yet, you will have to compile it.*

## Development

This project use Premake to generate build configuration files for your preferred build system, including Visual Studio, GNU Make and Xcode.

### Prerequisites

- [Premake 5.0](https://premake.github.io/download)
- [Vulkan SDK 1.4](https://vulkan.lunarg.com/sdk/home)
- [GLM 1.0](https://glm.g-truc.net)
- [GLFW 3.4](https://www.glfw.org)

### Build

#### Linux

1. Generate Make files: `premake5 gmake`
1. Build the project: `make` (or `make config=release` to build the release version)

#### Windows

1. Generate Visual Studio files: `premake5.exe vs2022` (or another `vs...` for another Visual Studio version)
1. Open the generated `.sln` file with Visual Studio, select your configuration and build

### Clang tools

For Clang tools to known the build configuration we are using, they need a `compile_commands.json` file.
To generate it, execute `premake5 ecc`.