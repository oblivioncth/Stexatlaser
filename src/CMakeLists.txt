#================= Common Build =========================

# Allow includes relative to source root
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Get path to resources
set(RESOURCE_PATH "${PROJECT_SOURCE_DIR}/res")

# Add executable target
qt_add_executable(${MAIN_TARGET_NAME})

# Make alias target so target can be referred to with its friendly
# export name both internally and when part of another build tree
add_executable(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${MAIN_TARGET_NAME})

# Set app sources
set(CXX_SOURCES
    command.h
    command.cpp
    command/c-pack.h
    command/c-unpack.cpp
    klei/k-atlas.h
    klei/k-atlas.cpp
    klei/k-atlaskey.h
    klei/k-atlaskey.cpp
    klei/k-tex-io.h
    klei/k-tex-io.cpp
    klei/k-tex.h
    klei/k-tex.cpp
    klei/k-xml.h
    klei/k-xml.cpp
    stex.h
    stex.cpp
    conversion.h
    conversion.cpp
    main.cpp
)

# Add sources to target
target_sources(${MAIN_TARGET_NAME}
    PRIVATE
        ${CXX_SOURCES}
)

# Link target to libraries
target_link_libraries(${MAIN_TARGET_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qx::Core
        Qx::Io
        Squish::Squish
)

# Configure target properties
set_target_properties(${MAIN_TARGET_NAME} PROPERTIES
    EXPORT_NAME "${PROJECT_NAME}"
)

################# Windows Build #################
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    # Target properties
    # Only set the target version on Windows because on Linux it
    # causes the executable to have the version number as a suffix
    # (doesn't seem like this can be disabled). The number is set
    # on the target primarily for posterity anyway as it only tags
    # the binary with the version in a place that doesn't seem to
    # be user visible or really used in any significant way
    set_target_properties(${MAIN_TARGET_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
    )

    # Configure target properties
    set_target_properties(${MAIN_TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}"
    )

    # Set target exe details
    include(OB/WinExecutableDetails)
    set_win_executable_details(${MAIN_TARGET_NAME}
        ICON "${RESOURCE_PATH}/app/CLIFp.ico"
        FILE_VER ${PROJECT_VERSION}
        PRODUCT_VER ${PROJECT_VERSION}
        COMPANY_NAME "oblivioncth"
        FILE_DESC "CLI for BlueMaxima's Flashpoint"
        INTERNAL_NAME ${PROJECT_NAME}
        COPYRIGHT "Open Source @ 2023 oblivioncth"
        TRADEMARKS_ONE "All Rights Reserved"
        TRADEMARKS_TWO "GNU AGPL V3"
        ORIG_FILENAME "${PROJECT_NAME}.exe"
        PRODUCT_NAME "${FORMAL_NAME}"
    )
endif()

################# Linux Build #################
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    # Configure target properties
    set_target_properties(${MAIN_TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME_LC}"
    )
endif()

#================= Install ==========================

# Install executable
install(TARGETS ${MAIN_TARGET_NAME}
    CONFIGURATIONS Release
    COMPONENT ${MAIN_TARGET_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ${SUB_PROJ_EXCLUDE_FROM_ALL} # "EXCLUDE_FROM_ALL" if project is not top-level
    RUNTIME DESTINATION bin
)
