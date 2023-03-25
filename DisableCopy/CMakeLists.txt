file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "src/*.cpp" "sre/*.h")
add_executable(DisableCopy ${SourceFiles})
target_include_directories(DisableCopy PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)