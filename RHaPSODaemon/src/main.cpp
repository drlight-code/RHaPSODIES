#include <iostream>

#include <GL/freeglut.h>

#include <VistaBase/VistaStreamUtils.h>

#include "RHaPSODaemon.hpp"

int main(int argc, char **argv) {
	rhapsodies::RHaPSODaemon daemon;
	
	vstr::out() << "RHaPSODaemon proud to serve!" << std::endl;

	glutInit(&argc, argv);

	bool success = daemon.Initialize();
	if(!success) {
		vstr::err() << "Failed to initialize RHaPSODaemon! Aborting."
					<< std::endl;
		exit(1);
	}

	daemon.Run();
	
	return 0;
}
