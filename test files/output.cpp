#include <iostream>
#include <string>

int main()
{
	int i = (((33) / (2)) + (1)) % (2);
	std::string s = std::string("Hello") + std::string(" world!");
	std::string str = s;
	str += s;
	double f = (i) - (0.5);
	char c = 0;
	std::cin >> c >> f;

	return 0;
}
