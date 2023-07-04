
if (NOT MSVC)
    return()
endif()

include(GroupSet)
set(TargetName ALL_CMAKE)
set(TargetGroup CMakePredefinedTargets)
set(GroupRoot ../)

set_ro_group(TargetName_SRC
    :${GroupRoot}/CMakeLists.txt
    :${GroupRoot}/README.md
    Actions:${GroupRoot}/.github/workflows/*.yml
    Style:${GroupRoot}/.clang-format
    Config:${GroupRoot}/CMake/CMakeLists.txt
    Config:${GroupRoot}/CMake/*.cmake
    Config/Modules:${GroupRoot}/CMake/Modules/*.cmake
    Python:${GroupRoot}/*.py
    Python:${GroupRoot}/CMake/*.py
)

add_custom_target(
    ${TargetName} 
    ALL SOURCES ${TargetName_SRC}
    )

set_target_properties(
    ${TargetName} 
    PROPERTIES FOLDER "${TargetGroup}"
)