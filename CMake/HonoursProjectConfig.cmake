@PACKAGE_INIT@

include (CMakeFindDependencyMacro)

find_dependency (OpenCL 1.1)

add_library (OpenCL STATIC IMPORTED)

set_target_properties (OpenCL PROPERTIES
    IMPORTED_LOCATION ${OpenCL_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${OpenCL_INCLUDE_DIR})

include ("${CMAKE_CURRENT_LIST_DIR}/HonoursProjectTargets.cmake")