.PHONY = run build libs

CC = g++
CPPFLAGS = -I./src/ -Itypes/ -std=c++20 -O3 -flto
LINKER_FLAGS = -lSDL2 -ldl -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lpthread -fcoroutines

ifeq ($(OS),Windows_NT)
SRCS=$(shell cd src & dir /b /s *.cpp) $(shell cd libs & dir /b /s *.cpp)
LINKER_FLAGS += -w -Wl,-subsystem,console -isystem C:/mingw64/include/SDL2/
else
SRCS=$(shell find src/ -type f -name '*.cpp') $(shell find libs/ -type f -name '*.cpp')
LINKER_FLAGS += -lGL -lX11 -lrt
endif

OBJS=$(SRCS:.cpp=.o)

build : libs
	$(CC) $(SOURCE)generated.cpp -o $(SOURCE)generated.out $(CPPFLAGS) $(OBJS) $(LINKER_FLAGS)

# compile and run a file
run : build
	cd $(SOURCE) && generated.out

# compile threespeed and external libraries
libs : $(OBJS)
	@:

benchmark : all
	time ./main benchmark

clean :
ifeq ($(OS),Windows_NT)
	del /S *.o /Q
else
	find . -name "*.o" -type f -delete
endif

main.o : main.cpp
	@:

test.o : test.cpp
	@:

test1.o : test1.cpp
	@:

%.o : %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@ $(LINKER_FLAGS)