cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("3D")
set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "*.cpp")

include_directories(C:/raylib/include)
link_directories(C:/raylib/lib)

add_executable(3D ${SOURCES})

target_link_libraries(3D raylib.lib opengl32.lib gdi32.lib winmm.lib)