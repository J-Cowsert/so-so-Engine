add_library(soso-doctest INTERFACE)
target_include_directories(soso-doctest SYSTEM INTERFACE
    "${PROJECT_SOURCE_DIR}/so-so/vendor/doctest"
)
target_compile_definitions(soso-doctest INTERFACE DOCTEST_CONFIG_USE_STD_HEADERS)

add_executable(soso-core-tests
    TestMain.cpp
    UIDTests.cpp
    LayerTests.cpp
    ResourceTests.cpp
)
target_link_libraries(soso-core-tests PRIVATE soso-core soso-doctest)
target_compile_options(soso-core-tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-Wextra;-Wpedantic>
)
set_target_properties(soso-core-tests PROPERTIES COMPILE_WARNING_AS_ERROR ON)

add_test(NAME soso.core COMMAND soso-core-tests)
set_tests_properties(soso.core PROPERTIES LABELS "unit;core" TIMEOUT 30)
