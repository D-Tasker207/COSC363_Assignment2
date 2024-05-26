cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Assignment_2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CXX_FLAGS "-Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CXX_FLAGS} -O3")
# set(CMAKE_CXX_FLAGS_DEBUG "${CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS} -fsanitize=address")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

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