# Win32 compile settings
# When generating post cache in visual studio, CMAKE_BUILD_TYPE is always empty
# So I set them in `CMAKE_C_FLAGS` + `CMAKE_CXX_FLAGS` instead
# `target_compile_options` does not have a way to set for specific configs?

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
ADD_DEFINITIONS(-D_USRDLL)

# https://stackoverflow.com/questions/41264827/setting-optimization-settings-in-visual-studio-through-cmake
# https://github.com/fl0werD/rezombie/blob/fa28fc01bf5da5a22239587ca310e93b79cf3d20/cmake/Windows.cmake
set(OPT_DEB
""             # hack for visual studio??
"/Gw-"         # Whole-program global data optimization
"/Gy-"         # Function-level linking
"/Od"          # Disable optimization
"/Oy-"         # Omit frame pointer
"/Zi"          # Generate complete debugging information
)

message(STATUS "updating options for target: `${PROJECT_NAME}` `Debug` `${OPT_DEB}`")

foreach (link_line ${OPT_DEB})
    string(APPEND CMAKE_C_FLAGS_DEBUG " \"${link_line}\" ")
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " \"${link_line}\" ")
endforeach(link_line)

message(STATUS "updated options for target: `${PROJECT_NAME}` `Debug` `${OPT_DEB}`")

set(OPT_REL
# ""         # hack for visual studio??
"/O1"        # Create size code
"/Gy"        # Function-level linking
"/Ob1"       # Inline Function expansion (Only __inline (/Ob1))
"/Os"        # Favor size code
"/Oi"        # Generate intrinsic functions
"/Oy-"       # Omit frame pointer
"/Zi"        # Generate complete debugging information
"/Zo-"       # Generate richer debugging information for optimized code
)

message(STATUS "updating options for target: `${PROJECT_NAME}` `RelWithDebInfo` `${OPT_REL}`")

foreach (link_line ${OPT_REL})
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " \"${link_line}\" ")
    string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " \"${link_line}\" ")
endforeach(link_line)

message(STATUS "updated options for target: `${PROJECT_NAME}` `RelWithDebInfo` `${OPT_REL}`")

set(LINK_FLAGS
    "/SUBSYSTEM:WINDOWS"        # Prevents Visual Studio from starting console on debug start
)

# appending this on its own doesn't work
set(LINK_REL
    "/OPT:REF,ICF"              # Link optimizations
    "/INCREMENTAL:NO"           # Incremental linking
    "/LTCG"                     # Link-time code generation
    "${LINK_FLAGS}"
)

message(STATUS "Before RelDebInfo Linker flag: ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")

# turns out you have to set them in one line
# so here you go
foreach (link_line ${LINK_REL})
    string(APPEND CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO " \"${link_line}\" ")
endforeach(link_line)

message(STATUS "After RelDebInfo Linker flag: ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")

set(LINK_DEB
    "${LINK_FLAGS}"
)

message(STATUS "Before Debug Linker flag: ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")

# turns out you have to set them in one line
# so here you go
foreach (link_line ${LINK_DEB})
    string(APPEND CMAKE_SHARED_LINKER_FLAGS_DEBUG " \"${link_line}\" ")
endforeach(link_line)

message(STATUS "After Debug Linker flag: ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")

ADD_DEFINITIONS(-DPROJECT_NAME="${PROJECT_NAME}")
ADD_DEFINITIONS(-D_PROJECT_NAME=L"${PROJECT_NAME}")
