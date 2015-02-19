# $Id$

set( RelativeDir "src/GLDraw" )
set( RelativeSourceGroup "source\\GLDraw" )

set( DirFiles
  	ImageDraw.cpp
	TexturedQuadGLDraw.cpp
	ImagePBOOpenGLDraw.cpp
  	HandRenderDraw.cpp
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )
