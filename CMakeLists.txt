cmake_minimum_required(VERSION 3.15)
project(InternalUtils)

# Use C++-17 by default.
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set the module path to include scripts
# CMake - Contains project local modules.
#
# CMake/Modules - Contains project independent modules.
#   GitUpdate.cmake invokes python ${InternalUtils_SOURCE_DIR}/gitupdate.py
#   to pull all submodules and update to head.
#   If this fails, it will attempt to invoke git directly to pull 
#   submodules. 
list(APPEND CMAKE_MODULE_PATH 
  ${InternalUtils_SOURCE_DIR}/CMake 
  ${InternalUtils_SOURCE_DIR}/CMake/Modules
)

include(Configure)
if (NOT Configure_SUCCEEDED)
   return()
endif()


if (InternalUtils_ExternalTarget)
    set(TargetFolders ${InternalUtils_TargetFolders})
    set(TargetName    ${InternalUtils_TargetName})
    set(TargetGroup   ${InternalUtils_TargetGroup})
else()
    add_subdirectory(CMake)

    set(TargetFolders TRUE)
    set(TargetName InternalUtils)
    set(TargetGroup )

    set(InternalUtils_INCLUDE  ${InternalUtils_SOURCE_DIR}/Source)
    set(InternalUtils_LIBRARY  ${TargetName})

endif()

add_subdirectory(Utils)

if (InternalUtils_BUILD_TEST)
    add_subdirectory(${GTEST_DIR})

    set(TargetGroup Units)
    if (TargetFolders)
        set_target_properties(gtest_main PROPERTIES FOLDER "${TargetGroup}")
        set_target_properties(gtest      PROPERTIES FOLDER "${TargetGroup}")
    endif()
    add_subdirectory(Test)
endif()
