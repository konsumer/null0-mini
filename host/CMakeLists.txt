find_package(PhysFS REQUIRED)
find_package(pntr REQUIRED)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.c)

if (EMSCRIPTEN)
  target_link_libraries(${PROJECT_NAME} pntr physfs-static)
  target_link_options(${PROJECT_NAME} PUBLIC -Oz -sSINGLE_FILE -sUSE_WEBGL2=1 -sFORCE_FILESYSTEM -sEXPORTED_RUNTIME_METHODS=FS -sASYNCIFY)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs/"
    RUNTIME_OUTPUT_NAME "host"
    SUFFIX ".mjs"
  )
else()
  find_package(wamr REQUIRED)
  find_package(minifb REQUIRED)
  target_link_libraries(${PROJECT_NAME} pntr physfs-static m wamr minifb)
endif()
