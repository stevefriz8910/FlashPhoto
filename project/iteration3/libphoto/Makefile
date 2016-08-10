CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst src/%.cpp,build/%.o,$(CPP_FILES))
CXXFLAGS = -c -Wall

all: prereqs lib/libphoto.a

lib/libphoto.a: $(OBJ_FILES)
	ar rcs $@ $^

build/%.o: src/%.cpp
	g++ $(CXXFLAGS) -o $@ $<

prereqs:
	mkdir -p lib
	mkdir -p build

.PHONY: prereqs

clean:
	rm  $(OBJ_FILES) lib/libphoto.a
