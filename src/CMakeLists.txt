cmake_minimum_required(VERSION 3.23)

set(GLAD_DIR "../external/glad/")
set(STBI_DIR "../external/stbi/")

add_executable(${PROJECT_NAME}
    main.cpp
    shader.cpp
    camera.cpp
    mesh.cpp
    model.cpp
    ${GLAD_DIR}/src/glad.c
    ${STBI_DIR}/src/stbi.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -O3)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLAD}/include/
    ${STBI_DIR}/include/
)
