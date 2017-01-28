cmake_minimum_required (VERSION 3.2)

###############################################################################
#
#   MAIN
#
###############################################################################

project (HonoursProject VERSION 1.0.0)

include (ExternalProject)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
	message(FATAL_ERROR "In-tree builds are not supported; please run cmake in a separate build directory.")
endif ()

list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")
list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

set (CMAKE_DEBUG_POSTFIX "_Debug")

set (TARGET_NAME "${CMAKE_PROJECT_NAME}Targets")
set (CONFIG_FILE "${CMAKE_PROJECT_NAME}-Config.cmake")
set (VERSION_FILE "${CMAKE_PROJECT_NAME}-Config-Version.cmake")
set (TARGET_FILE "${CMAKE_PROJECT_NAME}-Targets.cmake")

set (INCLUDE_INSTALL_DIR "Include")
set (LIBRARY_INSTALL_DIR "Library")
set (BINARY_INSTALL_DIR "Binary")
set (CONFIG_INSTALL_DIR "CMake")

###############################################################################
#
#   EXTERN PROJECTS
#
###############################################################################

set (EXTERN_BASE_DIR "${CMAKE_BINARY_DIR}/Extern")
set (EXTERN_DOWNLOAD_DIR "${EXTERN_BASE_DIR}/Download")
set (EXTERN_INSTALL_DIR "${EXTERN_BASE_DIR}/Install")
set (EXTERN_SOURCE_DIR "${EXTERN_BASE_DIR}/Source")
set (EXTERN_BINARY_DIR "${EXTERN_BASE_DIR}/Build")

set_directory_properties (PROPERTIES EP_BASE ${EXTERN_BASE_DIR})

ExternalProject_Add (
    Docopt

    GIT_REPOSITORY "https://github.com/docopt/docopt.cpp.git"
    GIT_TAG "master"  
    
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property (Docopt SOURCE_DIR)
ExternalProject_Get_Property (Docopt BINARY_DIR)

add_library (docopt STATIC IMPORTED)
set_target_properties (docopt PROPERTIES
	IMPORTED_LOCATION "${BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}docopt_s${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}")

ExternalProject_Add (
    OpenCL-Headers

    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-Headers"
    GIT_TAG "opencl21"
    
    CONFIGURE_COMMAND "" 
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add (
    OpenCL-ICD-Loader

    DEPENDS OpenCL-Headers
    
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-ICD-Loader"
    GIT_TAG "master"
    
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

ExternalProject_Add_Step (OpenCL-ICD-Loader copy_opencl_header_files
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${EXTERN_SOURCE_DIR}/OpenCL-Headers" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL"
	COMMAND ${CMAKE_COMMAND} "-DREMOTE=https://www.khronos.org/registry/cl/api/2.1/cl.hpp" "-DLOCAL=${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl.hpp" -P "${CMAKE_SOURCE_DIR}/CMake/DownloadFile.cmake"
	DEPENDEES update
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	LOG 1
) 

ExternalProject_Get_Property (OpenCL-ICD-Loader SOURCE_DIR)
ExternalProject_Get_Property (OpenCL-ICD-Loader BINARY_DIR)

file(MAKE_DIRECTORY "${SOURCE_DIR}/inc")

add_library (OpenCL STATIC IMPORTED)
set_target_properties (OpenCL PROPERTIES
	IMPORTED_LOCATION "${BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}OpenCL${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}/inc")

###############################################################################
#
#   INTERN PROJECTS
#
###############################################################################

add_subdirectory ("${CMAKE_SOURCE_DIR}/HashCrack")
add_subdirectory ("${CMAKE_SOURCE_DIR}/HashApp")

###############################################################################
#
#   INSTALL
#
###############################################################################
include (CMakePackageConfigHelpers)

configure_package_config_file (
	"${CMAKE_SOURCE_DIR}/ProjectConfig.cmake.inc" ${CONFIG_FILE}
	INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR BINARY_INSTALL_DIR)

write_basic_package_version_file (
	${VERSION_FILE}
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

install (FILES "${CMAKE_BINARY_DIR}/${CONFIG_FILE}" 
	"${CMAKE_BINARY_DIR}/${VERSION_FILE}"
    DESTINATION ${CONFIG_INSTALL_DIR})

install (EXPORT ${TARGET_NAME}
	FILE ${TARGET_FILE}
	NAMESPACE "${CMAKE_PROJECT_NAME}::"
	DESTINATION ${CONFIG_INSTALL_DIR})

###############################################################################
#
#   CPACK
#
###############################################################################
    
set (CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Honours Project for Computing School at UWS")
set (CPACK_PACKAGE_VENDOR "B00233705")
set (CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})
set (CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set (CPACK_COMPONENT_HEADERS_DEPENDS Binaries Libraries)
set (CPACK_COMPONENT_APPLICATIONS_GROUP Runtime)
set (CPACK_COMPONENT_BINARIES_GROUP Development)
set (CPACK_COMPONENT_LIBRARIES_GROUP Development)
set (CPACK_COMPONENT_HEADERS_GROUP Development)

set (CPACK_ALL_INSTALL_TYPES Full Application Developer)
set (CPACK_COMPONENTS_ALL Applications Binaries Libraries Headers)
set (CPACK_COMPONENT_BINARIES_INSTALL_TYPES Application Developer Full)
set (CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Developer Full)
set (CPACK_COMPONENT_HEADERS_INSTALL_TYPES Developer Full)
set (CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES Application Full)
 
set (CPACK_PACKAGE_DIRECTORY "${PROJECT_SOURCE_DIR}/Package")
set (CPACK_STRIP_FILES TRUE)

include (CPack)
