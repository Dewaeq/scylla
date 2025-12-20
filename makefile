.PHONY: all build clean

all: build

build:
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
	mv build/compile_commands.json .
	cmake --build build -j$(shell nproc)

clean:
	rm -rf build
