PROJECT_NAME=SuperSecretProject

CPP_COMPILER=g++
CPP_VERSION=c++2a

include ./Make.make

CPP_FLAGS+=-std=${CPP_VERSION}
CPP_DEBUG_FLAGS+=${CPP_FLAGS} -O0 -g3 -ggdb -DDEBUG -DMALLOC_CHECK_=2
CPP_DEFINITIONS+=-DPROJECT_NAME=\"${PROJECT_NAME}\" -DIMGUI_IMPL_OPENGL_LOADER_GLAD



# All the .cpp and .c files
SOURCE=$(foreach dir, $(addprefix ${SOURCE_DIR}, ${FILES_SUB_DIRS}), $(wildcard $(dir)/*.cpp)) $(addprefix ${SOURCE_DIR}, ${FILES})
# All the header files
INCLUDE=$(foreach dir, $(addprefix ${INCLUDE_DIR}/${PROJECT_NAME}, ${FILES_SUB_DIRS}), $(wildcard $(dir)/*.h))



# Libs
LIB_INCLUDE=$(addprefix -I${THIRD_PARTY}, ${LIB_INCLUDE_DIRS}) -I${THIRD_PARTY} -I${INCLUDE_DIR}
# Where to look for libraries
LIB_FILES=$(addprefix -L${THIRD_PARTY}, ${LIB_DIRS}) -L${LIB_DIR}
# Libraries to use, each file must be in format 'lib{name}.so' or 'lib{name}.dll'
DEPS:=$(addprefix -l, ${DEPS})



ODIR=obj/
OBJ=$(subst ${SOURCE_DIR}, ${ODIR}, $(filter %.o, $(SOURCE:%.cpp=%.o) $(SOURCE:%.c=%.o)))
CPP_DEPENDENCIES=$(subst ${SOURCE_DIR}, ${ODIR}, $(filter %.d, $(SOURCE:%.cpp=%.d)))
C_DEPENDENCIES=$(subst ${SOURCE_DIR}, ${ODIR}, $(filter %.d, $(SOURCE:%.c=%.d)))



-include ${C_DEPENDENCIES} ${CPP_DEPENDENCIES}



# Final build targets
.PHONY: debug
debug: ${OBJ}
	@echo -e "\033[0;34mCompiling ${PROJECT_NAME} $^\033[0m"
	@${CPP_COMPILER} ${CPP_DEBUG_FLAGS} ${CPP_DEFINITIONS} -o ${DIR}${PROJECT_NAME} $^ ${DEPS} ${LIB_FILES} ${LIB_INCLUDE} 
	@echo -e "\033[0;32mCompiling finished\033[0m"

.PHONY: release
release: ${OBJ}
	@echo -e "\033[0;35mCreating release build"
	@echo -e "\033[0;34mCompiling ${PROJECT_NAME}\033[0m"
	@${CPP_COMPILER} ${CPP_FLAGS} ${CPP_DEFINITIONS} -o ${DIR}${PROJECT_NAME} $^ ${DEPS} ${LIB_FILES} ${LIB_INCLUDE} 
	@echo -e "\033[0;32mCompiling finished\033[0m"

.PHONY: library
library: lib${PROJECT_NAME}.so



# Final targets
.PHONY: test
test: ${PROJECT_NAME}


${PROJECT_NAME}: debug
	./${PROJECT_NAME}

.PHONY: build
build:
	@mkdir -p build
	@${MAKE} DIR=./build/ release
	@cp -r assets ./build/

${CPP_DEPENDENCIES}: ${ODIR}%.d: ${SOURCE_DIR}%.cpp
	mkdir -p $(dir $@)
	@$(CPP_COMPILER) $(CPP_FLAGS) ${CPP_DEFINITIONS} ${LIB_INCLUDE} -MM -MT '$(subst .d,.o, $@)' $< -MF $@;

${C_DEPENDENCIES}: ${ODIR}%.d: ${SOURCE_DIR}%.c
	mkdir -p $(dir $@)
	$(CPP_COMPILER) $(CPP_FLAGS) ${CPP_DEFINITIONS} ${LIB_INCLUDE} -MM -MT '$(subst .d,.o, $@)' $< -MF $@;



# FILES
%.o:
	@echo -e "\033[0;34mCompiling $< for $@\033[0m"
	@${CPP_COMPILER} ${CPP_DEBUG_FLAGS} ${CPP_DEFINITIONS} ${LIB_INCLUDE} -c -o $@ $<
	@echo -e "\033[0;32mCompiling $< for $@ was successfull\033[0m"

lib${PROJECT_NAME}.so: ${OBJ}
	@echo Creating $@ static library
	@ar rvs -c lib${PROJECT_NAME}.a $^
	@echo Creating $@ is complete



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
