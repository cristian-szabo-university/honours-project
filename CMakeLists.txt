cmake_minimum_required (VERSION 3.6)

include (GNUInstallDirs)
include (ExternalProject)
include (CMakePackageConfigHelpers)

###############################################################################
#
#   CONFIGURE
#
###############################################################################

project (HonoursProject VERSION 1.0.0)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
	message(FATAL_ERROR "In-tree builds are not supported; please run cmake in a separate build directory.")
endif ()

list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_DEBUG_POSTFIX "_Debug")

set (TARGET_NAME "${CMAKE_PROJECT_NAME}Targets")
set (CONFIG_FILE "${CMAKE_PROJECT_NAME}-Config.cmake")
set (VERSION_FILE "${CMAKE_PROJECT_NAME}-Config-Version.cmake")
set (TARGET_FILE "${CMAKE_PROJECT_NAME}-Targets.cmake")

set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}")
set (LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}")
set (BINARY_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
set (CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}/CMake")

###############################################################################
#
#   EXTERNAL PROJECTS
#
###############################################################################

if (WIN32)
    set (DEST_INSTALL_DIR ${BINARY_INSTALL_DIR})
else ()
    set (DEST_INSTALL_DIR ${LIBRARY_INSTALL_DIR})
endif ()

set (EXTERN_BASE_DIR "${CMAKE_BINARY_DIR}/ExternalProjects")

set_directory_properties (PROPERTIES EP_BASE ${EXTERN_BASE_DIR})

include (ExternalProjects/Docopt)
include (ExternalProjects/OpenCL)

###############################################################################
#
#   INTERNAL PROJECTS
#
###############################################################################

add_subdirectory ("${CMAKE_SOURCE_DIR}/HashCracker")
add_subdirectory ("${CMAKE_SOURCE_DIR}/PasswordRecovery")

###############################################################################
#
#   INSTALL
#
###############################################################################

configure_package_config_file (
	"${CMAKE_SOURCE_DIR}/CMake/${CMAKE_PROJECT_NAME}-Config.cmake" ${CONFIG_FILE}
	INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR BINARY_INSTALL_DIR)

write_basic_package_version_file (
	${VERSION_FILE}
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

install (FILES "${CMAKE_BINARY_DIR}/${CONFIG_FILE}" "${CMAKE_BINARY_DIR}/${VERSION_FILE}"
    DESTINATION ${CONFIG_INSTALL_DIR} COMPONENT Libraries)
    
install (DIRECTORY "${CMAKE_SOURCE_DIR}/CMake/ExternalProjects"
    DESTINATION ${CONFIG_INSTALL_DIR} COMPONENT Libraries)

install (EXPORT ${TARGET_NAME}
	NAMESPACE "${CMAKE_PROJECT_NAME}::"
	DESTINATION ${CONFIG_INSTALL_DIR})

###############################################################################
#
#   PACKAGE
#
###############################################################################

set (CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Honours Project for Computing School at UWS")
set (CPACK_PACKAGE_VENDOR "B00233705")
set (CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})
set (CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set (CPACK_COMPONENT_HEADERS_DEPENDS Binaries Libraries)
set (CPACK_COMPONENT_APPLICATIONS_DEPENDS Binaries)

set (CPACK_COMPONENT_APPLICATIONS_GROUP Runtime)
set (CPACK_COMPONENT_BINARIES_GROUP Development)
set (CPACK_COMPONENT_LIBRARIES_GROUP Development)
set (CPACK_COMPONENT_HEADERS_GROUP Development)

set (CPACK_ALL_INSTALL_TYPES Full Applications Developer)
set (CPACK_COMPONENTS_ALL Applications Binaries Libraries Headers)
set (CPACK_COMPONENT_BINARIES_INSTALL_TYPES Applications Developer Full)
set (CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Developer Full)
set (CPACK_COMPONENT_HEADERS_INSTALL_TYPES Developer Full)
set (CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES Applications Full)

set (CPACK_STRIP_FILES TRUE)

include (CPack)
