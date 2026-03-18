#include "StaticFuncCantVirtual.h"

StaticFuncCantVirtual::StaticFuncCantVirtual(const Token& token) : Error(token, "A static function can't also be virtual")
{
}
