
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.4 )

project (Chuchu)

#######################################
# Get the libs/inc for the dependencies
#######################################
find_package(PkgConfig REQUIRED)

pkg_search_module(GLEW REQUIRED glew)
#message( STATUS "GLEW_CFLAGS ${GLEW_CFLAGS}")
#message( STATUS "GLEW_LDFLAGS ${GLEW_LDFLAGS}")
#message( STATUS "GLEW_LIBS ${GLEW_LIBS}")
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(FTGL REQUIRED ftgl)

#######################################
# Setting the compilation flags
#######################################

# cflags added by the package
SET(PROJECT_CFLAGS "-Wall -std=c++11")

# cflags added by the pkg-config dependencies contains ';' as separator. This is a fix.
string(REPLACE ";" " " GLEW_CFLAGS "${GLEW_CFLAGS}")
string(REPLACE ";" " " GLFW_CFLAGS "${GLFW_CFLAGS}")

# ldflasgs added by the pkg-config dependencies contains ';' as separator. This is a fix.
string(REPLACE ";" " " GLEW_LDFLAGS "${GLEW_LDFLAGS}")
string(REPLACE ";" " " GLFW_LDFLAGS "${GLFW_LDFLAGS}")

# Gathering of all flags
# (e.g. for compiling examples)
SET(PROJECT_ALL_CFLAGS  "${PROJECT_CFLAGS}" )

## Set of libraries for tests
SET(TESTS_LIBS "")

###################################
#  Subdirectories
###################################
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(opengltuto)
