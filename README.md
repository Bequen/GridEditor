# Grid

Grid is yet simple voxel editor. Can do basic things like painting, adding, substracting, shapes etc.

### But why?

Well, I wanted something open source, which Magica Voxel is not, and Goxel has weird controls :D
I tried making the controls similar to Blender, meaning `Ctrl`/`Shift`/`Alt` + `MMB` to move around, `Ctrl` + `Z` for undo etc.

### Feathures

Basic voxel editing.

### How to compile

Project should be pretty self-contained, meaning all the external libs are packed with it, but, I might have some lib preinstalled in system dir and therefore I didn't notice it's needed, PLEASE, notify me if you find any (your compilation won't go good).

Libs that are not included:
OpenGL:
- libGL
- libOpenGL
XLib:
- libX11
- libXxf86vm
- libXrandr
Other:
- dl
- libpthread

Compilation:
`make debug` for debug release, quick result with gdb support and outputting.
`make release` fully optimized version, use in case you won't be editing the source code.