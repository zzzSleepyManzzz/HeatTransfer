# Heat Transfer Simulation

A modular C++ project for solving heat transfer problems using iterative numerical methods.

## Project Structure

- `libs/` – Core libraries (Solvers, Simulation logic, Core utilities)
- `apps/` – Executable applications
- `tests/` – Unit and integration tests
- `build/` – Build output (ignored)

## Dependencies

- C++23 compatible compiler (GCC / Clang)
- CMake >= 3.20
- Conan 2.x
- Ninja

## Build Instructions

### 1. Install dependencies
```bash
conan install . --output-folder=build --build=missing -s build_type=Release
```

### 2. Configure project
```bash
cmake --preset release
```

### 3. Build
```bash
cmake --build --preset release
```

## Run Tests

```bash
ctest --preset conan-release
``` 