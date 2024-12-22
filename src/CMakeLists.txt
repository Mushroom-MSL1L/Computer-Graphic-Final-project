add_executable(ICG_2024_HW3
"main.cpp"
"stb_image.cpp"
"shader.cpp"
"object.cpp"
) #列所有的cpp
target_link_libraries(ICG_2024_HW3
glfw
glm::glm
glad
)