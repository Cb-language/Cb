#include <iostream>
#include "/home/schafiii/CLionProjects/kiryatgat-1401-c/Transposer/test files/ClassTest1/class.h"

Test::Test(Primitive<int> x)
{
	do_x = x;
	std::cout << String("entered ctor") << std::endl;
}

void Test::f()
{
	std::cout << String("Hello World!") << std::endl;
}


Test2::Test2()
{
	do_y = Primitive<int>(2);
}

