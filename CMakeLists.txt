cmake_minimum_required(VERSION 3.10)
project(AutomataSimulator VERSION 1.0 DESCRIPTION "DFA, PDA, NPDA Simulator Suite")

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")
endif()

# Platform-specific settings for GUI
if(WIN32)
    add_definitions(-DGLEW_STATIC -DWIN32_LEAN_AND_MEAN)
elseif(UNIX AND NOT APPLE)
    add_definitions(-D_DEFAULT_SOURCE -DGL_GLEXT_PROTOTYPES)
endif()

# GUI Application (requires OpenGL, GLFW, GLEW)
option(BUILD_GUI "Build GUI application" ON)

if(BUILD_GUI)
    find_package(PkgConfig REQUIRED)
    find_package(OpenGL REQUIRED)
    
    pkg_check_modules(GLFW REQUIRED glfw3)
    find_package(GLEW REQUIRED)
    
    add_executable(gui GUI.c)
    target_include_directories(gui PRIVATE ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
    target_link_libraries(gui ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})
    
    # Platform-specific linking
    if(WIN32)
        target_link_libraries(gui gdi32 user32 kernel32)
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(gui m dl)
        find_package(Threads REQUIRED)
        target_link_libraries(gui Threads::Threads)
    elseif(APPLE)
        find_library(COCOA_LIBRARY Cocoa)
        find_library(IOKIT_LIBRARY IOKit)
        find_library(COREVIDEO_LIBRARY CoreVideo)
        target_link_libraries(gui ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
    endif()
    
    set_target_properties(gui PROPERTIES OUTPUT_NAME "AutomataSimulator")
endif()

# Command-line Applications (no external dependencies)
add_executable(dfa dfa.c)
add_executable(pda pda.c)
add_executable(npda npda.c)

# Print build information
message(STATUS "=== Automata Simulator Build Configuration ===")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "Build GUI: ${BUILD_GUI}")
if(BUILD_GUI)
    message(STATUS "OpenGL: ${OPENGL_LIBRARIES}")
    message(STATUS "GLFW: ${GLFW_LIBRARIES}")
    message(STATUS "GLEW: ${GLEW_LIBRARIES}")
endif() 