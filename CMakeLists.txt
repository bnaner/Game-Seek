cmake_minimum_required(VERSION 3.15)
project(Game-Seek)

# Include vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "C:/Users/saysa/.vscode/Game-Seek/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Define the executable
add_executable(Game-Seek src/main.cpp src/GameBase.cpp src/GameBase.h src/Node.h)

# Find packages
find_package(asio REQUIRED)
find_package(Crow CONFIG REQUIRED)

# Link Crow
target_link_libraries(Game-Seek PRIVATE Crow::Crow)

