cmake_minimum_required(VERSION 3.16)

project(imgui_glfw_opengl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------
# Find dependencies
# -----------------------------
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

# -----------------------------
# ImGui sources
# -----------------------------
set(IMGUI_DIR imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# -----------------------------
# Patterns (OBJECT library)
# -----------------------------
file(GLOB PATTERNS
    "${CMAKE_CURRENT_SOURCE_DIR}/patterns_impl/*.cpp"
)
add_library(patterns_impl OBJECT
    ${PATTERNS}
)

# -----------------------------
# Fields (OBJECT library)
# -----------------------------
file(GLOB FIELDS
    "${CMAKE_CURRENT_SOURCE_DIR}/fields_impl/*.cpp"
)
add_library(fields_impl OBJECT
    ${FIELDS}
)

# -----------------------------
# Sub macros (OBJECT library)
# -----------------------------
file(GLOB SUB_MACROS
    "${CMAKE_CURRENT_SOURCE_DIR}/sub_macros/*.cpp"
)
add_library(sub_macros OBJECT
    ${SUB_MACROS}
)

# -----------------------------
# Build executable
# -----------------------------
add_executable(app
    ./core.cpp
    ./application.cpp
    ./interface.cpp
    ./theme_api.cpp
    ./walk.cpp
    ./utils.cpp
    ${IMGUI_SOURCES}
    $<TARGET_OBJECTS:patterns_impl>
    $<TARGET_OBJECTS:fields_impl>
    $<TARGET_OBJECTS:sub_macros>
)

# -----------------------------
# Include directories
# -----------------------------
target_include_directories(app PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ./
)

# -----------------------------
# Link libraries
# -----------------------------
target_link_libraries(app PRIVATE
    glfw
    OpenGL::GL
    X11::X11
    Xtst
    X11::Xext
)

# Link additional libraries for Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(app PRIVATE dl pthread)
endif()

# -----------------------------
# Compile options
# -----------------------------
target_compile_options(app PRIVATE -g)
