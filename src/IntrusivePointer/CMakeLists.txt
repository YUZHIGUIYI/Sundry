file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h")
add_executable(IntrusivePointer ${SourceFiles})
target_include_directories(IntrusivePointer PUBLIC ${CMAKE_CURRENT_LIST_DIR})

set_target_properties(IntrusivePointer PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin")
set_target_properties(IntrusivePointer PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin")