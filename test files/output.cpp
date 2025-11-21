#include <iostream>
#include <string>

int main()
{
	int i = (static_cast<double>(33) / static_cast<double>(2 * 5) + 1) * 3;
	std::string s = std::string("Hello") + std::string(" world!");
	std::string str = s;
	str += s;
	double f = i - 0.5;
	char c = 67;
	i++;
	i--;
	i = 0;
	std::cout << "const" << std::endl;
	std::cout << i << ' ' << f << '\n' << c << "const" << std::endl;
	std::cout << 'c' << std::endl;
	std::cin >> c >> f;
	return 0;
}
