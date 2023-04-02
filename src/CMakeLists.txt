set( CMAKE_CXX_STANDARD 17 )

find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )

include_directories( ${Qt5Core_INCLUDE_DIRS} )
include_directories( ${Qt5Widgets_INCLUDE_DIRS} )

set(CMAKE_AUTOMOC ON)

file( GLOB SOURCES *.cpp )
file( GLOB HEADERS *.h )

add_executable( dijkstras_train ${HEADERS} ${SOURCES} )
target_link_libraries( dijkstras_train Qt5::Core Qt5::Widgets )
install( TARGETS dijkstras_train RUNTIME DESTINATION bin )

add_executable( dijkstra_test test/dijkstra_test.cpp digraph.cpp dijkstra.cpp )
install( TARGETS dijkstra_test RUNTIME DESTINATION bin )
