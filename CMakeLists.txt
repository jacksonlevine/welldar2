cmake_minimum_required(VERSION 3.10)

# set the project name
project(welldaw)

add_executable(main src/main.cpp)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(main PRIVATE imgui::imgui)

find_package(SFML COMPONENTS system window graphics audio REQUIRED)
target_link_libraries(main PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)

find_package(portaudio CONFIG REQUIRED)
    target_link_libraries(main PRIVATE portaudio)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(main PRIVATE glfw)
find_package(glm CONFIG REQUIRED)
target_link_libraries(main PRIVATE glm::glm)
find_package(GLEW REQUIRED)
target_link_libraries(main PRIVATE GLEW::GLEW)
