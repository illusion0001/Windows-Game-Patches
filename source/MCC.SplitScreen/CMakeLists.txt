include(yaml-cpp)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT_NAME)
string(REPLACE " " "_" ProjectId ${PROJECT_NAME})
project(${PROJECT_NAME})
include(win32)

file(GLOB SOURCES "*.cpp" "*.hpp" "../Shared/*.cpp" "../../include/*.h" "../../include/*.hpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

include_directories(${YAML_CPP_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      ${YAML_CPP_LIBRARIES}
                      )

include(win32_install)
