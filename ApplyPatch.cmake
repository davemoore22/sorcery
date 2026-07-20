if(NOT DEFINED SOURCE_DIR)
    message(FATAL_ERROR "SOURCE_DIR was not provided")
endif()

if(NOT DEFINED PATCH_FILE)
    message(FATAL_ERROR "PATCH_FILE was not provided")
endif()

# First determine whether the patch can be applied.
execute_process(
    COMMAND git apply --check "${PATCH_FILE}"
    WORKING_DIRECTORY "${SOURCE_DIR}"
    RESULT_VARIABLE patch_can_apply
    OUTPUT_QUIET
    ERROR_QUIET
)

if(patch_can_apply EQUAL 0)
    message(STATUS "Applying patch: ${PATCH_FILE}")

    execute_process(
        COMMAND git apply "${PATCH_FILE}"
        WORKING_DIRECTORY "${SOURCE_DIR}"
        RESULT_VARIABLE patch_result
    )

    if(NOT patch_result EQUAL 0)
        message(FATAL_ERROR "Failed to apply patch: ${PATCH_FILE}")
    endif()

    return()
endif()

# If it cannot be applied, check whether it has already been applied.
execute_process(
    COMMAND git apply --reverse --check "${PATCH_FILE}"
    WORKING_DIRECTORY "${SOURCE_DIR}"
    RESULT_VARIABLE patch_already_applied
    OUTPUT_QUIET
    ERROR_QUIET
)

if(patch_already_applied EQUAL 0)
    message(STATUS "Patch already applied: ${PATCH_FILE}")
    return()
endif()

message(FATAL_ERROR
    "Patch cannot be applied and does not appear to be already applied:\n"
    "${PATCH_FILE}"
)