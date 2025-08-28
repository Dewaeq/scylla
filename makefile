.PHONY: all build clean

all: build

build:
	cmake -S . -B build
	cmake --build build -j$(shell nproc)

clean:
	rm -rf build

