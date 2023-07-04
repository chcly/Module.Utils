set(TargetName Utils)
include (GroupSet)

set_group(Utils_SRC
    Source:*.hint
    Source:*.cpp
    Source/CommandLine:CommandLine/*.cpp
    Source/Directory:Directory/*.cpp
    Source/LeakCheck:Win32/*.cpp
    Header:*.h
    Header/CommandLine:CommandLine/*.h
    Header/Directory:Directory/*.h
    Header/LeakCheck:Win32/*.h
    Header/Stream:Streams/*.h
    Header/Stream:*Stream*.h
    Header/Stream/Converters:StreamConverters/*.h
    )


include_directories(${Utils_SOURCE_DIR})

add_library(
    ${TargetName}
    ${Utils_SRC} 
)

set_target_properties(
    ${TargetName} 
    PROPERTIES FOLDER "${TargetGroup}"
)
