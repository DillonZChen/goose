# This CMakeLists.txt file is only for use with C++ projects and not used by setup.py
# See also https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html
cmake_minimum_required(VERSION 3.20)
project(wlplan)

# make cache variables for install destinations
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic -fPIC -O3 -DNDEBUG -fomit-frame-pointer")

# Gather source files
# Do NOT glob src/main.cpp because that is only for creating Python bindings and requires pybind11
file(GLOB_RECURSE SRC_FILES "src/**/*.cpp")

# Define the library target
add_library(wlplan ${SRC_FILES})

# Add compile definitions
target_compile_definitions(wlplan PRIVATE WLPLAN_VERSION="${WLPLAN_VERSION}")

# Specify include directories for the target
target_include_directories(wlplan PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Export the targets to a file
install(TARGETS wlplan
    EXPORT wlplanTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install header files
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Create and install the package configuration file
include(CMakePackageConfigHelpers)

# Generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/wlplanConfigVersion.cmake"
    VERSION "${WLPLAN_VERSION}"
    COMPATIBILITY ExactVersion
)

# Create config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/wlplanConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wlplan"
)

# Install the config files
install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/wlplanConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/wlplanConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wlplan"
)

# Export and install the targets
install(
    EXPORT wlplanTargets
    FILE wlplanTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wlplan"
)
