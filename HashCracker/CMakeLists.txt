project (HashCracker VERSION 1.0.0)

include (GenerateExportHeader)

set (THREADS_PREFER_PTHREAD_FLAG ON)

find_package (Threads REQUIRED)

add_custom_command (
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Source/ProgramEntry.cpp" "${CMAKE_CURRENT_BINARY_DIR}/Include/ProgramEntry.hpp"
    COMMAND ${CMAKE_COMMAND} "-DCL_DIR=${CMAKE_CURRENT_SOURCE_DIR}/OpenCL" "-DFILE_OUT=ProgramEntry.cpp" "-DDIR_OUT=${CMAKE_CURRENT_BINARY_DIR}" -P "${CMAKE_SOURCE_DIR}/CMake/cl2cpp.cmake")
    
configure_file (	
	${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.inc 
	${CMAKE_CURRENT_BINARY_DIR}/Include/Config.hpp @ONLY)

set (BASE_HEADER_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Include/Config.hpp
	${CMAKE_CURRENT_BINARY_DIR}/Include/Export.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/Include/ProgramEntry.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/HashCracker.hpp)

set (CORE_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Core/Charset.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Core/Logger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Core/Platform.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Core/HashCracker.hpp)

set (OPENCL_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/OpenCL/Device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/OpenCL/DeviceMemory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/OpenCL/Kernel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/OpenCL/KernelBuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/OpenCL/KernelParam.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/OpenCL/Program.hpp) 
    
set (TASKS_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Tasks/Task.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Tasks/AttackTask.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Tasks/AutotuneTask.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Tasks/BruteforceAttackTask.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Tasks/SetupTask.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Tasks/BruteforceSetupTask.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Tasks/WorkDispatchTask.hpp)

set (HASHCRACK_HEADER_FILES
	${BASE_HEADER_FILES}
    ${CORE_HEADER_FILES}
    ${OPENCL_HEADER_FILES}
    ${TASKS_HEADER_FILES})

source_group ("Include\\" FILES ${BASE_HEADER_FILES})
source_group ("Include\\Core" FILES ${CORE_HEADER_FILES})
source_group ("Include\\OpenCL" FILES ${OPENCL_HEADER_FILES})
source_group ("Include\\Tasks" FILES ${TASKS_HEADER_FILES})

set (BASE_SOURCE_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Source/ProgramEntry.cpp)

set (CORE_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Charset.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Platform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/HashCracker.cpp)

set (OPENCL_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenCL/Device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenCL/DeviceMemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenCL/Kernel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenCL/KernelBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenCL/KernelParam.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/OpenCL/Program.cpp) 

set (TASKS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Tasks/Task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Tasks/AttackTask.cpp    
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Tasks/AutotuneTask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Tasks/BruteforceAttackTask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Tasks/SetupTask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Tasks/BruteforceSetupTask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Tasks/WorkDispatchTask.cpp)

set (HASHCRACK_SOURCE_FILES
	${BASE_SOURCE_FILES}
    ${CORE_SOURCE_FILES}
    ${OPENCL_SOURCE_FILES}
    ${TASKS_SOURCE_FILES})

source_group ("Source\\" FILES ${BASE_SOURCE_FILES})
source_group ("Source\\Core" FILES ${CORE_SOURCE_FILES})
source_group ("Source\\OpenCL" FILES ${OPENCL_SOURCE_FILES})
source_group ("Source\\Tasks" FILES ${TASKS_SOURCE_FILES})

add_library (${PROJECT_NAME} SHARED ${HASHCRACK_HEADER_FILES} ${HASHCRACK_SOURCE_FILES})

add_dependencies (${PROJECT_NAME} OpenCL-ICD-Loader)

target_link_libraries (${PROJECT_NAME} PUBLIC OpenCL Threads::Threads)

target_include_directories (${PROJECT_NAME} PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include> 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/Include>)

generate_export_header(${PROJECT_NAME}
	EXPORT_MACRO_NAME "HASHCRACK_PUBLIC"
	EXPORT_FILE_NAME "Include/Export.hpp")

install (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Include/"
    DESTINATION "${INCLUDE_INSTALL_DIR}" COMPONENT Headers)

install (DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Include/"
    DESTINATION "${INCLUDE_INSTALL_DIR}" COMPONENT Headers)

install (TARGETS ${PROJECT_NAME}
	EXPORT ${TARGET_NAME}
	ARCHIVE DESTINATION "${LIBRARY_INSTALL_DIR}"  COMPONENT Libraries
	LIBRARY DESTINATION "${LIBRARY_INSTALL_DIR}" COMPONENT Libraries
	RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT Binaries)
