cmake_minimum_required      ( VERSION 3.12.0 )

set                         ( PROJECT_NAME                       lib )
set                         ( SRC_FILES                          lib.cpp)
file                        ( GLOB_RECURSE 
                              INC_ST_FILES                       tree_search/*.hpp )
file                        ( GLOB 
                              INC_OT_FILES                       tree_other/*.hpp )


set                         ( INC_FILES ${INC_ST_FILES} 
                                        ${INC_OT_FILES})

# compiler/linker flags
string                      ( APPEND CMAKE_CXX_FLAGS             " /EHsc" )
string                      ( APPEND CMAKE_CXX_FLAGS             " /std:c++14" )
string                      ( APPEND CMAKE_SHARED_LINKER_FLAGS   " /SAFESEH:NO" )
message                     ( STATUS                             "CXX flags ${CMAKE_CXX_FLAGS}" )
message                     ( STATUS                             "Linker flags ${CMAKE_SHARED_LINKER_FLAGS}" )
         
project                     ( ${PROJECT_NAME} )

include_directories         ( ${CMAKE_CURRENT_SOURCE_DIR} )
     

add_library                 ( ${PROJECT_NAME} STATIC             ${SRC_FILES} 
                                                                 ${INC_FILES} )

source_group                (tree_search      FILES              ${INC_ST_FILES})
source_group                (tree_other       FILES              ${INC_OT_FILES})

target_link_libraries       ( ${PROJECT_NAME}                     )
