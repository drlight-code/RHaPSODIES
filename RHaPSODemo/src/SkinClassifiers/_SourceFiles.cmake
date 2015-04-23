# $Id$

set( RelativeDir "src/SkinClassifiers" )
set( RelativeSourceGroup "source\\SkinClassifiers" )

set( DirFiles
	SkinClassifierLogOpponentYIQ.cpp
	SkinClassifierRedMatter0.cpp
	SkinClassifierRedMatter1.cpp
	SkinClassifierRedMatter2.cpp
	SkinClassifierRedMatter3.cpp
	SkinClassifierRedMatter4.cpp
	SkinClassifierRedMatter5.cpp
	SkinClassifierDhawale.cpp
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )
