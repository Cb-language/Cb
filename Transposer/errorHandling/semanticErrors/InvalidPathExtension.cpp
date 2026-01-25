#include "InvalidPathExtension.h"

InvalidPathExtension::InvalidPathExtension(const Token &token) : Error(token, "invalid file extension, please include .cb or .stem only")
{
}
