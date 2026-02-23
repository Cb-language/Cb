#include "ClassDeclStmt.h"

#include <ranges>

#include "FuncDeclStmt.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/SymbolTable.h"

ClassDeclStmt::ClassDeclStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
    const std::wstring& name, std::vector<Field>& fields, std::vector<Method>& methods, std::vector<Ctor>& ctors,
    const bool isInhereting, const std::wstring& inheritingPublic, const std::wstring& inheretingName)
    : Stmt(token, scope, funcDecl, currClass), name(name), isInhereting(isInhereting), inheritingPublic(inheritingPublic), inheretingName(inheretingName)
{
    for (auto& [isPublic, func] : fields) this->fields.emplace_back(isPublic, std::move(func));
    for (auto& [isPublic, method] : methods) this->methods.emplace_back(isPublic, std::move(method));
    for (auto& [isPublic, ctor] : ctors) this->ctors.emplace_back(isPublic, std::move(ctor));
}

void ClassDeclStmt::analyze() const
{
    for (const auto& field : fields | std::views::values) field->analyze();
    for (const auto& method : methods | std::views::values) method->analyze();

    if (isInhereting)
    {
        if (inheretingName.empty())
        {
            throw HowDidYouGetHere(token);
        }
        if (inheritingPublic != L"tutti" || inheritingPublic == L"section" || inheritingPublic == L"sordino")
        {
            throw HowDidYouGetHere(token);
        }
    }
}

std::string ClassDeclStmt::translateToCpp() const
{
    std::ostringstream oss;

    for (const auto& ctor : ctors | std::views::values) oss << ctor->translateToCpp() << std::endl;
    for (const auto& method : methods | std::views::values) oss << method->translateToCppClass(name) << std::endl;
    return oss.str();
}

std::string ClassDeclStmt::translateToH() const
{
    std::ostringstream oss;
    std::ostringstream privates;
    std::ostringstream publics;

    privates << "private:";
    publics << "public:";

    oss << "class " << Utils::wstrToStr(name);
    if (isInhereting)
    {
        oss << " : ";
        if (inheritingPublic == L"tutti")
        {
            oss << "public ";
        }
        else if (inheritingPublic == L"section")
        {
            oss << "protected ";
        }
        else
        {
            oss << "private ";
        }

        oss << Utils::wstrToStr(inheretingName);
    }
    oss << std::endl << "{" << std::endl;

    for (const auto& [isPublic, field] : fields)
    {
        if (isPublic) publics << std::endl << getTabs(1) << field->translateToCpp();
        else privates << std::endl << getTabs(1) << field->translateToCpp();
    }

    for (const auto& [isPublic, method] : methods)
    {
        if (isPublic) publics << std::endl << getTabs(1) << method->translateToH() << ";";
        else privates << std::endl << getTabs(1) << method->translateToH() << ";";
    }

    for (const auto& [isPublic, ctor] : ctors)
    {
        if (isPublic) publics << std::endl << getTabs(1) << ctor->translateToH() << ";";
        else privates << std::endl << getTabs(1) << ctor->translateToH() << ";";
    }

    oss << privates.str() << std::endl << std::endl << publics.str() << std::endl << "};" << std::endl;
    return oss.str();
}
