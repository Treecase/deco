all: release

release: libdeco.so

debug:
	cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build/
	cmake --build build/ --config Debug -j`nproc 2>/dev/null || getconf NPROCESSORS_CONF`

libdeco.so:
	cmake -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=0 -B build/
	cmake --build build/ --config Release -j`nproc 2>/dev/null || getconf NPROCESSORS_CONF`

clean:
	rm -r ./build

.PHONY: all debug release clean
