## AvgLib

AvgLib, stands for average library, is simple C++ library with feathures like Perlin Noise, File Watcher and Strings. My main focus is performance whilst delivering easy interface.

### Feathures
- [x] Perlin Noise
- [x] String management interface
- [x] File Watcher
- [x] Simple Content Pipeline
- [x] More noise functions!
- [ ] Noise Fractals

## How to Use?

AvgLib is built to be used directly in project and being compiled alongside it. So, usually you will want to create `third-party` directory in your project files, place the source code of **AvgLib** there and in your **Makefile** use `$(MAKE) path/to/AvgLib/Makefile`.

Also, do not forget to include the directory with `-I./dir/to/AvgLib/include/` and link the library with `-lAvgLib`, to tell where to find the static link library (.a file), use `-L./dir/to/AvgLib/`.

The resulting compile command should then look like this:
`${CXX_COMPILER} ${CXX_FLAGS} -o ${PROJECT_NAME} -L./dir/to/AvgLib/ -I./dir/to/AvgLib/include/ -lAvgLib`

In your C++ code including <avg/*.h> should do the job.