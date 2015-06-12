## The files that make this library
set( SRC_FILES chuchu.cpp cell.cpp )

add_library( CHUCHU_LIB ${SRC_FILES} )
set_target_properties( CHUCHU_LIB PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties( CHUCHU_LIB PROPERTIES COMPILE_FLAGS "${PROJECT_ALL_CFLAGS} ${SHARE_PATH_CFLAGS}")
## Nothing yet
