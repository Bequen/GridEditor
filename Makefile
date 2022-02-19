# This is universal Makefile that can be adapted to high amouth of repositories
# Please, try to not edit this file, edit Make.conf instead

PROJECT_NAME=SuperSecretProject

CXX=/usr/bin/clang++
CC=/usr/bin/clang

# WARNING: This afaik is only valid version for clang, use c++2a for gcc/g++
CXXVERSION=c++20
CVERSION=c11

# Some programs
LS=/usr/bin/ls

# Includes our config file
include ./Make.conf

# C++
CXXFLAGS+=-std=${CXXVERSION}
CXXDBGFLAGS+=${CPP_FLAGS} -O0 -g3 -ggdb -DDEBUG

# C
CFLAGS+=
CDBGFLAGS+=${CFLAGS} -O0 -g3 -ggdb -DDEBUG


# Lists source files
SOURCE_C=$(foreach dir, $(addprefix ${SOURCE_DIR}, ${FILES_SUB_DIRS}), ${filter-out ${IGNORE}, $(wildcard $(dir)/*.c)}) ${filter-out ${IGNORE}, $(wildcard $(SOURCE_DIR)/*.c)}
SOURCE_CPP=$(foreach dir, $(addprefix ${SOURCE_DIR}, ${FILES_SUB_DIRS}), $(wildcard $(dir)/*.cpp)) $(wildcard $(SOURCE_dir)/*.cpp) ./src/main.cpp

# All the .cpp and .c files
SOURCE=${SOURCE_C} ${SOURCE_CPP}
# $(addprefix ${SOURCE_DIR}, ${FILES})
# All the header files
INCLUDE=$(foreach dir, $(addprefix ${INCLUDE_DIR}/${PROJECT_NAME}, ${FILES_SUB_DIRS}), $(wildcard $(dir)/*.h))



# Libs
LIB_INCLUDE=$(addprefix -I${THIRD_PARTY}, ${LIB_INCLUDE_DIRS}) -I${INCLUDE_DIR} -I${THIRD_PARTY}
# Where to look for libraries
LIB_FILES=$(addprefix -L${THIRD_PARTY}, ${LIB_DIRS}) -L${LIB_DIR}
# Libraries to use, each file must be in format 'lib{name}.so' or 'lib{name}.dll'
DEFS+=-DPROJECT_NAME=\"${PROJECT_NAME}\"


ODIR=./obj/
OBJ=$(subst ${SOURCE_DIR}, ${ODIR}, $(filter %.o, $(SOURCE:%.c=%.o) $(SOURCE:%.cpp=%.o)))
COMPILE_CMDS=$(subst .o,.o.json,${OBJ})
CPP_DEPENDENCIES=$(subst ${SOURCE_DIR}, ${ODIR}, $(filter %.d, $(SOURCE:%.cpp=%.d)))
C_DEPENDENCIES=$(subst ${SOURCE_DIR}, ${ODIR}, $(filter %.d, $(SOURCE:%.c=%.d)))


# Include our dependencies
-include ${C_DEPENDENCIES} ${CPP_DEPENDENCIES}



# Final build targets
.PHONY: debug
debug: ${OBJ} ./third-party/glad.o
	@echo -e "\033[0;34mCompiling ${PROJECT_NAME} $^\033[0m"
	@${CXX} ${CXXFLAGS} ${CXXDBGFLAGS} ${DEFS} -o ${DIR}${PROJECT_NAME} $^ ${DEPS} ${LIB_FILES} ${LIB_INCLUDE}
	@echo -e "\033[0;32mCompiling finished\033[0m"

.PHONY: release
release: ${OBJ}
	@echo -e "\033[0;35mCreating release build\033[0m"
	@echo -e "\033[0;34mCompiling ${PROJECT_NAME}\033[0m"
	@${CPP_COMPILER} ${CPP_FLAGS} ${DEFS} -o ${DIR}${PROJECT_NAME} $^ ${DEPS} ${LIB_FILES} ${LIB_INCLUDE}
	@echo -e "\033[0;32mCompiling finished\033[0m"

.PHONY: library
library: lib${PROJECT_NAME}.so



# Final targets
.PHONY: test
test: ${PROJECT_NAME}


${PROJECT_NAME}: debug
	@GTK_DEBUG=interactive ./${PROJECT_NAME}

.PHONY: build
build:
	@mkdir -p build
	@${MAKE} DIR=./build/ release
	@cp -r assets ./build/

${CPP_DEPENDENCIES}: ${ODIR}%.d: ${SOURCE_DIR}%.cpp
	@mkdir -p $(dir $@)
	@${CXX} $(CXXFLAGS) ${CXXDBGFLAGS} ${DEFS} ${LIB_INCLUDE} -MM -MT '${subst .d,.o, $@}' $< -MF $@;

${C_DEPENDENCIES}: ${ODIR}%.d: ${SOURCE_DIR}%.c
	@mkdir -p $(dir $@)
	@${CC} ${CFLAGS} ${DEFS} ${LIB_INCLUDE} -MM -MT '${subst .d,.o, $@}' $< -MF $@;



# FILES
%.o:
	@echo ${SOURCE} ${OBJ}
	@echo -e "\033[0;34mCompiling $< for $@\033[0m"
	@${CXX} ${CXXFLAGS} ${CXXDBGFLAGS} ${DEFS} ${DEPS} ${LIB_INCLUDE} -c -o $@ $<
	@echo -e "\033[0;32mCompiling $< for $@ was successfull\033[0m"

lib${PROJECT_NAME}.so: ${OBJ}
	@echo Creating $@ static library
	@ar rvs -c lib${PROJECT_NAME}.a $^
	@echo Creating $@ is complete

${COMPILE_CMDS}: ${ODIR}%.o.json: ${SOURCE_DIR}%.cpp
	@clang -MJ $@ ${CXXFLAGS} ${CXXDBGFLAGS} ${DEPS} ${DEFS} ${LIB_INCLUDE} -o ${subst .o.json,.o,$@} -c $<


# Compilation database
compile_commands.json: ${COMPILE_CMDS}
	@echo "Creating $@"
	@rm -f ./compile_commands.json
	@$(foreach file,$(COMPILE_CMDS),`cat ${file} >> compile_commands.json.tmp`)
	@echo "[" >> compile_commands.json
	@cat compile_commands.json.tmp | sed '$$s/,$$//' >> compile_commands.json
	@echo "]" >> compile_commands.json
	@rm -f ./compile_commands.json.tmp
	@echo "Done"


# CLEANING
.PHONY: clean
clean:
	@echo "Cleaning up"
	@rm -f -r ./obj/
	@rm -f -r ./build/
	@rm -f ./${PROJECT_NAME}
	@rm -f ./lib${PROJECT_NAME}.a
	@echo "Finished cleaning"

.PHONY: clean_build
clean_build:
	@rm -f -r ./build/
	@rm -f ./${PROJECT_NAME}
	@rm -f ./lib${PROJECT_NAME}.a
