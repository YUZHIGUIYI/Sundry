file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "src/*.cpp" "sre/*.h")
add_executable(ObjectClone ${SourceFiles})
target_include_directories(ObjectClone PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)