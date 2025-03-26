#include <iostream>
#include <vector>

#include "application.h"

using namespace Neu;

int main(int argc, char** argv) {
  std::cout << "RUNNING NEU DEMO" << std::endl;

  Application* app = Application::createInstance(argc, argv);
  app->setup();
  app->start();
  app->terminate();
};