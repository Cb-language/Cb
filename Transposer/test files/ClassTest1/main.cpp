#include <iostream>
#include <string>
#include "includes/Object.h"
#include "includes/String.h"
#include "includes/Array.h"
#include "includes/Primitive.h"
#include "includes/SafePtr.h"


#include "/home/schafiii/CLionProjects/kiryatgat-1401-c/Transposer/test files/ClassTest1/class.h"

Primitive<int> prelude()
{
	Primitive<int> x = Primitive<int>(Primitive<int>(0));
	Test t1 = 	Test(x);;
	Test2 t2 = 	Test2();;
	return Primitive<int>(0);
}
int main()
{
	return prelude().getValue();
}
