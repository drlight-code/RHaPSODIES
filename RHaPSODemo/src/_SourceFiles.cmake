# $Id:$

set( RelativeDir "src" )
set( RelativeSourceGroup "source" )
set( SubDirs SkinClassifiers )

set( DirFiles
	main.cpp
	RHaPSODemo.cpp
  	ImageDraw.cpp
	TexturedQuadGLDraw.cpp
	ImagePBOOpenGLDraw.cpp
	DepthHistogramHandler.cpp
	HistogramUpdater.cpp
  	ShaderRegistry.cpp
  	HandModel.cpp
  	HandRenderer.cpp
  	HandRenderDraw.cpp
  	HandTracker.cpp
  	HandTrackingNode.cpp
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
