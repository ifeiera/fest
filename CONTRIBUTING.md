# Contributing to FEST

Thank you for your interest in contributing to the FEST (Fast & Efficient System Tracking) project! This document provides guidelines and instructions to help you contribute effectively.

## Project Structure

FEST follows a standardized directory structure:

```
fest/
├── include/       # Header files (.h)
├── src/           # Implementation files (.c)
├── test/          # Test files
├── CMakeLists.txt # Main CMake configuration
└── ...
```

## Setting Up the Development Environment

1. **Prerequisites**:

   - CMake (version 3.10 or higher)
   - A C compiler (MSVC recommended for Windows)
   - Git

2. **Clone the repository**:

   ```bash
   git clone https://github.com/ifeiera/fest.git
   cd fest
   ```

3. **Build the project**:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

## Contributing Guidelines

### Adding New Features

1. **Header Files**:

   - All new header files should be placed in the `include/` directory
   - Use include guards or `#pragma once` to prevent multiple inclusions
   - Document public functions with clear comments

2. **Implementation Files**:

   - All source files should be placed in the `src/` directory
   - Follow the existing code style and naming conventions
   - Implement proper error handling and memory management

3. **Update CMakeLists.txt**:
   - Add new source files to the appropriate target in `CMakeLists.txt`
   - Check for any dependencies that might need to be added

### Writing Tests

1. **Test Files**:

   - Add test files in the `test/` directory
   - Name test files clearly, indicating what they test
   - Write comprehensive tests that cover both normal and edge cases

2. **Update Test CMakeLists.txt**:
   - Add your test files to `test/CMakeLists.txt`
   - Ensure your tests are included in the test suite

### Example: Adding a New System Metric

If you want to add a new system metric (e.g., temperature sensors):

1. Create a header file: `include/temperature_info.h`
2. Create an implementation file: `src/temperature_info.c`
3. Add the new source file to `CMakeLists.txt`
4. Create tests in `test/test_temperature_info.c`
5. Update `test/CMakeLists.txt` to include your new test

## Pull Request Process

1. **Fork the repository** and create a new branch for your feature
2. **Make your changes** following the guidelines above
3. **Run the tests** locally to ensure everything works
   ```bash
   cd build
   ctest
   ```
4. **Submit a pull request** with a clear description of the changes
5. **Address any feedback** during the review process

## Automatic Testing

This repository has GitHub Actions configured to automatically run tests on pull requests. Before submitting a PR, make sure your code passes all tests locally to avoid delays in the review process.

## Code Style Guidelines

- Use consistent indentation (4 spaces recommended)
- Keep lines at a reasonable length (< 100 characters)
- Use clear, descriptive variable and function names
- Comment complex logic, but let clear code speak for itself
- Avoid memory leaks by properly freeing allocated resources

## Memory Management

As FEST deals with system information collection that runs for extended periods, proper memory management is critical:

- Always free resources in the reverse order they were allocated
- Use helper functions to encapsulate allocation and freeing
- Consider using a memory tracking system during development
- Be careful with string operations to avoid buffer overflows

## Additional Notes

- Always review the `CMakeLists.txt` files before making changes
- The project is designed to be lightweight - avoid unnecessary dependencies
- When implementing WMI queries, ensure proper COM initialization and cleanup
- When adding new JSON fields, maintain backward compatibility if possible

Thank you for contributing to FEST!
