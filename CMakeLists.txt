cmake_minimum_required (VERSION 3.7)

include (GNUInstallDirs)
include (ExternalProject)
include (CMakePackageConfigHelpers)

###############################################################################
#
#   CONFIG
#
###############################################################################

project (HonoursProject VERSION 1.0.0)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
	message(FATAL_ERROR "In-tree builds are not supported; please run cmake in a separate build directory.")
endif ()

list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_DEBUG_POSTFIX "_Debug")

set (EXPORT_NAME "${CMAKE_PROJECT_NAME}Targets")
set (CONFIG_FILE "${CMAKE_PROJECT_NAME}Config.cmake")
set (VERSION_FILE "${CMAKE_PROJECT_NAME}ConfigVersion.cmake")
set (CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}/cmake")

set_directory_properties (PROPERTIES EP_BASE "${CMAKE_BINARY_DIR}/ExternalProjects")

###############################################################################
#
#   PROJECTS
#
###############################################################################

include (ExternalProjects/Docopt)
include (ExternalProjects/OpenCL)

add_subdirectory ("${CMAKE_SOURCE_DIR}/HashCracker")
add_subdirectory ("${CMAKE_SOURCE_DIR}/PasswordRecovery")

###############################################################################
#
#   INSTALL
#
###############################################################################

configure_package_config_file (
	"${CMAKE_SOURCE_DIR}/CMake/${CMAKE_PROJECT_NAME}Config.cmake" "${CMAKE_BINARY_DIR}/${CONFIG_FILE}"
	INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}")

write_basic_package_version_file (
	"${CMAKE_BINARY_DIR}/${VERSION_FILE}"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

install (FILES "${CMAKE_BINARY_DIR}/${CONFIG_FILE}" "${CMAKE_BINARY_DIR}/${VERSION_FILE}"
    DESTINATION ${CONFIG_INSTALL_DIR})

install (EXPORT ${EXPORT_NAME} 
    DESTINATION ${CONFIG_INSTALL_DIR}
    NAMESPACE "${CMAKE_PROJECT_NAME}::")

###############################################################################
#
#   PACKAGE
#
###############################################################################

set (CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Honours Project for Computing Science at UWS")
set (CPACK_PACKAGE_VENDOR "B00233705")
set (CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})
set (CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set (CPACK_STRIP_FILES TRUE)

include (CPack)
