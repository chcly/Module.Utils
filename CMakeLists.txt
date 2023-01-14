cmake_minimum_required(VERSION 3.15)
project(Template)

# Use C++-17 by default.
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set the module path to include scripts
# CMake - Contains project local modules.
#
# CMake/Modules - Contains project independent modules.
#   GitUpdate.cmake invokes python ${Template_SOURCE_DIR}/gitupdate.py
#   to pull all submodules and update to head.
#   If this fails, it will attempt to invoke git directly to pull 
#   submodules. 
list(APPEND CMAKE_MODULE_PATH 
  ${Template_SOURCE_DIR}/CMake 
  ${Template_SOURCE_DIR}/CMake/Modules
)
include(Configure)
if (NOT Configure_SUCCEEDED)
   return()
endif()



if (Template_ExternalTarget)
    set(TargetFolders ${Template_TargetFolders})
    set(TargetName    ${Template_TargetName})
    set(TargetGroup   ${Template_TargetGroup})
else()
    add_subdirectory(CMake)

    set(TargetFolders TRUE)
    set(TargetName Template)
    set(TargetGroup )

    set(Template_INCLUDE  ${Template_SOURCE_DIR}/Source)
    set(Template_LIBRARY  ${TargetName})
endif()

add_subdirectory(Source)

if (Template_BUILD_TEST)
    add_subdirectory(${GTEST_DIR})

    set(TargetGroup Units)
    if (TargetFolders)
        set_target_properties(gtest_main PROPERTIES FOLDER "${TargetGroup}")
        set_target_properties(gtest      PROPERTIES FOLDER "${TargetGroup}")
    endif()
    add_subdirectory(Test)
endif()
