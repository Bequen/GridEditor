# Name of the project
PROJECT_NAME=GridEditor

# Directory of the .cpp and .h files.
# Use both the same if you don't divide and have all in one
SOURCE_DIR=src/
INCLUDE_DIR=src/
LIB_DIR=deps/

# If your source files have some sub directories, specify them here 
FILES_SUB_DIRS=System System/Tiles Rendering Editor ImGUI ImGUI/GL Editor/Tiles System/Voxels

# Where libraries, other than those in /usr/lib64/ are put
THIRD_PARTY=./third-party/
# All the dependencies that are in use
DEPS=dl GL OpenGL GLU X11 Xxf86vm Xrandr pthread gdiplus AvgLib glfw3
# Where should we look for the .so files in sub projects
LIB_DIRS=AvgLib glfw/src glm/glm
# Where we can find include files in these libs
LIB_INCLUDE_DIRS=glad glm glfw/include AvgLib/include

# Other files that should be compiled into .o file
FILES=main.cpp glad.c

CPP_FLAGS=-DCHUNK_SIZE=32 -DCHUNK_HEIGHT=128 -DMAX_DYNAMIC_LIGHT_COUNT=32

# Set default target, otherwise install would be default, which is kinda bullshit, right?
.DEFAULT_GOAL:= test

# Custom targets can be declared here
# Watch out, because they are going to be over the ones defined in Makefile
install:
	cd ${THIRD_PARTY}/AvgLib; make;