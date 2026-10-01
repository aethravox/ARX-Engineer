# tools/arx_sign/CMakeLists.txt
add_executable(arx_sign main.c)
target_link_libraries(arx_sign PRIVATE arx_common)
