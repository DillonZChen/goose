""" Build script from chatgpt """
import subprocess
import os

# Set the path to your project's root directory
project_directory = "."

# Create a build directory if it doesn't exist
build_directory = os.path.join(project_directory, "build")
os.makedirs(build_directory, exist_ok=True)

# Step 1: Configure CMake
cmake_configure_command = ["cmake", ".."]
cmake_configure_process = subprocess.Popen(cmake_configure_command, cwd=build_directory)
cmake_configure_process.wait()

# Step 2: Build with Make
make_command = ["make"]
make_process = subprocess.Popen(make_command, cwd=build_directory)
make_process.wait()
