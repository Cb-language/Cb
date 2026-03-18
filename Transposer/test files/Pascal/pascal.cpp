#include <iostream>
#include <string>
#include "includes/Object.h"
#include "includes/Utils.h"
#include "includes/String.h"
#include "includes/Array.h"
#include "includes/Primitive.h"
#include "includes/SafePtr.h"

void pascal(Primitive<unsigned int> size);
Primitive<int> prelude()
{
	{
	Primitive<unsigned int> size;
	std::cout << String("Enter pascal triangle level") << std::endl;
	std::cin >> size;
	pascal(size);
	return Primitive<int>(0);
	}
}

void pascal(Primitive<unsigned int> size)
{
	{
	Array<Array<Primitive<unsigned int>>> arr = Array<Array<Primitive<unsigned int>>>(size, Array<Primitive<unsigned int>>(size, Primitive<unsigned int>()));
	(arr[Primitive<int>(0)][Primitive<int>(0)] = Primitive<int>(1));

	for (Primitive<int> i = Primitive<int>(1); i < size; i += Primitive<int>(1))
	{
		(arr[i][Primitive<int>(0)] = Primitive<int>(1));
		for (Primitive<int> j = Primitive<int>(1); j < size; j += Primitive<int>(1))
		{
			(arr[i][j] = (arr[(i - Primitive<int>(1))][(j - Primitive<int>(1))] + arr[(i - Primitive<int>(1))][j]));

		}
	}
	for (Primitive<int> i = Primitive<int>(0); i < size; i += Primitive<int>(1))
	{
		std::cout << String("[");		for (Primitive<int> j = Primitive<int>(0); j < size; j += Primitive<int>(1))
		{
			std::cout << String("[") << arr[i][j] << String("]");			if ((j != (size - Primitive<int>(1))))
			{
				std::cout << String(", ");
			}
		}		std::cout << String("]");		if ((i != (size - Primitive<int>(1))))
		{
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
	}
}
int main()
{
	try{return prelude().getValue();}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}
}
