set( EXE_NAME T35Afgs1App )
add_executable(${EXE_NAME} T35Afgs1App.cpp)
target_link_libraries( ${EXE_NAME} LibAFGS1 )