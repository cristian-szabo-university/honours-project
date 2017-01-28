project (HashApp VERSION 1.0.0)

include (ExternalProject)

ExternalProject_Get_Property (Docopt SOURCE_DIR)
ExternalProject_Get_Property (Docopt BINARY_DIR)

include_directories ("${SOURCE_DIR}/inc")
link_directories ("${BINARY_DIR}")

add_library (docopt STATIC IMPORTED)
set_target_properties (docopt PROPERTIES
	IMPORTED_LOCATION "${BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}docopt_s${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}")

include_directories (
	${CMAKE_CURRENT_SOURCE_DIR}/Include)

set (HASHAPP_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Include/Main.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Program.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/ConsoleInput.hpp)
	
set (HASHAPP_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Program.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/ConsoleInput.cpp)

add_executable (${PROJECT_NAME} ${HASHAPP_HEADER_FILES} ${HASHAPP_SOURCE_FILES})

target_link_libraries (${PROJECT_NAME} PUBLIC docopt HashCrack)

add_dependencies (${PROJECT_NAME} Docopt)
  
install (TARGETS ${PROJECT_NAME}
	EXPORT ${TARGET_NAME}
	RUNTIME DESTINATION "${BINARY_INSTALL_DIR}"
    COMPONENT Applications)
