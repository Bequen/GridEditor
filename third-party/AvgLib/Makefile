PROJECT_NAME=AvgLib
CPP_COMPILER=g++
CPP_FLAGS=-std=c++2a -DDEBUG

SOURCE=$(wildcard ./src/*.cpp) $(wildcard ./src/Math/*.cpp) $(wildcard ./src/Random/*.cpp) $(wildcard ./src/Terminal/*.cpp)
INCLUDE=$(wildcard ./include/avg/*.h) $(wildcard ./include/avg/Math/*.h) $(wildcard ./include/avg/Random/*.h) $(wildcard ./src/Terminal/*.h)

OBJ=${SOURCE:.cpp=.o}

lib${PROJECT_NAME}.a: ${OBJ}
	@ar rvs lib${PROJECT_NAME}.a $^

.PHONY: test
test: ${OBJ} main.cpp
	@${CPP_COMPILER} ${CPP_FLAGS} -O0 -g3 -ggdb -DDEBUG -o $@ $^ -I./include/
${OBJ} : %.o : %.cpp ${INCLUDE}
	@${CPP_COMPILER} ${CPP_FLAGS} -c -o $@ $< -I./include/

.PHONY: clean
clean:
	@rm ${OBJ}