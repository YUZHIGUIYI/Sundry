file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h")
add_executable(Sparse ${SourceFiles})
target_include_directories(Sparse PUBLIC ${CMAKE_CURRENT_LIST_DIR})

find_package(OpenMP)
if (OpenMP_FOUND)
    target_link_libraries(Sparse PUBLIC OpenMP::OpenMP_CXX)
else ()
    message(STATUS "OpenMP not found")
endif ()

set_target_properties(Sparse PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin")
set_target_properties(Sparse PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin")