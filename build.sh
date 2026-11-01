
conan install . --profile=./conan_profile --output-folder=build --build=missing -s build_type=Release

cmake --preset release
cmake --build --preset release
