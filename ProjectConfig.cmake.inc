@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

include ("${CMAKE_CURRENT_LIST_DIR}/@CMAKE_PROJECT_NAME@-Targets.cmake")

find_dependency(OpenCL 1.1)

add_library (OpenCL STATIC IMPORTED)

set_target_properties (OpenCL PROPERTIES
	IMPORTED_LOCATION ${OpenCL_LIBRARIES}
	INTERFACE_INCLUDE_DIRECTORIES ${OpenCL_INCLUDE_DIRS}
)