# format/CMakeLists.txt
# Tests del formato .aex

add_executable(test_aex_roundtrip tests/test_roundtrip.c)
target_link_libraries(test_aex_roundtrip PRIVATE arx_common)
add_test(NAME aex_roundtrip COMMAND test_aex_roundtrip)

add_executable(test_aex_sign tests/test_sign.c)
target_link_libraries(test_aex_sign PRIVATE arx_common)
add_test(NAME aex_sign COMMAND test_aex_sign)
