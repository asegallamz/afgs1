# library
set( LIB_NAME LibAFGS1 )

# get source files
file( GLOB_RECURSE SRC_FILES "." "*.cpp" )

# get include files
file( GLOB_RECURSE INC_FILES "." "*.h" )

# library
add_library( ${LIB_NAME} STATIC ${SRC_FILES} ${INC_FILES} )

# set the folder where to place the projects
set_target_properties( ${LIB_NAME} PROPERTIES FOLDER lib )
