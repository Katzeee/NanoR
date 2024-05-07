FetchContent_GetProperties(shaderc)
if(NOT shaderc_POPULATED)
  FetchContent_Populate(shaderc)
  message(STATUS "Sync shaderc dependencies.")
  find_program(Python_EXECUTABLE python NO_CMAKE_FIND_ROOT_PATH)
  execute_process(COMMAND ${Python_EXECUTABLE} ${shaderc_SOURCE_DIR}/utils/git-sync-deps)
  add_subdirectory(${shaderc_SOURCE_DIR} ${shaderc_BINARY_DIR})
endif()