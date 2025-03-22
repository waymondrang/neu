#include "application.h"

int main(int argc, char** argv)
{
	Application* application = Application::createInstance(argc, argv);

	///////////////////////////
	// APPLICATION LIFECYCLE //
	///////////////////////////

	application->setup();
	application->start();
	application->terminate();

	return 0;
}