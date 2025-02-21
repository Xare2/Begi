cmake_minimum_required(VERSION 3.24.0)

set (CMAKE_CXX_STANDARD 20)

project(Begi VERSION 1.0.0)

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

include_directories(src/api/include)
include_directories(SYSTEM "src/imgui")
include_directories(SYSTEM "libs/glad/inc/")

file(GLOB API_SRC "src/api/src/*.cpp")
file(GLOB IMGUI_SRC "src/imgui/*.cpp")

add_subdirectory(libs/glfw)

add_executable(Begi "src/main.cpp" ${API_SRC} ${IMGUI_SRC} "libs/glad/inc/glad/gl.h")

target_link_libraries(Begi Vulkan::Vulkan Vulkan::shaderc_combined)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

target_link_libraries(Begi glfw)

add_subdirectory(libs/pugixml)
target_link_libraries(Begi pugixml::pugixml)

# I think /Debug/data should be /data on linux
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
