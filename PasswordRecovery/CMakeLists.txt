project (PasswordRecovery)

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

target_link_libraries (${PROJECT_NAME} PUBLIC docopt HashCracker)

add_custom_command (TARGET ${PROJECT_NAME} 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE_DIR:HashCracker>/$<TARGET_FILE_NAME:HashCracker>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE_DIR:docopt>/$<TARGET_FILE_NAME:docopt>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

install (TARGETS ${PROJECT_NAME}
	EXPORT ${TARGET_NAME}
	RUNTIME DESTINATION "${BINARY_INSTALL_DIR}"
    COMPONENT Applications)
