include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/illusion0001/yaml-cpp.git
  GIT_TAG 1e9ee6e0e8329dc1b039d8c43195df8fcfff1924 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)

FetchContent_GetProperties(yaml-cpp)
FetchContent_MakeAvailable(yaml-cpp)

set(YAML_CPP_INCLUDE_DIR ${yaml-cpp_SOURCE_DIR}/include)
set(YAML_CPP_LIBRARIES yaml-cpp::yaml-cpp)
