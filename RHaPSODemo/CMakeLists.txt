# $Id:$

cmake_minimum_required( VERSION 2.8 )
project( RHaPSODemo )

list( APPEND CMAKE_MODULE_PATH "$ENV{VISTA_CMAKE_COMMON}" )

include( VistaCommon )

vista_set_version( RHaPSODemo HEAD HEAD 0 1 0 svn_rev )
vista_use_package( VistaCoreLibs "HEAD" REQUIRED FIND_DEPENDENCIES )


# Including the source files of all source subfolders recursively
include( "src/_SourceFiles.cmake" )

add_executable( RHaPSODemo ${ProjectSources} )
target_link_libraries( RHaPSODemo
	${VISTA_USE_PACKAGE_LIBRARIES} # contains all libraries from vista_use_package() calls
)

vista_configure_app( RHaPSODemo )
vista_create_default_info_file( RHaPSODemo )
