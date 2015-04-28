# $Id:$

cmake_minimum_required( VERSION 2.8 )
project( RHaPSODaemon )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaCommon )

vista_set_version( RHaPSODaemon HEAD MASTER 0 1 0 )

vista_use_package( VistaCoreLibs "HEAD" REQUIRED FIND_DEPENDENCIES )
vista_use_package( RHaPSODIES "MASTER" REQUIRED FIND_DEPENDENCIES )

# Including the source files of all source subfolders recursively
include( "src/_SourceFiles.cmake" )

add_executable( RHaPSODaemon ${ProjectSources} )
target_link_libraries( RHaPSODaemon
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( RHaPSODaemon )
vista_create_default_info_file( RHaPSODaemon )

