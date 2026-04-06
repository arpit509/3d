cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("FPS_Shooter")
set(CMAKE_CXX_STANDARD 20)

# Auto-collect all .cpp files recursively
file(GLOB_RECURSE SOURCES "*.cpp")

# Exclude CMake's own build folder (contains CMakeCXXCompilerId.cpp etc.)
list(FILTER SOURCES EXCLUDE REGEX ".*/out/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/CMakeFiles/.*")

message(STATUS "Sources found: ${SOURCES}")

include_directories(C:/raylib/include)
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/bullet
    ${CMAKE_SOURCE_DIR}/enemy
    ${CMAKE_SOURCE_DIR}/level
    ${CMAKE_SOURCE_DIR}/player
    ${CMAKE_SOURCE_DIR}/title
)

link_directories(C:/raylib/lib)

add_executable(FPS_Shooter ${SOURCES})

target_link_libraries(FPS_Shooter raylib.lib opengl32.lib gdi32.lib winmm.lib)