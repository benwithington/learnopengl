cmake_minimum_required(VERSION 3.23)

set(GLAD_DIR "../external/glad/")
set(STBI_DIR "../external/stbi/")

add_executable(${PROJECT_NAME}
    main.cpp
    shader.cpp
    camera.cpp
    mesh.cpp
    model.cpp
    window.cpp
    texture.cpp
    ${GLAD_DIR}/src/glad.c
    ${STBI_DIR}/src/stbi.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -O3 -lassimp)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLAD_DIR}/include/
    ${STBI_DIR}/include/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glm::glm
    assimp::assimp
    imgui::imgui
)