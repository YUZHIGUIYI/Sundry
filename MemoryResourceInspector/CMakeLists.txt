file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "src/*.cpp" "sre/*.h")
add_executable(ResourceInspector ${SourceFiles})
target_include_directories(ResourceInspector PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

set_target_properties(ResourceInspector PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin")
set_target_properties(ResourceInspector PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin")