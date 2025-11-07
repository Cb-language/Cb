#include <iostream>
#include <boost/regex.hpp>

int main()
{
    std::string text = "My email is example@test.com";
    boost::regex pattern(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}\b)");

    if (boost::regex_search(text, pattern))
        std::cout << "Found an email address!" << std::endl;
    else
        std::cout << "No email found." << std::endl;

    return 0;
}