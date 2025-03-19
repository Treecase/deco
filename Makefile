all: libdeco.so

debug:
	cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build/
	cmake --build build/ --config Debug

release: libdeco.so

libdeco.so:
	cmake -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build/
	cmake --build build/ --config Release

clean:
	rm -r ./build

.PHONY: all debug release clean
