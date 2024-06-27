cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(Sundry LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

add_subdirectory(src/TypeErasure)
add_subdirectory(src/VisitorDesignPattern)
add_subdirectory(src/ObjectClone)
add_subdirectory(src/DisableCopy)
add_subdirectory(src/MiniLog)
add_subdirectory(src/MemoryResourceInspector)
add_subdirectory(src/Profiler)
add_subdirectory(src/ThreadPool)
add_subdirectory(src/MultiThreadPool)
add_subdirectory(src/Object)
add_subdirectory(src/Sparse)
add_subdirectory(src/ObjectPool)
add_subdirectory(src/Callable)
add_subdirectory(src/IntrusivePointer)
add_subdirectory(src/SubscribeMode)
add_subdirectory(src/MultiThreadQueue)
add_subdirectory(src/FactoryPattern)
add_subdirectory(src/Signal)