# -----------------------------------------------------------------------------
#
#   Copyright (c) Charles Carley.
#
#   This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
# ------------------------------------------------------------------------------
set(TestTargetName ${TargetName}Test)

include(GroupSet)
include(TestDirectory)
gen_path_to_here()

set_group(
   TestTarget_SOURCE
   Source:*cpp
)

include_directories(
    ${GTEST_INCLUDE} 
    ${Utils_INCLUDE})

add_executable(
    ${TestTargetName}
    ${TestTarget_SOURCE}
)

target_link_libraries(
    ${TestTargetName} 
    ${GTEST_LIBRARY} 
    ${Utils_LIBRARY})


set_target_properties(
    ${TestTargetName} 
    PROPERTIES FOLDER "${TargetGroup}"
)

if (Utils_AUTO_RUN_TEST)
    run_test(${TestTargetName})
endif()
