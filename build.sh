
conan install . --output-folder=build --build=missing -s build_type=Release

cmake --preset release
cmake --build --preset release
