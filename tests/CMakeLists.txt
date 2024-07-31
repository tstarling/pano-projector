find_package (Python3 COMPONENTS Interpreter REQUIRED)

add_test(
        NAME "Integration tests"
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/integration-tests.py
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_BINARY_DIR}
)