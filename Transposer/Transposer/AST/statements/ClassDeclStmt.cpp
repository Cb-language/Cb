#include "ClassDeclStmt.h"

#include <ranges>

#include "FuncDeclStmt.h"

ClassDeclStmt::ClassDeclStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, const std::wstring& name, std::vector<Field>& fields,
                             std::vector<Method>& methods) : Stmt(token, scope, funcDecl), name(name)
{
    for (auto& [isPublic, func] : fields) this->fields.emplace_back(isPublic, std::move(func));
    for (auto& [isPublic, method] : methods) this->methods.emplace_back(isPublic, std::move(method));
}

void ClassDeclStmt::analyze() const
{
    for (const auto& field : fields | std::views::values) field->analyze();
    for (const auto& method : methods | std::views::values) method->analyze();
}

std::string ClassDeclStmt::translateToCpp() const
{
    std::ostringstream oss;

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

    oss << "class " << Utils::wstrToStr(name) << std::endl << "{" << std::endl;

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

    oss << privates.str() << std::endl << std::endl << publics.str() << std::endl << "};" << std::endl;
    return oss.str();
}
