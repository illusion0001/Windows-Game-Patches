include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/illusion0001/yaml-cpp.git
  GIT_TAG 07212814f233201b3b2a9d3cabcba05d9cf9b891 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)

FetchContent_GetProperties(yaml-cpp)
FetchContent_MakeAvailable(yaml-cpp)

set(YAML_CPP_INCLUDE_DIR ${yaml-cpp_SOURCE_DIR}/include)
set(YAML_CPP_LIBRARIES yaml-cpp::yaml-cpp)
