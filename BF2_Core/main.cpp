#include "CORE/BF_Core.h"

int main()
{
	Core engine;

	engine.init();
	engine.run();
	engine.shutdown();

	return 0;
}