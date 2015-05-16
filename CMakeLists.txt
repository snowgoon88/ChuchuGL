
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.4 )

project (Chuchu)

#######################################
# Get the libs/inc for the dependencies
#######################################
find_package(PkgConfig REQUIRED)

pkg_search_module(GL REQUIRED gl)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(FTGL REQUIRED ftgl)

#######################################
# Setting the compilation flags
#######################################

# cflags added by the package
SET(PROJECT_CFLAGS "-Wall -std=c++11")

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
