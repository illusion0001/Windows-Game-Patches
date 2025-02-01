cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT_NAME)
string(REPLACE " " "_" ProjectId ${PROJECT_NAME})
project(${PROJECT_NAME})
include(win32)

ADD_DEFINITIONS(-DMAX_CAVE_SIZE=128)
ADD_DEFINITIONS(-D__IM_SUBMENU__)

include_directories("../../external/imgui")
include_directories("../../external")

file(GLOB SOURCES "*.cpp"
                  "*.hpp"
                  "../Shared/*.cpp"
                  "../../include/*.h"
                  "../../include/*.hpp"
                  "../../external/imgui/*.cpp"
                  "../../external/imgui/backends/imgui_impl_dx11.cpp"
                  "../../external/imgui/backends/imgui_impl_dx11.h"
                  "../../external/imgui/backends/imgui_impl_win32.cpp"
                  "../../external/imgui/backends/imgui_impl_win32.h")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

include(win32_install)

install(
    FILES $<TARGET_PDB_FILE:${PROJECT_NAME}>
    DESTINATION ${PROJECT_NAME} OPTIONAL
)

install(TARGETS ${PROJECT_NAME}
    DESTINATION ${PROJECT_NAME}
)
