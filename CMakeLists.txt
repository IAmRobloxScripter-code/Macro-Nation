cmake_minimum_required(VERSION 3.16)

project("Macro Nation" CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

set(IMGUI_DIR imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

file(GLOB PATTERNS
    "${CMAKE_CURRENT_SOURCE_DIR}/patterns_impl/*.cpp"
)
add_library(patterns_impl OBJECT
    ${PATTERNS}
)

file(GLOB FIELDS
    "${CMAKE_CURRENT_SOURCE_DIR}/fields_impl/*.cpp"
)
add_library(fields_impl OBJECT
    ${FIELDS}
)

file(GLOB SUB_MACROS
    "${CMAKE_CURRENT_SOURCE_DIR}/sub_macros/*.cpp"
)
add_library(sub_macros OBJECT
    ${SUB_MACROS}
)

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

target_include_directories(app PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ./
)

target_link_libraries(app PRIVATE
    glfw
    OpenGL::GL
    X11::X11
    Xtst
    X11::Xext
)

if(UNIX AND NOT APPLE)
    target_link_libraries(app PRIVATE dl pthread)
endif()

target_compile_options(app PRIVATE -g)