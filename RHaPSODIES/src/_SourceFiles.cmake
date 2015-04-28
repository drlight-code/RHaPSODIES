# $Id:$

set( RelativeDir "src" )
set( RelativeSourceGroup "Source Files" )
set( SubDirs PSO SkinClassifiers )

set( DirFiles
	RHaPSODIES.cpp
	ShaderRegistry.cpp
	HandGeometry.cpp
	HandModel.cpp
	HandRenderer.cpp
	HandTracker.cpp
	HandTrackingNode.cpp
	CameraFramePlayer.cpp
	CameraFrameRecorder.cpp
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

set( SubDirFiles "" )
foreach( Dir ${SubDirs} )
	list( APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake" )
endforeach()

foreach( SubDirFile ${SubDirFiles} )
	include( ${SubDirFile} )
endforeach()

