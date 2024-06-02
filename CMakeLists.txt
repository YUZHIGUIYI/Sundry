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

add_subdirectory(TypeErasure)
add_subdirectory(VisitorDesignPattern)
add_subdirectory(ObjectClone)
add_subdirectory(DisableCopy)
add_subdirectory(MiniLog)
add_subdirectory(MemoryResourceInspector)
add_subdirectory(Profiler)
add_subdirectory(ThreadPool)
add_subdirectory(MultiThreadPool)
add_subdirectory(Object)
add_subdirectory(Sparse)
add_subdirectory(ObjectPool)
add_subdirectory(Callable)
add_subdirectory(IntrusivePointer)
add_subdirectory(SubscribeMode)