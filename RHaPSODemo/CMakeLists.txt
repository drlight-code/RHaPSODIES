# $Id:$

cmake_minimum_required( VERSION 2.8 )
project( RHaPSODemo )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaCommon )

vista_set_version( RHaPSODemo HEAD HEAD 0 1 0 svn_rev )

vista_use_package( VistaCoreLibs "HEAD" REQUIRED FIND_DEPENDENCIES )
#vista_use_package( Vfl2DDiagrams REQUIRED FIND_DEPENDENCIES )
vista_use_package( OpenCV REQUIRED FIND_DEPENDENCIES )

set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -pg")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.3 or greater.")
    endif ()
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

# Including the source files of all source subfolders recursively
include( "src/_SourceFiles.cmake" )
include_directories( "src/" )

add_executable( RHaPSODemo ${ProjectSources} )
target_link_libraries( RHaPSODemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

#//vista_add_pathscript_envvar(  _VALUE [PATH_LIST] )

vista_configure_app( RHaPSODemo )
vista_create_default_info_file( RHaPSODemo )
