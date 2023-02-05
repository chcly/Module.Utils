cmake_minimum_required(VERSION 3.15)
project(Utils)

# Use C++-17 by default.
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set the module path to include scripts
# CMake - Contains project local modules.
#
# CMake/Modules - Contains project independent modules.
#   GitUpdate.cmake invokes python ${Utils_SOURCE_DIR}/gitupdate.py
#   to pull all submodules and update to head.
#   If this fails, it will attempt to invoke git directly to pull 
#   submodules. 
list(APPEND CMAKE_MODULE_PATH 
  ${Utils_SOURCE_DIR}/CMake 
  ${Utils_SOURCE_DIR}/CMake/Modules
)

include(${Utils_SOURCE_DIR}/CMake/Configure.cmake)
if (NOT Configure_SUCCEEDED)
   return()
endif()


if (Utils_ExternalTarget)
    set(TargetFolders ${Utils_TargetFolders})
    set(TargetName    ${Utils_TargetName})
    set(TargetGroup   ${Utils_TargetGroup})
else()
    add_subdirectory(CMake)

    set(TargetFolders TRUE)
    set(TargetName Utils)
    set(TargetGroup )

    set(Utils_INCLUDE  ${Utils_SOURCE_DIR})
    set(Utils_LIBRARY  ${TargetName})

endif()

add_subdirectory(Utils)

if (Utils_BUILD_TEST)
    if (NOT Utils_ExternalTarget)
        add_subdirectory(${GTEST_DIR})
        set_target_properties(gtest_main PROPERTIES FOLDER "${TargetGroup}")
        set_target_properties(gtest      PROPERTIES FOLDER "${TargetGroup}")
    endif()
    
    set(TargetGroup Units)
    add_subdirectory(Test)
endif()
