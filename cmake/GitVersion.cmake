function(get_version_from_git)
    find_package(Git QUIET)
    if(NOT Git_FOUND)
        message(WARNING "Git not found")
        return()
    endif()

    execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --always
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE GIT_RESULT
    )

    if(NOT GIT_RESULT EQUAL 0)
        message(WARNING "Failed to get git tag")
        return()
    endif()

    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short=7 HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_SHORT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE GIT_HASH_RESULT
    )

    if(NOT GIT_HASH_RESULT EQUAL 0)
        message(WARNING "Failed to get git commit hash, falling back to version without git metadata")
        unset(GIT_COMMIT_SHORT_HASH)
    endif()

    string(REGEX REPLACE "^v" "" CLEAN_TAG "${GIT_TAG}")
    if(CLEAN_TAG MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-.*)?$")

        set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
        set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1} PARENT_SCOPE)
        set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
        set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2} PARENT_SCOPE)
        set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
        set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3} PARENT_SCOPE)

        # Base semantic version from the tag
        set(VERSION_BASE "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")

        # Only append git metadata when it is available
        if(GIT_COMMIT_SHORT_HASH)
            set(FULL_VERSION "${VERSION_BASE}+${GIT_COMMIT_SHORT_HASH}")
        else()
            set(FULL_VERSION "${VERSION_BASE}")
        endif()

        set(FULL_VERSION "${FULL_VERSION}" PARENT_SCOPE)
        set(PROJECT_VERSION "${VERSION_BASE}")
        set(PROJECT_VERSION "${VERSION_BASE}" PARENT_SCOPE)
    else()
        message(WARNING "Tag '${CLEAN_TAG}' does not match semver format")
    endif()
endfunction()