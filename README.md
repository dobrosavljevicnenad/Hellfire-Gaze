# Dagger

A fully-featured, modern game engine made for educational purposes.

## Features

- Dagger is data-driven and event-based.
- Dagger is extremely modular.
- Dagger is clear and clearly educational.

  



https://github.com/igorkandic/UbiBelPMF_2024/assets/10564596/8ca6d072-ee93-41b5-b7ea-a2f1a431f04a





## Installation

You can clone and fully set up this project by simply running:

```git
  git clone https://github.com/igorkandic/UbiBelPMF_2024.git
```

### Conan 
Conan is used for dependecies
#### For Ubuntu
```shell
pip install conan
```
#### Or
[Download](https://conan.io/downloads) conan from site.
### Conan simpleini
Official conan repository doesn't have [simpleini](https://github.com/brofield/simpleini).
##### Building simpleini
```shell
cd deps
conan create .
```

### Build from terminal
```shell
  mkdir build
  cd build
  conan install .. --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
  cd ..
  cmake --preset conan-release
  cd build/Release
  make
```

### Using CLion
#### CLion conan extension:
[Download extension](https://www.jetbrains.com/help/clion/conan-plugin.html)

## Resources

Any kind of data used by engine that is not source code is considered a resource. Root directory for resources is `data\`
and so all references to resoruces begin there. For example, if you want to get a texture (from `data\textures`) you would
use `textures\mytexture.png` instead `data\textures\mytexture.png`.


## Assets used

### Sound
- https://fallenblood.itch.io/50-sfx 
- https://kronbits.itch.io/freesfx 
- https://xdeviruchi.itch.io/8-bit-fantasy-adventure-music-pack 
- https://jdwasabi.itch.io/8-bit-16-bit-sound-effects-pack 
- https://www.youtube.com/watch?v=195XntreoMc 

### Sprites
- https://www.spriters-resource.com/pc_computer/touhoufuujinrokumountainoffaith/sheet/51905/

## Rights and Reservations

Dagger is made as a part of Ubisoft Belgrade's game development course. All rights reserved.
