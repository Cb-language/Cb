#include "IllegalDotOpError.h"

IllegalDotOpError::IllegalDotOpError(const Token& token, const std::string& left, const std::string& right)
    : Error(token, "Cant do: "+ left + "\\" + right + " because " + right + " doesn't exists in " + left + " or not accessible")
{
}

IllegalDotOpError::IllegalDotOpError(const Token& token, const std::string& left, const std::string& right,
    const std::string& leftType) : Error(token, "Cant do: "+ left + "\\" + right + " because " + right + " doesn't exists in " + leftType + " or not accessible")
{
}
