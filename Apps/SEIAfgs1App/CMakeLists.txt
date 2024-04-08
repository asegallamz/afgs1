# add the HM directory
add_subdirectory("External/HM" EXCLUDE_FROM_ALL)
target_compile_options(TLibCommon PUBLIC "-Wno-error=mismatched-new-delete")

# executable
set( EXE_NAME SEIAfgs1App )

# get source files
file( GLOB SRC_FILES "*.cpp" )

# get include files
file( GLOB INC_FILES "*.h" )

# get additional libs for gcc on Ubuntu systems
if( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
  if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    if( USE_ADDRESS_SANITIZER )
      set( ADDITIONAL_LIBS asan )
    endif()
  endif()
endif()

# NATVIS files for Visual Studio
if( MSVC )
  file( GLOB NATVIS_FILES "../../VisualStudio/*.natvis" )
endif()

# add executable
add_executable( ${EXE_NAME} ${SRC_FILES} ${INC_FILES} )

# include the output directory, where the svnrevision.h file is generated
include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries( ${EXE_NAME} LibAFGS1 TLibCommon TLibDecoder TLibEncoder Utilities ${ADDITIONAL_LIBS} )

#if( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
#  add_custom_command( TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
#                                                          $<$<CONFIG:Debug>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/SEIAfgs1App>
#                                                          $<$<CONFIG:Release>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/SEIAfgs1App>
#                                                          $<$<CONFIG:RelWithDebInfo>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}/SEIAfgs1App>
#                                                          $<$<CONFIG:MinSizeRel>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL}/SEIAfgs1App>
#                                                          $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/bin/SEIAfgs1AppStaticd>
#                                                          $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/bin/SEIAfgs1AppStatic>
#                                                          $<$<CONFIG:RelWithDebInfo>:${CMAKE_SOURCE_DIR}/bin/SEIAfgs1AppStaticp>
#                                                          $<$<CONFIG:MinSizeRel>:${CMAKE_SOURCE_DIR}/bin/SEIAfgs1AppStaticm> )
#endif()

# set the folder where to place the projects
set_target_properties( ${EXE_NAME}  PROPERTIES FOLDER app LINKER_LANGUAGE CXX )
