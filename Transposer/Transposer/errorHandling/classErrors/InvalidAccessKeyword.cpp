#include "InvalidAccessKeyword.h"

InvalidAccessKeyword::InvalidAccessKeyword(const Token& token)
    : Error(token, "invalid Access keyword, use tutti, sordino or section")
{
}
