
add_executable(test_pretty_print
    catch_main.cpp
    test_pretty_print_stl.cpp
)

target_link_libraries(test_pretty_print PUBLIC pretty_print)

# Gah, I hate CMake. Why is this required when we have pretty_print defined
# as an interface library?
target_include_directories(test_pretty_print PUBLIC ${pretty_print_SOURCE_DIR}/include)