ExternalProject_Add (
    Docopt

    GIT_REPOSITORY "https://github.com/docopt/docopt.cpp.git"
    GIT_TAG "master"
       
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
    
    UPDATE_COMMAND "")

ExternalProject_Get_Property (Docopt INSTALL_DIR)

set (PROJECT_NAME "docopt")

set (DEST_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})

if (WIN32)
    set (DEST_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
endif ()

file (MAKE_DIRECTORY "${INSTALL_DIR}/include/${PROJECT_NAME}")

add_library (${PROJECT_NAME} STATIC IMPORTED)

set_target_properties (${PROJECT_NAME} PROPERTIES
	IMPORTED_LOCATION "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}/include/${PROJECT_NAME}")
    
if (WIN32)
    set_target_properties (${PROJECT_NAME} PROPERTIES
        IMPORTED_LOCATION "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${PROJECT_NAME}_s${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif ()
    
add_dependencies (${PROJECT_NAME} Docopt)
