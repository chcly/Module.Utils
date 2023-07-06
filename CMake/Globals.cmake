option(Utils_BUILD_TEST           "Build the unit test program." ON)
option(Utils_AUTO_RUN_TEST        "Automatically run the test program." OFF)
option(Utils_USE_STATIC_RUNTIME   "Build with the MultiThreaded(Debug) runtime library." ON)
option(Utils_JUST_MY_CODE         "Enable the /JMC flag" ON)
option(Utils_OPEN_MP              "Enable low-level fill and copy using OpenMP" OFF)
option(Utils_DISABLE_CRT_WARNINGS "Disable CRT warnings" ON)


set(Utils_EXTRA_FLAGS )
add_definitions(-DNOMINMAX=1)


if (MSVC)
    if (Utils_DISABLE_CRT_WARNINGS)
        msvc_disable_crt_warnings()
    endif()

    # globally disable scoped enum warnings
    set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} /wd26812")
    set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} /W3")

    if (Utils_JUST_MY_CODE)
        # Enable just my code...
        set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} /JMC")
    endif ()

    set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} /fp:precise")
    set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} /fp:except")

    if (Utils_OPEN_MP)
        add_definitions(-DRT_OPEN_MP=1)
        set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} /openmp")
    endif()
    
else ()
    set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} -Os")
    set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} -O3")
    set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} -fPIC")

    if (Utils_OPEN_MP)
        add_definitions(-DRT_OPEN_MP=1)
        set(Utils_EXTRA_FLAGS "${Utils_EXTRA_FLAGS} -fopenmp")
    endif()

    link_libraries(stdc++fs)
endif ()
