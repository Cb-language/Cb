#include "InstantiateAbstractClass.h"

InstantiateAbstractClass::InstantiateAbstractClass(const Token& token) : Error(token, "Cannot create an instance of an abstract class")
{
}
