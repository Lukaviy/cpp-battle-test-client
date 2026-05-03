# Build Instructions

## Building without tests (default)

By default, tests are **disabled** to speed up build times and avoid unnecessary dependencies.

```bash
cmake -B build
cmake --build build
```

## Building with tests

To enable tests, set the `BUILD_TESTS` option to `ON`:

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
```

### Running tests

After building with tests enabled:

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test
ctest --test-dir build -R "test_name" --verbose

# List all available tests
ctest --test-dir build -N
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | `OFF` | Enable building of unit tests with Catch2 v3 |

## Visual Studio

### Method 1: Using CMake Settings UI

1. Open the project in Visual Studio
2. Go to **Project > CMake Settings**
3. Find **CMake command arguments**
4. Add `-DBUILD_TESTS=ON`
5. Save and reconfigure

### Method 2: Using CMakeSettings.json

You can use the provided example configuration:

1. Copy `CMakeSettings.json.example` to `CMakeSettings.json`
2. Select the configuration with tests: `x64-Debug-WithTests` or `x64-Release-WithTests`
3. Visual Studio will automatically reconfigure

The example provides 4 configurations:
- `x64-Debug` - Debug build without tests
- `x64-Debug-WithTests` - Debug build with tests
- `x64-Release` - Release build without tests
- `x64-Release-WithTests` - Release build with tests

### Running tests in Visual Studio

After building with tests enabled, you can:

1. **Test Explorer**: View > Test Explorer (tests will appear automatically)
2. **CTest**: Use the command line from the Visual Studio Developer PowerShell
   ```powershell
   ctest --test-dir out\build\x64-Debug-WithTests --output-on-failure
   ```

## Why tests are optional?

- **Faster builds**: Catch2 doesn't need to be downloaded if you're not running tests
- **CI/CD friendly**: Production builds don't need test dependencies
- **Developer choice**: Only enable tests when you need them
