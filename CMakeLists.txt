cmake_minimum_required          ( VERSION 3.12.0 )
project(algorithms)

if( NOT DEFINED BUILDDIR )
    set                         ( BUILDDIR          ${CMAKE_CURRENT_SOURCE_DIR}/bin )
    message                     ( STATUS            "BUILDDIR is undefined. Setting it to ${BUILDDIR}. Use -DBUILDDIR=[path] to redefine it" )
else()
    file                        (TO_CMAKE_PATH      "${BUILDDIR}" BUILDDIR)
    message                     ( STATUS            "BUILDDIR = ${BUILDDIR}" )
endif()

add_subdirectory                ( src )
unset                           ( src CACHE )
