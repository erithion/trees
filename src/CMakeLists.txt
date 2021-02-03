cmake_minimum_required      ( VERSION 3.12.0 )
set_property                ( GLOBAL PROPERTY USE_FOLDERS ON)

include                     (${CMAKE_CURRENT_SOURCE_DIR}/lib.cmake)
add_subdirectory            ( tree_search_tests )
add_subdirectory            ( tree_other_tests )
unset                       ( tree_search_tests CACHE )
unset                       ( tree_other_tests CACHE )
