function(_FIND_OPENCL_VERSION)
    include(CheckSymbolExists)
    include(CMakePushCheckState)
    
    set(CMAKE_REQUIRED_QUIET ${OpenCL_FIND_QUIETLY})

    CMAKE_PUSH_CHECK_STATE()
        foreach(VERSION "2_2" "2_1" "2_0" "1_3" "1_2" "1_1" "1_0")
            set(CMAKE_REQUIRED_INCLUDES "${OpenCL_INCLUDE_DIR}")

            if(APPLE)
                CHECK_SYMBOL_EXISTS(
                    CL_VERSION_${VERSION}
                    "${OpenCL_INCLUDE_DIR}/OpenCL/cl.h"
                    OPENCL_VERSION_${VERSION})
            else()
                CHECK_SYMBOL_EXISTS(
                    CL_VERSION_${VERSION}
                    "${OpenCL_INCLUDE_DIR}/CL/cl.h"
                    OPENCL_VERSION_${VERSION})
            endif()

            if(OPENCL_VERSION_${VERSION})
                string(REPLACE "_" "." VERSION "${VERSION}")
                set(OpenCL_VERSION_STRING ${VERSION} PARENT_SCOPE)
                string(REGEX MATCHALL "[0-9]+" version_components "${VERSION}")
                list(GET version_components 0 major_version)
                list(GET version_components 1 minor_version)
                set(OpenCL_VERSION_MAJOR ${major_version} PARENT_SCOPE)
                set(OpenCL_VERSION_MINOR ${minor_version} PARENT_SCOPE)
                break()
            endif()
        endforeach()
    CMAKE_POP_CHECK_STATE()
    endfunction()

find_path(OpenCL_INCLUDE_DIR
    NAMES
        CL/cl.h OpenCL/cl.h
    PATHS
        ENV "PROGRAMFILES(X86)"
        ENV AMDAPPSDKROOT
        ENV INTELOCLSDKROOT
        ENV NVSDKCOMPUTE_ROOT
        ENV CUDA_PATH
        ENV ATISTREAMSDKROOT
        ENV OPENCL_ICD_LOADER_ROOT
    PATH_SUFFIXES
        include
        inc
        OpenCL/common/inc
        "AMD APP/include")

_FIND_OPENCL_VERSION()

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        find_library(OpenCL_LIBRARY
            NAMES 
                OpenCL
            PATHS
                ENV "PROGRAMFILES(X86)"
                ENV AMDAPPSDKROOT
                ENV INTELOCLSDKROOT
                ENV CUDA_PATH
                ENV NVSDKCOMPUTE_ROOT
                ENV ATISTREAMSDKROOT
                ENV OPENCL_ICD_LOADER_ROOT
            PATH_SUFFIXES
                Build
                Build/bin
                "AMD APP/lib/x86"
                lib/x86
                lib/Win32
                OpenCL/common/lib/Win32)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        find_library(OpenCL_LIBRARY
            NAMES 
                OpenCL
            PATHS
                ENV "PROGRAMFILES(X86)"
                ENV AMDAPPSDKROOT
                ENV INTELOCLSDKROOT
                ENV CUDA_PATH
                ENV NVSDKCOMPUTE_ROOT
                ENV ATISTREAMSDKROOT
                ENV OPENCL_ICD_LOADER_ROOT
            PATH_SUFFIXES
                Build
                Build/bin
                "AMD APP/lib/x86_64"
                lib/x86_64
                lib/x64
                OpenCL/common/lib/x64)
    endif()
else()
    find_library(OpenCL_LIBRARY
        NAMES 
            OpenCL
        PATHS
            ENV OPENCL_ICD_LOADER_ROOT
        PATH_SUFFIXES
            Build
            Build/bin)
endif()

set(OpenCL_LIBRARIES ${OpenCL_LIBRARY})
set(OpenCL_INCLUDE_DIRS ${OpenCL_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    OpenCL
    FOUND_VAR OpenCL_FOUND
    REQUIRED_VARS OpenCL_LIBRARY OpenCL_INCLUDE_DIR
    VERSION_VAR OpenCL_VERSION_STRING)

mark_as_advanced(
    OpenCL_INCLUDE_DIR
    OpenCL_LIBRARY)
