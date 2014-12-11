# $Id:$

set( RelativeDir "src" )
set( RelativeSourceGroup "source" )

set( DirFiles
	main.cpp
	RHaPSODemo.cpp
  	ImageDraw.cpp
	TexturedQuadGLDraw.cpp
	ImagePBOOpenGLDraw.cpp
	DepthHistogramHandler.cpp
	HistogramUpdater.cpp
  	ShaderRegistry.cpp
  	HandTracker.cpp
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

