
find_package(GTest REQUIRED)
find_program(GTEST_PARALLEL_EXECUTABLE gtest-parallel)

if(GTEST_PARALLEL_EXECUTABLE)
    execute_process(
            COMMAND nproc
            OUTPUT_VARIABLE NUM_PROCESSORS
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    add_test(NAME IntegrationTest
            COMMAND ${GTEST_PARALLEL_EXECUTABLE} $<TARGET_FILE:tests> --workers=${NUM_PROCESSORS} --gtest_color=yes
    )
else()
    add_test(NAME IntegrationTest
            COMMAND tests
    )
endif()

find_program(VALGRIND_EXECUTABLE valgrind)
find_program(LEAKS_EXECUTABLE leaks)

if(VALGRIND_EXECUTABLE)
    add_test(NAME MemoryTest
            COMMAND valgrind
            --leak-check=full
            --track-origins=yes
            --track-fds=yes
            --show-error-list=yes
            --show-leak-kinds=all
            --errors-for-leak-kinds=all
            --show-reachable=yes
            --show-possibly-lost=yes
            --undef-value-errors=yes
            --expensive-definedness-checks=yes
            --show-mismatched-frees=yes
            --show-realloc-size-zero=yes
            --partial-loads-ok=yes
            --error-exitcode=1
            --log-file=valgrind-report.txt
            $<TARGET_FILE:tests>
    )
elseif(LEAKS_EXECUTABLE)
    add_test(NAME MemoryTest COMMAND leaks -atExit -- $<TARGET_FILE:tests>)
else()
    message(WARNING "Neither 'valgrind' nor 'leaks' were found. Memory test cannot be run.")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)

    if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        set(COVERAGE_TARGET_NAME "coverage")
        add_custom_target(${COVERAGE_TARGET_NAME}
                COMMAND ${LCOV_EXECUTABLE}
                    --directory .
                    --capture
                    --output-file coverage.info
                    --rc lcov_branch_coverage=1
                    --ignore-errors mismatch
                COMMAND ${LCOV_EXECUTABLE}
                    --remove coverage.info
                    --output-file coverage.info
                    --ignore-errors unused
                COMMAND ${GENHTML_EXECUTABLE} -o coverage coverage.info
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating coverage report..."
                VERBATIM
        )
        add_test(NAME CoverageTest
                COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${COVERAGE_TARGET_NAME}
        )
        set_tests_properties(CoverageTest PROPERTIES DEPENDS IntegrationTest)
    else()
        message(WARNING "lcov and/or genhtml not found, coverage target will not be available.")
    endif()
endif()
