include(yaml-cpp)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT_NAME)
string(REPLACE " " "_" ProjectId ${PROJECT_NAME})
project(${PROJECT_NAME})
include(win32)

file(GLOB SOURCES "*.cpp" "*.h" "../Shared/*.cpp" "../../include/*.h" "../../include/*.hpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

include_directories(${YAML_CPP_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      ${YAML_CPP_LIBRARIES}
                      )

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        SUFFIX ".asi"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}"
)

install(
    FILES $<TARGET_PDB_FILE:${PROJECT_NAME}>
    "${CMAKE_CURRENT_SOURCE_DIR}/install.xml"
    DESTINATION ${PROJECT_NAME} OPTIONAL
)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${PROJECT_NAME}
)
