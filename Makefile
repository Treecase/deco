all: release

release: libdeco.so

debug:
	cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build/
	cmake --build build/ --config Debug

libdeco.so:
	cmake -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=0 -B build/
	cmake --build build/ --config Release

clean:
	rm -r ./build

.PHONY: all debug release clean
