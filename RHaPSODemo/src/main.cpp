#include <iostream>

#include <RHaPSODemo.hpp>

using rhapsodies::RHaPSODemo;

int main(int argc, char** argv) {
  std::cout << "RHaPSODIES Demo Application -- for the SWAAARM!!"
            << std::endl;

  RHaPSODemo demo(argc, argv);

  if(!demo.Initialize()) {
    std::cerr << "failed to initialize RHaPSODemo. Aborting!"
              << std::endl;
    return -1;
  }

  return demo.Run();
}
