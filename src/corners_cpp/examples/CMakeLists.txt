cmake_minimum_required( VERSION 2.8.4 )
project( hough_example )
set( pname hough_example )
set( pversion "0.0.250" )
set( tool_EXE ${pname} )
include( FindPkgConfig )
find_package( OpenCV REQUIRED )

set(${OpenCV_INCLUDE_DIRS} "/home/satvik/Software/opencv4/OpenCV/include/opencv4/opencv2/")
set(${OpenCV_LIBS}  "/home/satvik/Software/opencv4/OpenCV/build/lib/")
find_package(OpenCV 4.2.0 REQUIRED)

file( GLOB tool_SRC "*.cpp" )
add_executable( ${tool_EXE} ${tool_SRC} )

set( cfs "-Wall -W -DDEBUG" )
set_target_properties( ${tool_EXE} PROPERTIES COMPILE_FLAGS "${cfs}" )

target_link_libraries( ${tool_EXE} m ${OpenCV_LIBS} )

install( TARGETS ${tool_EXE} DESTINATION bin )

add_custom_target( realclean cd ${CMAKE_HOME_DIRECTORY} COMMAND rm -fr ${tool_EXE} ${tool_EXE}.exe CMakeFiles CMakeCache.txt cmake_install.cppmake install_manifest.txt Makefile )
