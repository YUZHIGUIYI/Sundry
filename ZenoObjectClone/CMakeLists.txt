file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "src/*.cpp" "sre/*.h")
add_executable(ZenoObjectClone ${SourceFiles})
target_include_directories(ZenoObjectClone PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)