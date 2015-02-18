#include <iostream>

#include <RHaPSODemo.hpp>

using rhapsodies::RHaPSODemo;

int main(int argc, char** argv) {
	std::cout << std::endl
			  << "RHaPSODIES Demo Application -- for the SWAAARM!!"
			  << std::endl;

	RHaPSODemo demo;

	if(!demo.Initialize(argc, argv)) {
		std::cerr << "Failed to initialize RHaPSODemo! Aborting."
				  << std::endl;
		return -1;
	}

	return demo.Run();
}
