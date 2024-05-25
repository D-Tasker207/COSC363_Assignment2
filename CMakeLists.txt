cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Lab07)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CXX_FLAGS "-Wnodeprecated-declarations")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(include)
file(GLOB SOURCES
 "src/*.cpp"
 "src/**/*.cpp"
)

add_executable(RayTracer ${SOURCES})
if(APPLE)
    find_package(glm REQUIRED)
    find_package(OpenGL REQUIRED)
    find_library(GLUT_LIBRARY NAMES glut PATHS /opt/homebrew/opt/freeglut/lib)
    include_directories(/opt/homebrew/opt/freeglut/include)
    target_link_libraries(RayTracer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} glm::glm-header-only)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
    target_link_libraries(RayTracer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
endif()