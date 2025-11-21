#include <iostream>
#include <string>

int main()
{

	int i = (static_cast<double>(33) / static_cast<double>(2 * 5) + 1) * 3;
	std::string s = std::string("Hello") + std::string(" world!");
	std::string str = s;
	str += s;
	double f = i - 0.5;
	char c = 0;
	std::cin >> c >> f;
	i++;
	i--;
	i = 0;
	std::cout  << i;
	return 0;
}
