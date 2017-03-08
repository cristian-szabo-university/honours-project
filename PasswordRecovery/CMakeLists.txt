project (PasswordRecovery)

file (GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp")
file (GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

source_group ("Include\\" FILES ${HEADER_FILES})
source_group ("Source\\" FILES ${SOURCE_FILES})

add_executable (${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories (${PROJECT_NAME} PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>)

target_link_libraries (${PROJECT_NAME} PUBLIC HashCracker docopt)

add_custom_command (TARGET ${PROJECT_NAME} 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "$<TARGET_FILE_DIR:HashCracker>/$<TARGET_FILE_NAME:HashCracker>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

install (TARGETS ${PROJECT_NAME} EXPORT ${EXPORT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
