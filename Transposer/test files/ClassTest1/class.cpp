#include <iostream>
#include "//home/schafiii/CLionProjects/kiryatgat-1401-c/Transposer/test files/ClassTest1/class.h"

Test::Test(int x)
{
	do_x = x;
	fa_n = x * x + 0.01;
}

Test::Test()
{
	do_x = 1;
}

void Test::f()
{
	std::cout << "Hello World!" << std::endl;
}

int Test::g(int z)
{
	f();
	return z * z;
}

