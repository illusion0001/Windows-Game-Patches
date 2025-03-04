cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT_NAME)
string(REPLACE " " "_" ProjectId ${PROJECT_NAME})
project(${PROJECT_NAME})
include(win32)

ADD_DEFINITIONS(-DWINXP)

file(GLOB SOURCES 
                  "*.cpp"
                  "*.h"
                  "../Shared/*.cpp"
                  "../../include/*.h"
                  "../../include/*.hpp")
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# static link c runtime for
# hopefully compatibility with nt 5.2 x64
set_property(TARGET ${PROJECT_NAME}
                    PROPERTY
                    MSVC_RUNTIME_LIBRARY
                    "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(win32_install)
