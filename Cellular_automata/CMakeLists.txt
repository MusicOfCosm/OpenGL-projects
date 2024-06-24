cmake_minimum_required (VERSION 3.8)

project ("Conway-s_Game_of_Life")

# Add source to this project's executable.
add_executable(${PROJECT_NAME} 
  "src/Cellular_automata.cpp"
  "../ressources/renderer.cpp"
  "../ressources/stb_image.c"
  )

#target_compile_definitions(${PROJECT_NAME} PRIVATE GOL=1)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)


project ("GPU_Info")

add_executable(${PROJECT_NAME} "src/GPU_Info.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
