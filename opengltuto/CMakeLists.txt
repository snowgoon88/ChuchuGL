# Make sure the compiler can find include files from our library.
include_directories (${CMAKE_SOURCE_DIR}/src)
# Define the libs used to compile the examples
set(TUTO_LIBS "-lSOIL ${GLEW_LDFLAGS} ${GLFW_LDFLAGS}")

# Define our examples to compile in variable ${usage_examples}
file(
        GLOB 
        usage_tuto
        *.cpp
)

# loop over the list to compile them
# Prefer to use 'targer_link' over  set_target_properties'
foreach(f ${usage_tuto})
    #message( STATUS "f=${f}" )
    get_filename_component(fileName ${f} NAME_WE) 
    add_executable (${fileName} ${f})
    target_link_libraries (${fileName} ${TUTO_LIBS})
    set_target_properties(${fileName} PROPERTIES LINKER_LANGUAGE CXX)
    #set_target_properties(${exampleName} PROPERTIES COMPILE_FLAGS "${PROJECT_ALL_CFLAGS} ${SHARE_PATH_CFLAGS}" LINK_FLAGS "${PROJECT_ALL_LDFLAGS}")
    set_target_properties(${fileName} PROPERTIES COMPILE_FLAGS "${PROJECT_ALL_CFLAGS} ${SHARE_PATH_CFLAGS}")
endforeach(f)
  