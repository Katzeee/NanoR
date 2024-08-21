## PBR + IBL

```bash
$ git checkout pre-learning
```

![alt text](/doc/24-ibl.png)

## nanoR(WIP)

![alt text](doc/nanoR.png)

Using `shaderc` and `spirv-cross` to dynamically compile shaders and reflect to generate uniform descriptors, making it convenient to directly integrate with Vulkan in the future and automatically generate corresponding parameters in the material panel.

## Build

Windows should use `MSVC_2022_amd64_x86` MSVC to compile.

First build may consume lots of time, resulting of downloading and building dependencies.