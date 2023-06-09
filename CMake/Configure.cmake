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
include(GitUpdate)
if (NOT GitUpdate_SUCCESS)
    return()
endif()

include(StaticRuntime)
include(ExternalTarget)
include(GTestUtils)
include(Globals)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (Utils_USE_STATIC_RUNTIME)
    set_static_runtime()
else()
    set_dynamic_runtime()
endif()

configure_gtest(${Utils_SOURCE_DIR}/Test/googletest 
                ${Utils_SOURCE_DIR}/Test/googletest/googletest/include)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Utils_EXTRA_FLAGS}")

set(Configure_SUCCEEDED TRUE)