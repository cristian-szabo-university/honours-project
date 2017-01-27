cmake_minimum_required (VERSION 3.2)

###############################################################################
#
#   MAIN
#
###############################################################################

include (ExternalProject)

project (HonoursProject VERSION 1.0.0)

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
#   PROJECTS
#
###############################################################################

ExternalProject_Add (
    OpenCL-Headers

    PREFIX "OpenCL-Headers"
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-Headers"
    GIT_TAG "opencl21"

    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Extern/OpenCL-Headers"
    BINARY_DIR "${CMAKE_BINARY_DIR}/Extern/OpenCL-Headers"   
    
    CONFIGURE_COMMAND "" 
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add (
    OpenCL-CPP-Header
    
    PREFIX "OpenCL-CPP-Header"
    URL "https://www.khronos.org/registry/cl/api/2.1/cl.hpp"
    DOWNLOAD_NO_EXTRACT 0
    
    DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/Extern/OpenCL-CPP-Header"
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Extern/OpenCL-CPP-Header"
    BINARY_DIR "${CMAKE_BINARY_DIR}/Extern/OpenCL-CPP-Header"
    
    CONFIGURE_COMMAND "" 
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add(
    OpenCL-ICD-Loader

    DEPENDS OpenCL-Headers OpenCL-CPP-Header
    
    PREFIX "OpenCL-ICD-Loader"
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-ICD-Loader"
    GIT_TAG "master"
    
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Extern/OpenCL-ICD-Loader"
    BINARY_DIR "${CMAKE_BINARY_DIR}/Extern/OpenCL-ICD-Loader"
        
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

ExternalProject_Add_Step(OpenCL-ICD-Loader copy_opencl_header_files
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Extern/OpenCL-Headers" "${CMAKE_SOURCE_DIR}/Extern/OpenCL-ICD-Loader/inc/CL"
	DEPENDERS build
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	LOG 1
) 

ExternalProject_Add_Step(OpenCL-ICD-Loader copy_opencl_hpp_file
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/Extern/OpenCL-CPP-Header/cl.hpp" "${CMAKE_SOURCE_DIR}/Extern/OpenCL-ICD-Loader/inc/CL/cl.hpp"
	DEPENDERS build
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	LOG 1
) 

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
