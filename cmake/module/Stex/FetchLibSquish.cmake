function(fetch_libsquish svn_rev)
    include(FetchContent)

    # Make sure static libs are used
    set(BUILD_SHARED_LIBS OFF)

    FetchContent_Declare(LIBSQUISH
        SVN_REPOSITORY  svn://svn.code.sf.net/p/libsquish/code/trunk
        SVN_REVISION    -r${svn_rev}
    )

    # Sadly, libsquish's CMake's compatability with FetchContent is (understandably given its age) garbage, as it
    # was written before modern CMake practices emerged, and isn't being updated anymore; therefore, some work
    # must be done to fetch it in a way that makes it more easily consumeable. Because of this,
    # FetchContent_MakeAvailable cannot be used and instead FetchContent_Populate must be used in addition to
    # some manual configuration.

    # Prevent libsquish from overwriting "BUILD_SHARED_LIBS" with its "option" version (old practice)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

    # Populate libsquish build
    FetchContent_GetProperties(LIBSQUISH)
    if(NOT libsquish_POPULATED)
        FetchContent_Populate(LIBSQUISH)

        # EXCLUDE_FROM_ALL so that only main squish library gets built since it's a dependency, ignore extras, install, etc.
        add_subdirectory(${libsquish_SOURCE_DIR} ${libsquish_BINARY_DIR} EXCLUDE_FROM_ALL)

        # Copy the headers to a different directory so that they're more friendly to include
        set(__FRIENDLY_PH_DIR_NAME "Squish")
        set(__FRIENDLY_PH_ROOT "${libsquish_BINARY_DIR}/__public_headers")
        set(__FRIENDLY_PH_DIR "${__FRIENDLY_PH_ROOT}/${__FRIENDLY_PH_DIR_NAME}")
        file(GLOB __SQUISH_PUBLIC_HEADERS "${libsquish_SOURCE_DIR}/*.h")
        file(COPY ${__SQUISH_PUBLIC_HEADERS} DESTINATION "${__FRIENDLY_PH_DIR}")
    endif()

    # Add public headers to consumers
    target_include_directories(squish INTERFACE "${__FRIENDLY_PH_ROOT}")

    # Create a reasonable alias for the target
    add_library(Squish::Squish ALIAS squish)
endfunction()
