cmake_minimum_required(VERSION 3.2)
project(cg-rta)

# -DCMAKE_BUILD_TYPE=Release
# -DCMAKE_BUILD_TYPE=Debug
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

Set(CMAKE_CXX_STANDARD 11)
Set(CMAKE_CXX_EXTENSIONS OFF)

# Add OpenGL
find_package(OpenGL REQUIRED)

# Add Assimp
find_package(ASSIMP REQUIRED)

# Add Eigen
include_directories(/usr/local/include/eigen3)

# Add GLFW
find_package(GLFW3 REQUIRED)

# Add GLM
find_package(GLM REQUIRED)

# Add TinyDir
include_directories(${CMAKE_SOURCE_DIR}/tinydir)

# Add MeshOptimizer
include_directories(${CMAKE_SOURCE_DIR}/meshoptimizer/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/meshoptimizer)

# Add source files and final compilation
include_directories(include)
set(SOURCE src/main.cpp src/glad.c src/ui/imgui.cpp src/ui/imgui_draw.cpp 
    src/ui/imgui_widgets.cpp src/ui/imgui_demo.cpp src/ui/imgui_impl_opengl3.cpp 
    src/ui/imgui_impl_glfw.cpp src/blendshape_loader.cpp src/blendshape_model.cpp
    src/glprogram.cpp src/mesh_utility.cpp src/blendshape_modifier.cpp src/blendshape_animator.cpp
    src/ui_manipulator.cpp src/primitives.cpp src/bvh.cpp)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
#add_compile_options(-Wpragma-pack)
add_executable(${PROJECT_NAME} ${SOURCE})
#${ASSIMP_LIBRARIES}
target_link_libraries(${PROJECT_NAME} glfw ${OPENGL_gl_LIBRARY} meshoptimizer)
