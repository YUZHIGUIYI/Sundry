file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
add_executable(WriteWatchGL ${SourceFiles})
target_include_directories(WriteWatchGL PUBLIC ${CMAKE_CURRENT_LIST_DIR})

set_target_properties(WriteWatchGL PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(WriteWatchGL PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)