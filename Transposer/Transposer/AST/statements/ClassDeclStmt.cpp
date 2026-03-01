#include "ClassDeclStmt.h"

#include <ranges>

#include "FuncDeclStmt.h"
#include "errorHandling/how/HowDidYouGetHere.h"
#include "other/SymbolTable.h"

std::string ClassDeclStmt::generateToString() const
{
    std::ostringstream oss;
    const std::string strName = Utils::wstrToStr(name);

    oss << "std::string " << strName <<"::toString() const" << std::endl <<
        "{" << std::endl <<
            "\tstd::string str = \"" << strName << "{\";" << std::endl;

    for (const auto& field : fields | std::views::values)
    {
        const std::string fieldName = Utils::wstrToStr(field->getVar().getName());
        oss << "\tstr += \"" << fieldName << " = \" + " << fieldName  << R"(.toString() + "\n";)" << std::endl;
    }
    if (auto parent = currClass->getParent()) oss << "\tstr += " << Utils::wstrToStr(parent->getClass().getClassName()) << "::toString();" << std::endl;
    oss << "\tstr += \"}\";" << std::endl;
    oss << "\treturn str;" << std::endl << "}" << std::endl;
    return oss.str();
}

std::string ClassDeclStmt::generateEquals() const
{
    std::ostringstream oss;
    const std::string strName = Utils::wstrToStr(name);
    oss << "Primitive<bool> " << strName << "::equals(const Object& other) const" << std::endl <<
    "{" << std::endl <<
        "\tif (this == &other) return Primitive<bool>(true);" << std::endl <<
        std::endl <<
        "\tif (const " << strName << "* otherPtr = dynamic_cast<const " << strName << "*>(&other))" << std::endl <<
        "\t{" << std::endl <<
            "\t\tif (";

    if (auto parent = currClass->getParent()) oss << Utils::wstrToStr(parent->getClass().getClassName()) << "::equals(other)";
    else oss << "true"; // shouldn't get here tho

    for (const auto& field : fields | std::views::values)
    {
        const std::string fieldName = Utils::wstrToStr(field->getVar().getName());
        oss << " && ";
        oss << fieldName << " == otherPtr->" << fieldName;
    }
    oss << ") return Primitive<bool>(true);" << std::endl <<
        "\t}" << std::endl <<
        "\treturn Primitive<bool>(false);" << std::endl <<
        "}" << std::endl;

    return oss.str();
}

ClassDeclStmt::ClassDeclStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass,
                             const std::wstring& name, std::vector<Field>& fields, std::vector<Method>& methods, std::vector<Ctor>& ctors,
                             const bool isInheriting, const std::wstring& inheritingPublic, const std::wstring& inheritingName)
    : Stmt(token, scope, funcDecl, currClass), name(name), isInheriting(isInheriting), inheritingPublic(inheritingPublic), inheritingName(inheritingName)
{
    for (auto& [isPublic, func] : fields) this->fields.emplace_back(isPublic, std::move(func));
    for (auto& [isPublic, method] : methods) this->methods.emplace_back(isPublic, std::move(method));
    for (auto& [isPublic, ctor] : ctors)
    {
        if (!hasEmptyCtor && ctor->getConstractor().getArgs().empty()) hasEmptyCtor = true;
        this->ctors.emplace_back(isPublic, std::move(ctor));
    }

    if (!hasEmptyCtor)
    {
        std::vector<Var> emptyArgs;
        std::vector<std::unique_ptr<Stmt>> emptyBodyStmts;
        auto ctor = std::make_unique<ConstractorDeclStmt>(
                token,
                scope,
                currClass,
                name,
                std::move(emptyArgs)
            );
        ctor->setBody(std::make_unique<BodyStmt>(
                token,
                scope,
                nullptr,
                currClass,
                emptyBodyStmts
            )
        );

        this->ctors.emplace_back(PUBLIC, std::move(ctor));
    }
}

void ClassDeclStmt::analyze() const
{
    for (const auto& field : fields | std::views::values) field->analyze();
    for (const auto& method : methods | std::views::values) method->analyze();
    for (const auto& ctor : ctors | std::views::values) ctor->analyze();

    if (isInheriting)
    {
        if (inheritingName.empty())
        {
            throw HowDidYouGetHere(token);
        }
        if (inheritingPublic != L"tutti" && inheritingPublic != L"section" && inheritingPublic != L"sordino")
        {
            throw HowDidYouGetHere(token);
        }
    }
}

std::string ClassDeclStmt::translateToCpp() const
{
    std::ostringstream oss;

    for (const auto& ctor : ctors | std::views::values) oss << ctor->translateToCpp() << std::endl;

    oss << generateEquals() << std::endl;
    oss << generateToString() << std::endl;

    for (const auto& method : methods | std::views::values) oss << method->translateToCppClass(name) << std::endl;
    return oss.str();
}

std::string ClassDeclStmt::translateToH() const
{
    std::ostringstream oss;
    std::ostringstream privates;
    std::ostringstream publics;
    std::ostringstream protecteds;

    privates << "private:";
    publics << "public:";
    protecteds << "protected: ";

    oss << "class " << Utils::wstrToStr(name);
    if (isInheriting)
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

        oss << Utils::wstrToStr(inheritingName);
    }
    else
    {
        oss << " : public Object";
    }
    oss << std::endl << "{" << std::endl;

    for (const auto& [isPublic, field] : fields)
    {
        if (isPublic == PUBLIC) publics << std::endl << getTabs(1) << field->translateToCpp();
        else if (isPublic == PROTECTED) protecteds << std::endl << getTabs(1) << field->translateToCpp();
        else privates << std::endl << getTabs(1) << field->translateToCpp();
    }

    for (const auto& [isPublic, method] : methods)
    {
        if (isPublic == PUBLIC) publics << std::endl << getTabs(1) << method->translateToH() << ";";
        else if (isPublic == PROTECTED) protecteds << std::endl << getTabs(1) << method->translateToH() << ";";
        else privates << std::endl << getTabs(1) << method->translateToH() << ";";
    }

    for (const auto& [isPublic, ctor] : ctors)
    {
        if (isPublic == PUBLIC) publics << std::endl << getTabs(1) << ctor->translateToH() << ";";
        else if (isPublic == PROTECTED) protecteds << std::endl << getTabs(1) << ctor->translateToH() << ";";
        else privates << std::endl << getTabs(1) << ctor->translateToH() << ";";
    }

    publics << "std::string toString() const override;" << std::endl << "Primitive<bool> equals(const Object& other) const override;" << std::endl;

    oss << privates.str() << std::endl << std::endl << publics.str() << std::endl << protecteds.str() << "};" << std::endl;
    return oss.str();
}

bool ClassDeclStmt::getHasEmptyCtor() const
{
    return hasEmptyCtor;
}
