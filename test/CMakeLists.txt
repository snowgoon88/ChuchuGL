# Make sure the compiler can find include files from our library.
include_directories (${CMAKE_SOURCE_DIR}/src 
  ${CMAKE_SOURCE_DIR}/include 
  ${EIGEN3_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${FTGL_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR})

# Define the libs used to compile the tests
set(TEST_LIBS "-lSOIL ${GLEW_LDFLAGS} ${GLFW_LDFLAGS} ${PA_LDFLAGS} ${VORBIS_LDFLAGS} ${FTGL_LDFLAGS} ${Boost_LIBRARIES}")

# Define our examples to compile in variable ${usage_test}
file(
        GLOB 
        usage_test
        *.cpp
)

# loop over the list to compile them
# Prefer to use 'targer_link' over  set_target_properties'
foreach(f ${usage_test})
    get_filename_component(fileName ${f} NAME_WE) 
    add_executable (${fileName} ${f})
    target_link_libraries (${fileName} ${TEST_LIBS} CHUCHU_LIB )
    set_target_properties(${fileName} PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(${fileName} PROPERTIES COMPILE_FLAGS "${PROJECT_ALL_CFLAGS} ${SHARE_PATH_CFLAGS} ${PA_CFLAGS}")
endforeach(f)
