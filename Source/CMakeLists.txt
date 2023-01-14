set(TargetName_SRC  Main.cpp)

add_executable(
    ${TargetName} 
    ${TargetName_SRC}
)
set_target_properties(${TargetName} PROPERTIES FOLDER "${TargetGroup}")
