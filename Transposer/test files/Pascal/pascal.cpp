#include <iostream>
#include <string>
#include "includes/Object.h"
#include "includes/String.h"
#include "includes/Array.h"
#include "includes/Primitive.h"
#include "includes/SafePtr.h"

void pascal(Primitive<unsigned int> size);
Primitive<int> prelude()
	{
	Primitive<unsigned int> size;
	std::cout << String("Enter pascal triangle level") << std::endl;
	std::cin >> size;
	pascal(size);
	return Primitive<int>(0);
	}

void pascal(Primitive<unsigned int> size)
	{
	Array<Array<Primitive<unsigned int>>> arr = Array<Array<Primitive<unsigned int>>>(size, Array<Primitive<unsigned int>>(size, Primitive<unsigned int>()));
	arr[Primitive<int>(0)][Primitive<int>(0)] = Primitive<int>(1);
	String b = String("]");
	for (Primitive<int> i = Primitive<int>(1); i < size; i += Primitive<int>(1))
		{
		for (Primitive<int> j = Primitive<int>(0); j < size; j += Primitive<int>(1))
			{
			if (j == Primitive<int>(0))
			{
				arr[i][j] = arr[i - Primitive<int>(1)][j];
			}else 
			{
				arr[i][j] = arr[i - Primitive<int>(1)][j - Primitive<int>(1)] + arr[i - Primitive<int>(1)][j];
			}
			}
		for (Primitive<int> i = Primitive<int>(0); i < size; i += Primitive<int>(1))
			{
			std::cout << String("[");
			for (Primitive<int> j = Primitive<int>(0); j < size; j += Primitive<int>(1))
				{
				std::cout << String("[") << arr[i][j] << String("]");
				if (j != size - Primitive<int>(1))
				{
					std::cout << String(", ");
				}
				}
			if (i != size - Primitive<int>(1))
			{
				b += String(", ");
			}
			}
		}
	std::cout << b << std::endl;
	}
int main()
{
	return prelude().getValue();
}
