#include <iostream>
#include <string>
#include "/home/schafiii/CLionProjects/kiryatgat-1401-c/Transposer/test files/Pascal/includes/Array.h"
void pascal(unsigned int size);
int main()
{
	unsigned int size = 0;
	std::cout << "Enter pascal triangle level" << std::endl;
	std::cin >> size;
	pascal(size);
	return 0;
}

void pascal(unsigned int size)
{
	Array<Array<unsigned int>> arr = Array<Array<unsigned int>>(size, Array<unsigned int>(size, 0));
	arr[0][0] = 1;
	for (int i = 1; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (j == 0)
			{
				arr[i][j] = arr[i - 1][j];
			}
			else
			{
				arr[i][j] = arr[i - 1][j - 1] + arr[i - 1][j];
			}
		}
	}
	for (int i = 0; i < size; i++)
	{
		std::cout << "[";
		for (int j = 0; j < size; j++)
		{
			std::cout << "[" << arr[i][j] << "]";
			if (j != size - 1)
			{
				std::cout << ", ";
			}
		}
		std::string b = "]";
		if (i != size - 1)
		{
			b += ", ";
		}
		std::cout << b << std::endl;
	}
}
