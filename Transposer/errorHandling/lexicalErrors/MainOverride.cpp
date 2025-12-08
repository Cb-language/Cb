#include "MainOverride.h"

MainOverride::MainOverride(const Token& token) : Error(token, "overriding prelude (cannot have multiple preludes)")
{
}
