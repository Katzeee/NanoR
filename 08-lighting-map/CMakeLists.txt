set(EXE_NAME 08-lighting-map)
aux_source_directory(./src src)
add_executable(${EXE_NAME} ${src})
target_link_libraries(${EXE_NAME} PRIVATE glfw glad::glad)
