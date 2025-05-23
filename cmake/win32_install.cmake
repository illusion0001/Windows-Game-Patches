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
    DESTINATION ${PROJECT_NAME} OPTIONAL
)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${PROJECT_NAME}
)
