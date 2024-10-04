#================= Common Build =========================

# Add via ob standard executable
include(OB/Executable)
ob_add_standard_executable(${APP_TARGET_NAME}
    NAMESPACE "${PROJECT_NAMESPACE}"
    ALIAS "${APP_ALIAS_NAME}"
    SOURCE
        command/command.h
        command/command.cpp
        command/c-compress.h
        command/c-compress.cpp
        command/c-decompress.h
        command/c-decompress.cpp
        command/c-pack.h
        command/c-pack.cpp
        command/c-pack.h
        command/c-unpack.h
        command/c-unpack.cpp
        command/tex-command.h
        command/tex-command.cpp
        command/untex-command.h
        command/untex-command.cpp
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
    LINKS
        PRIVATE
            Qt6::Core
            Qt6::Gui
            Qx::Core
            Qx::Io
            Qx::Xml
            libsquish::Squish
    CONFIG STANDARD
)

## Forward select project variables to C++ code
include(OB/CppVars)
ob_add_cpp_vars(${APP_TARGET_NAME}
    NAME "project_vars"
    PREFIX "PROJECT_"
    VARS
        VERSION_STR "\"${PROJECT_VERSION}\""
        SHORT_NAME "\"${PROJECT_NAME}\""
        APP_NAME "\"${PROJECT_FORMAL_NAME}\""
)

## Add exe details on Windows
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    include(OB/WinExecutableDetails)
    ob_set_win_executable_details(${APP_TARGET_NAME}
        ICON "res/icon/Stex.ico"
        FILE_VER ${PROJECT_VERSION}
        PRODUCT_VER ${PROJECT_VERSION}
        COMPANY_NAME "oblivioncth"
        FILE_DESC "${PROJECT_DESCRIPTION}"
        INTERNAL_NAME ${PROJECT_NAME}
        COPYRIGHT "Open Source @ 2023 oblivioncth"
        TRADEMARKS_ONE "All Rights Reserved"
        TRADEMARKS_TWO "GNU AGPL V3"
        ORIG_FILENAME "${PROJECT_NAME}.exe"
        PRODUCT_NAME "${PROJECT_FORMAL_NAME}"
    )
endif()
