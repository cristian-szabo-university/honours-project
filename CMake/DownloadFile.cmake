if (${LOCAL} STREQUAL "")
    message (FATAL_ERROR "LOCAL can't be empty")
endif ()

if (${REMOTE} STREQUAL "")
    message (FATAL_ERROR "REMOTE can't be empty")
endif ()

message(STATUS "Downloading... src=${REMOTE} dst=${LOCAL}")

file(
    DOWNLOAD
    ${REMOTE} ${LOCAL}
    ${SHOW_PROGRESS}
    ${TIMEOUT_ARGS}
    STATUS status
    LOG log)
    
list (GET status 0 status_code)
list (GET status 1 status_string)

if (status_code EQUAL 0)
    message(STATUS "Downloading... done")
    
    return()
else ()
    message ("Error: Downloading '${REMOTE}' failed
        status_code: ${status_code}
        status_string: ${status_string}
        log:
        --- LOG BEGIN ---
        ${log}
        --- LOG END ---")
endif ()
