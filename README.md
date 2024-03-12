## PBR + IBL

![alt text](/doc/24-ibl.png)

## nanoR(WIP)

![alt text](doc/nanoR.png)

## Build

Windows should use `amd64_x86` MSVC to compile.

You may get an error when running cmake configure for shaderc, you should download its dependency manually:

```
[cmake] CMake Error at build/_deps/shaderc-src/third_party/CMakeLists.txt:80 (message):
[cmake]   SPIRV-Tools was not found - required for compilation
```

```bash
cd ./build/_deps/shaderc-src
python ./utils/git-sync-deps
```