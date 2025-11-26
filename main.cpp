#include "Logger/Logger.h"
#include "Main/Loop.h"
#include "Safety/Safety.h"
int main()
{
	sub::logger::InitializeLogger();
	sub::safety::Safety::EnableSafety(nullptr);
	int* x = nullptr;
	std::printf("%d\n", *x);
}