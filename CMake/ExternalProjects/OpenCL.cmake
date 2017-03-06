set (EXTERN_DOWNLOAD_DIR "${EXTERN_BASE_DIR}/Download")
set (EXTERN_SOURCE_DIR "${EXTERN_BASE_DIR}/Source")

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
    OpenCL-CPP-Header
    
    URL "https://www.khronos.org/registry/cl/api/2.1/cl.hpp"
    DOWNLOAD_NO_EXTRACT 0
    
    CONFIGURE_COMMAND "" 
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add (
    OpenCL-ICD-Loader

    DEPENDS OpenCL-Headers OpenCL-CPP-Header
    
    GIT_REPOSITORY "https://github.com/KhronosGroup/OpenCL-ICD-Loader"
    GIT_TAG "master"
    
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

ExternalProject_Add_Step (OpenCL-ICD-Loader copy_opencl_header_files
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/opencl.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/opencl.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl_platform.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl_platform.h" 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl_ext.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl_ext.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl_gl.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl_gl.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl_gl_ext.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl_gl_ext.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl_egl.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl_egl.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl_d3d10.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl_d3d10.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl_d3d11.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl_d3d11.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_SOURCE_DIR}/OpenCL-Headers/cl_dx9_media_sharing.h" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl_dx9_media_sharing.h"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EXTERN_DOWNLOAD_DIR}/OpenCL-CPP-Header/cl.hpp" "${EXTERN_SOURCE_DIR}/OpenCL-ICD-Loader/inc/CL/cl.hpp"
	DEPENDEES update
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	LOG 1
) 

ExternalProject_Get_Property (OpenCL-ICD-Loader SOURCE_DIR)
ExternalProject_Get_Property (OpenCL-ICD-Loader BINARY_DIR)

set (PROJECT_NAME "OpenCL")

file(MAKE_DIRECTORY "${SOURCE_DIR}/inc")

add_library (${PROJECT_NAME} SHARED IMPORTED)

set_target_properties (${PROJECT_NAME} PROPERTIES
	IMPORTED_LOCATION "${BINARY_DIR}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    IMPORTED_IMPLIB "${BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}/inc")
    
add_dependencies (${PROJECT_NAME} OpenCL-Headers OpenCL-CPP-Header OpenCL-ICD-Loader)
